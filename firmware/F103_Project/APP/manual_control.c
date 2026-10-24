/*
 * 频率/相位控制算法：本文件是 F103 的核心。
 * 阅读顺序：ManualControl_Update → Acquire → Fine → Dpll → WriteToneFtw。
 * Acquire 调 FTW，Fine 确认状态，Dpll 做 POW 对齐及 PI 调频；参数在 USER/app_config.h。
 * 频率单位为 mHz，相位为毫度；ASF 档位不等于物理电压闭环。
 */
#include "manual_control.h"
#include "AD9910.h"
#include "app_config.h"

typedef struct
{
    uint16_t asf;
    uint16_t nominal_vpp_mv_x10;
} VoltageSetting;

static const VoltageSetting voltage_settings[APP_VOLTAGE_COUNT] =
{
    {APP_LEVEL_4V_ASF, APP_LEVEL_4V_NOMINAL_MV_X10},
    {APP_LEVEL_3V_ASF, APP_LEVEL_3V_NOMINAL_MV_X10},
    {APP_LEVEL_2V_ASF, APP_LEVEL_2V_NOMINAL_MV_X10},
    {APP_LEVEL_1V_ASF, APP_LEVEL_1V_NOMINAL_MV_X10}
};

static ManualControl_Status control_status;
static uint8_t control_active;
static uint32_t feedback_window[APP_FEEDBACK_MEDIAN_SAMPLES];
static uint8_t feedback_window_count;
static uint8_t feedback_window_index;
static uint8_t feedback_filter_valid;
static uint32_t filtered_feedback_millihz;
static uint8_t feedback_sequence_valid;
static uint16_t last_feedback_sequence;
static uint8_t feedback_outlier_confirm_frames;
static int8_t feedback_outlier_direction;
static uint32_t feedback_outlier_frames;
static uint32_t gross_relock_count;

static uint32_t command_ftw;
static uint32_t last_command_ftw;
static uint16_t last_command_pow;
static uint16_t last_command_asf;
static uint32_t io_update_count;
static int32_t learned_clock_ppb;

static ManualControl_LoopMode loop_mode;
static uint8_t acquire_stable_frames;
static uint8_t acquire_commit_count;
static int64_t acquire_error_sum_millihz;
static uint8_t settle_frames_remaining;

static uint8_t phase_window_valid;
static uint16_t phase_last_sequence;
static int32_t phase_last_millidegrees;
static int64_t phase_unwrapped_millidegrees;
static uint32_t phase_last_timestamp_us;
static uint32_t phase_elapsed_us;
static uint8_t fine_lock_windows;
static uint8_t lock_gross_frames;

static uint32_t dpll_center_ftw;
static int64_t dpll_integral_numerator;
static uint32_t dpll_fraction_accumulator;
static uint8_t dpll_phase_filter_valid;
static int32_t dpll_filtered_phase_error_millidegrees;
static uint8_t dpll_sequence_valid;
static uint16_t dpll_last_sequence;
static uint32_t dpll_last_timestamp_us;
static uint8_t dpll_lock_frames;
static int32_t dpll_applied_correction_millihz;
static int32_t dpll_command_phase_millidegrees;
static uint8_t dpll_align_confirm_frames;
static uint8_t dpll_phase_aligned;
static uint32_t phase_slope_update_count;

static uint32_t Control_Abs32(int32_t value)
{
    if (value >= 0L)
        return (uint32_t)value;
    return (uint32_t)(-(int64_t)value);
}

static int32_t Control_Clamp32(int32_t value,
                               int32_t minimum,
                               int32_t maximum)
{
    if (value < minimum)
        return minimum;
    if (value > maximum)
        return maximum;
    return value;
}

static int32_t Control_WrapPhase(int32_t phase_millidegrees)
{
    while (phase_millidegrees > 180000L)
        phase_millidegrees -= 360000L;
    while (phase_millidegrees <= -180000L)
        phase_millidegrees += 360000L;
    return phase_millidegrees;
}

static int32_t Control_DpllCorrectionLimit(void)
{
    if (control_status.target_frequency_millihz >=
        APP_DPLL_HIGH_FREQ_THRESHOLD_MHZ)
        return APP_DPLL_HIGH_FREQ_LIMIT_MHZ;
    return APP_DPLL_MAX_CORRECTION_MHZ;
}

static uint32_t Control_FtwToMilliHz(uint32_t ftw)
{
    uint64_t numerator;

    /*
     * This project limits output to 205 kHz, so ftw * 1e12 remains safely
     * inside uint64_t.  Rounded conversion is used for display only.
     */
    numerator = (uint64_t)ftw * 1000000000000ULL +
                2147483648ULL;
    return (uint32_t)(numerator >> 32);
}

static uint32_t Control_FtwQ16ToMilliHz(uint64_t ftw_q16)
{
    uint32_t whole_ftw;
    uint32_t fractional_q16;
    uint32_t whole_millihz;
    uint32_t fractional_millihz;

    whole_ftw = (uint32_t)(ftw_q16 >> 16);
    fractional_q16 = (uint32_t)(ftw_q16 & 0xFFFFULL);
    whole_millihz = Control_FtwToMilliHz(whole_ftw);
    fractional_millihz =
        (uint32_t)(((uint64_t)fractional_q16 *
                    1000000000000ULL +
                    140737488355328ULL) /
                   281474976710656ULL);
    return whole_millihz + fractional_millihz;
}

static int32_t Control_ErrorToFtw(int32_t error_millihz)
{
    uint64_t magnitude;
    uint64_t numerator;
    uint32_t ftw;

    magnitude = (uint64_t)Control_Abs32(error_millihz);
    numerator = (magnitude << 32) + 500000000000ULL;
    ftw = (uint32_t)(numerator / 1000000000000ULL);
    return (error_millihz >= 0L) ? (int32_t)ftw :
                                   -(int32_t)ftw;
}

static void Control_UpdateDisplayedCorrection(void)
{
    int64_t correction;

    control_status.command_ftw = command_ftw;
    control_status.command_frequency_millihz =
        Control_FtwToMilliHz(command_ftw);
    correction =
        (int64_t)control_status.command_frequency_millihz -
        (int64_t)control_status.target_frequency_millihz;
    if (correction > 2147483647LL)
        correction = 2147483647LL;
    if (correction < -2147483647LL)
        correction = -2147483647LL;
    control_status.frequency_correction_millihz =
        (int32_t)correction;
}

/* 统一 DDS 写入口：组合 FTW（频率）、POW（相位）、ASF（幅度）并更新输出。 */
static void Control_WriteToneFtw(uint32_t ftw,
                                 int32_t phase_millidegrees,
                                 uint16_t asf)
{
    uint16_t pow;

    pow = AD9910_PowFromMilliDegrees(phase_millidegrees);
    if ((ftw == last_command_ftw) &&
        (pow == last_command_pow) &&
        (asf == last_command_asf))
        return;

    AD9910_ProfileWriteRaw(ftw, pow, asf);
    io_update_count++;
    last_command_ftw = ftw;
    last_command_pow = pow;
    last_command_asf = asf;
}

static void Control_ResetDpll(void)
{
    dpll_center_ftw = command_ftw;
    dpll_integral_numerator = 0LL;
    dpll_fraction_accumulator = 0UL;
    dpll_phase_filter_valid = 0U;
    dpll_filtered_phase_error_millidegrees = 0L;
    dpll_sequence_valid = 0U;
    dpll_last_sequence = 0U;
    dpll_last_timestamp_us = 0UL;
    dpll_lock_frames = 0U;
    dpll_applied_correction_millihz = 0L;
    dpll_command_phase_millidegrees =
        control_status.target_phase_millidegrees;
    dpll_align_confirm_frames = 0U;
    dpll_phase_aligned = 0U;
    control_status.phase_error_millidegrees = 0L;
    control_status.phase_locked = 0U;
    control_status.phase_aligned = 0U;
    control_status.dpll_applied_correction_millihz = 0L;
    control_status.dpll_correction_limit_millihz =
        Control_DpllCorrectionLimit();
    control_status.dpll_correction_saturated = 0U;
}

static int64_t Control_CorrectionMilliHzToFtwQ16(
    int32_t correction_millihz)
{
    uint64_t magnitude;
    uint64_t numerator;
    int64_t delta_q16;

    magnitude = (uint64_t)Control_Abs32(correction_millihz);
    numerator =
        magnitude * 281474976710656ULL +
        ((uint64_t)APP_AD9910_SYSCLK_HZ * 500ULL);
    delta_q16 =
        (int64_t)(numerator /
                  ((uint64_t)APP_AD9910_SYSCLK_HZ * 1000ULL));
    return (correction_millihz >= 0L) ? delta_q16 :
                                        -delta_q16;
}

/* 将修正量换成 Q16 FTW，由分数累加器在相邻整数 FTW 间选择，细化平均频率。 */
static void Control_ApplyDpllOutput(int32_t correction_millihz)
{
    int64_t desired_q16_signed;
    uint64_t desired_q16;
    uint64_t maximum_q16;
    uint32_t base_ftw;
    uint32_t fractional_q16;
    uint32_t applied_ftw;
    uint32_t effective_frequency_millihz;
    int64_t displayed_correction;

    maximum_q16 =
        ((uint64_t)AD9910_FtwFromMilliHz(APP_MAX_COMMAND_MHZ)) <<
        16;
    desired_q16_signed =
        ((int64_t)dpll_center_ftw << 16) +
        Control_CorrectionMilliHzToFtwQ16(correction_millihz);
    if (desired_q16_signed < 65536LL)
        desired_q16_signed = 65536LL;
    if ((uint64_t)desired_q16_signed > maximum_q16)
        desired_q16_signed = (int64_t)maximum_q16;
    desired_q16 = (uint64_t)desired_q16_signed;

    base_ftw = (uint32_t)(desired_q16 >> 16);
    fractional_q16 = (uint32_t)(desired_q16 & 0xFFFFULL);
    dpll_fraction_accumulator += fractional_q16;
    applied_ftw = base_ftw;
    if (dpll_fraction_accumulator >= 65536UL)
    {
        dpll_fraction_accumulator -= 65536UL;
        applied_ftw++;
    }

    command_ftw = applied_ftw;
    effective_frequency_millihz =
        Control_FtwQ16ToMilliHz(desired_q16);
    control_status.command_ftw = command_ftw;
    control_status.command_frequency_millihz =
        effective_frequency_millihz;
    displayed_correction =
        (int64_t)effective_frequency_millihz -
        (int64_t)control_status.target_frequency_millihz;
    displayed_correction = Control_Clamp32(
        (int32_t)displayed_correction,
        -2147483647L,
        2147483647L);
    control_status.frequency_correction_millihz =
        (int32_t)displayed_correction;
    control_status.dpll_applied_correction_millihz =
        correction_millihz;
    Control_WriteToneFtw(command_ftw,
                         dpll_command_phase_millidegrees,
                         control_status.asf);
}

static void Control_ResetFeedbackFilter(void)
{
    uint8_t index;

    feedback_window_count = 0U;
    feedback_window_index = 0U;
    feedback_filter_valid = 0U;
    filtered_feedback_millihz = 0UL;
    feedback_outlier_confirm_frames = 0U;
    feedback_outlier_direction = 0;
    for (index = 0U; index < APP_FEEDBACK_MEDIAN_SAMPLES; index++)
        feedback_window[index] = 0UL;
}

static void Control_ResetPhaseWindow(void)
{
    phase_window_valid = 0U;
    phase_last_sequence = 0U;
    phase_last_millidegrees = 0L;
    phase_unwrapped_millidegrees = 0LL;
    phase_last_timestamp_us = 0UL;
    phase_elapsed_us = 0UL;
}

static uint32_t Control_FeedbackMedian(void)
{
    uint32_t sorted[APP_FEEDBACK_MEDIAN_SAMPLES];
    uint32_t temporary;
    uint8_t index;
    uint8_t inner;

    for (index = 0U; index < APP_FEEDBACK_MEDIAN_SAMPLES; index++)
        sorted[index] = feedback_window[index];
    for (index = 1U; index < APP_FEEDBACK_MEDIAN_SAMPLES; index++)
    {
        temporary = sorted[index];
        inner = index;
        while ((inner > 0U) &&
               (sorted[inner - 1U] > temporary))
        {
            sorted[inner] = sorted[inner - 1U];
            inner--;
        }
        sorted[inner] = temporary;
    }
    return sorted[APP_FEEDBACK_MEDIAN_SAMPLES / 2U];
}

/* 反馈预处理：新帧去重、锁定期离群确认、3 点中值及 IIR；仅接受新反馈才推进控制。 */
static uint8_t Control_UpdateFeedbackFilter(
    const SignalCapture_Snapshot *capture,
    int32_t *error_millihz)
{
    int32_t outlier_delta_millihz;
    int8_t outlier_direction;
    uint16_t sequence;
    uint32_t median_feedback;

    if ((capture->feedback_valid == 0U) ||
        (error_millihz == 0))
        return 0U;
    sequence = (uint16_t)capture->feedback_sequence;
    if ((feedback_sequence_valid != 0U) &&
        (sequence == last_feedback_sequence))
        return 0U;
    feedback_sequence_valid = 1U;
    last_feedback_sequence = sequence;

    if ((loop_mode == MANUAL_LOOP_LOCKED) &&
        (feedback_filter_valid != 0U))
    {
        outlier_delta_millihz =
            (int32_t)capture->feedback_frequency_millihz -
            (int32_t)filtered_feedback_millihz;
        if (Control_Abs32(outlier_delta_millihz) >
            (uint32_t)APP_LOCKED_FEEDBACK_OUTLIER_MHZ)
        {
            if (feedback_outlier_frames < 0xFFFFFFFFUL)
                feedback_outlier_frames++;
            outlier_direction =
                (outlier_delta_millihz > 0L) ? 1 : -1;
            if (outlier_direction == feedback_outlier_direction)
            {
                if (feedback_outlier_confirm_frames < 255U)
                    feedback_outlier_confirm_frames++;
            }
            else
            {
                feedback_outlier_direction = outlier_direction;
                feedback_outlier_confirm_frames = 1U;
            }
            if (feedback_outlier_confirm_frames <
                APP_FEEDBACK_OUTLIER_CONFIRM_FRAMES)
                return 0U;

            /*
             * Five same-direction samples are a real frequency change, not
             * a one-frame crossing-count outlier.  Re-seed the normal
             * median/IIR path so gross-loss detection can reacquire it.
             */
            Control_ResetFeedbackFilter();
        }
        else
        {
            feedback_outlier_confirm_frames = 0U;
            feedback_outlier_direction = 0;
        }
    }

    feedback_window[feedback_window_index] =
        capture->feedback_frequency_millihz;
    feedback_window_index++;
    if (feedback_window_index >= APP_FEEDBACK_MEDIAN_SAMPLES)
        feedback_window_index = 0U;
    if (feedback_window_count < APP_FEEDBACK_MEDIAN_SAMPLES)
        feedback_window_count++;
    if (feedback_window_count < APP_FEEDBACK_MEDIAN_SAMPLES)
    {
        control_status.filtered_feedback_frequency_millihz =
            capture->feedback_frequency_millihz;
        return 0U;
    }

    median_feedback = Control_FeedbackMedian();
    if (feedback_filter_valid == 0U)
    {
        filtered_feedback_millihz = median_feedback;
        feedback_filter_valid = 1U;
    }
    else
    {
        filtered_feedback_millihz =
            ((filtered_feedback_millihz *
              (APP_FEEDBACK_IIR_DIVISOR - 1UL)) +
             median_feedback +
             (APP_FEEDBACK_IIR_DIVISOR / 2UL)) /
            APP_FEEDBACK_IIR_DIVISOR;
    }

    control_status.filtered_feedback_frequency_millihz =
        filtered_feedback_millihz;
    *error_millihz =
        (int32_t)control_status.target_frequency_millihz -
        (int32_t)filtered_feedback_millihz;
    control_status.frequency_error_millihz = *error_millihz;
    return 1U;
}

static void Control_CommitFtwDelta(int32_t delta_ftw)
{
    int64_t next_ftw;
    uint32_t maximum_ftw;

    maximum_ftw = AD9910_FtwFromMilliHz(APP_MAX_COMMAND_MHZ);
    next_ftw = (int64_t)command_ftw + (int64_t)delta_ftw;
    if (next_ftw < 1LL)
        next_ftw = 1LL;
    if (next_ftw > (int64_t)maximum_ftw)
        next_ftw = (int64_t)maximum_ftw;
    command_ftw = (uint32_t)next_ftw;
    Control_UpdateDisplayedCorrection();
    Control_WriteToneFtw(command_ftw,
                         control_status.target_phase_millidegrees,
                         control_status.asf);
    settle_frames_remaining = APP_FTW_SETTLE_FRAMES;
    Control_ResetPhaseWindow();
}

static void Control_SetTarget(uint32_t target_frequency)
{
    uint32_t nominal_ftw;

    control_status.target_frequency_millihz = target_frequency;
    control_status.frequency_error_millihz = 0L;
    control_status.proportional_term_millihz = 0L;
    control_status.integral_term_millihz = 0L;
    control_status.filtered_feedback_frequency_millihz = 0UL;
    control_status.relative_phase_millidegrees = 0L;
    control_status.phase_error_millidegrees = 0L;
    control_status.phase_slip_error_millihz = 0L;

    Control_ResetFeedbackFilter();
    Control_ResetPhaseWindow();
    acquire_stable_frames = 0U;
    acquire_commit_count = 0U;
    acquire_error_sum_millihz = 0LL;
    fine_lock_windows = 0U;
    lock_gross_frames = 0U;
    loop_mode = MANUAL_LOOP_ACQUIRE;
    settle_frames_remaining = APP_FTW_SETTLE_FRAMES;

    nominal_ftw = AD9910_FtwFromMilliHz(target_frequency);
    learned_clock_ppb = 0L;
    command_ftw = nominal_ftw;
    Control_UpdateDisplayedCorrection();
    Control_WriteToneFtw(command_ftw,
                         control_status.target_phase_millidegrees,
                         control_status.asf);
    Control_ResetDpll();
}

/* 按时间戳累计展开相位，换算残余频差；序号/时间异常或相位突跳则重置观测窗口。 */
static uint8_t Control_UpdatePhaseSlope(
    const SignalCapture_Snapshot *capture,
    uint8_t expected_harmonic,
    uint16_t required_frames,
    int32_t *slip_error_millihz)
{
    uint16_t sequence;
    uint16_t sequence_delta;
    uint32_t timestamp_delta_us;
    uint32_t required_us;
    int32_t phase;
    int32_t phase_delta;
    int64_t denominator;
    int64_t numerator;

    if (slip_error_millihz == 0)
    {
        Control_ResetPhaseWindow();
        return 0U;
    }
    if ((capture->relative_phase_valid == 0U) ||
        (capture->phase_harmonic != expected_harmonic))
        return 0U;
    if (settle_frames_remaining != 0U)
    {
        settle_frames_remaining--;
        Control_ResetPhaseWindow();
        return 0U;
    }

    sequence = (uint16_t)capture->feedback_sequence;
    phase = Control_WrapPhase(
        capture->relative_phase_millidegrees);
    control_status.relative_phase_millidegrees = phase;
    if (phase_window_valid == 0U)
    {
        phase_window_valid = 1U;
        phase_last_sequence = sequence;
        phase_last_millidegrees = phase;
        phase_last_timestamp_us = capture->sample_timestamp_us;
        phase_unwrapped_millidegrees = 0LL;
        phase_elapsed_us = 0UL;
        return 0U;
    }

    sequence_delta = (uint16_t)(sequence - phase_last_sequence);
    if (sequence_delta == 0U)
        return 0U;
    if (sequence_delta > APP_PHASE_MAX_SEQUENCE_GAP)
    {
        Control_ResetPhaseWindow();
        return 0U;
    }
    timestamp_delta_us =
        capture->sample_timestamp_us - phase_last_timestamp_us;
    if ((timestamp_delta_us == 0UL) ||
        (timestamp_delta_us > APP_TIMESTAMP_MAX_GAP_US))
    {
        Control_ResetPhaseWindow();
        return 0U;
    }

    phase_delta =
        Control_WrapPhase(phase - phase_last_millidegrees);
    phase_last_sequence = sequence;
    phase_last_millidegrees = phase;
    phase_last_timestamp_us = capture->sample_timestamp_us;
    if (Control_Abs32(phase_delta) >
        (uint32_t)APP_PHASE_MAX_STEP_MDEG)
    {
        Control_ResetPhaseWindow();
        return 0U;
    }

    phase_unwrapped_millidegrees += (int64_t)phase_delta;
    phase_elapsed_us += timestamp_delta_us;
    required_us =
        (uint32_t)required_frames *
        (uint32_t)APP_MEASUREMENT_FRAME_MS * 1000UL;
    if (phase_elapsed_us < required_us)
        return 0U;

    numerator = -phase_unwrapped_millidegrees * 1000000LL;
    denominator = 360LL * (int64_t)phase_elapsed_us;
    if (numerator >= 0LL)
        numerator += denominator / 2LL;
    else
        numerator -= denominator / 2LL;
    *slip_error_millihz = (int32_t)(numerator / denominator);
    control_status.phase_slip_error_millihz =
        *slip_error_millihz;
    phase_slope_update_count++;
    Control_ResetPhaseWindow();
    return 1U;
}

/* 捕获阶段：将直接频差换成有界 FTW 调整，连续进入误差范围后转 Fine。 */
static void Control_UpdateAcquire(int32_t direct_error_millihz)
{
    int32_t average_error;
    int32_t delta_ftw;

    if (Control_Abs32(direct_error_millihz) <=
        (uint32_t)APP_ACQUIRE_TO_FINE_MHZ)
    {
        if (acquire_stable_frames < 255U)
            acquire_stable_frames++;
        acquire_commit_count = 0U;
        acquire_error_sum_millihz = 0LL;
        if (acquire_stable_frames >= APP_ACQUIRE_STABLE_FRAMES)
        {
            loop_mode = MANUAL_LOOP_FINE;
            fine_lock_windows = 0U;
            Control_ResetPhaseWindow();
        }
        return;
    }

    acquire_stable_frames = 0U;
    acquire_error_sum_millihz += direct_error_millihz;
    acquire_commit_count++;
    if (acquire_commit_count < APP_ACQUIRE_COMMIT_FRAMES)
        return;

    average_error =
        (int32_t)(acquire_error_sum_millihz /
                  (int64_t)acquire_commit_count);
    delta_ftw = Control_ErrorToFtw(average_error);
    if (delta_ftw >= 0L)
        delta_ftw =
            (delta_ftw * APP_ACQUIRE_KP_NUMERATOR +
             (APP_ACQUIRE_KP_DENOMINATOR / 2L)) /
            APP_ACQUIRE_KP_DENOMINATOR;
    else
        delta_ftw =
            -(((-delta_ftw) * APP_ACQUIRE_KP_NUMERATOR +
               (APP_ACQUIRE_KP_DENOMINATOR / 2L)) /
              APP_ACQUIRE_KP_DENOMINATOR);
    delta_ftw = Control_Clamp32(
        delta_ftw,
        -APP_ACQUIRE_MAX_STEP_FTW,
        APP_ACQUIRE_MAX_STEP_FTW);
    control_status.proportional_term_millihz =
        (int32_t)((int64_t)delta_ftw * 233LL);
    if (delta_ftw != 0L)
        Control_CommitFtwDelta(delta_ftw);

    acquire_commit_count = 0U;
    acquire_error_sum_millihz = 0LL;
    Control_ResetFeedbackFilter();
}

/* 过渡确认阶段：检查相位有效、谐波一致及频差范围，满足帧数后进入 LOCKED。
 * 当前函数不直接修改 FTW，不能按名称推断这里还执行了额外细调算法。 */
static void Control_UpdateFine(
    const SignalCapture_Snapshot *capture,
    uint8_t expected_harmonic)
{
    if ((capture->relative_phase_valid == 0U) ||
        (capture->phase_harmonic != expected_harmonic) ||
        (Control_Abs32(control_status.frequency_error_millihz) >
         (uint32_t)APP_ACQUIRE_TO_FINE_MHZ))
    {
        loop_mode = MANUAL_LOOP_ACQUIRE;
        acquire_stable_frames = 0U;
        fine_lock_windows = 0U;
        return;
    }

    if (fine_lock_windows < 255U)
        fine_lock_windows++;
    if (fine_lock_windows >= APP_FINE_LOCK_WINDOWS)
    {
        loop_mode = MANUAL_LOOP_LOCKED;
        lock_gross_frames = 0U;
        Control_ResetDpll();
    }
}

/* 相位环：异常频差可触发重捕获；滤波相位后进行一次 POW 对齐，再由 PI 调频。
 * 积分采用测量时间戳，长窗相位斜率观测会有限度地修正积分状态。 */
static void Control_UpdateDpll(
    const SignalCapture_Snapshot *capture,
    uint8_t expected_harmonic,
    int32_t direct_error_millihz)
{
    uint16_t sequence;
    uint16_t sequence_delta;
    uint32_t timestamp_delta_us;
    int32_t relative_phase;
    int32_t raw_phase_error;
    int32_t filter_delta;
    int32_t control_phase_error;
    int32_t integral_phase_error;
    int32_t slope_error_millihz;
    int32_t slope_nudge_millihz;
    int64_t integral_denominator;
    int64_t integral_limit;
    int32_t integral_millihz;
    int32_t proportional_millihz;
    int32_t requested_correction_millihz;
    int32_t desired_correction_millihz;
    int32_t correction_step_millihz;
    int32_t correction_limit_millihz;
    int32_t phase_align_step_millidegrees;

    if (Control_Abs32(direct_error_millihz) >
        (uint32_t)APP_LOCK_GROSS_ERROR_MHZ)
    {
        if (lock_gross_frames < 255U)
            lock_gross_frames++;
    }
    else
    {
        lock_gross_frames = 0U;
    }
    if (lock_gross_frames >= APP_LOCK_GROSS_FRAMES)
    {
        if (gross_relock_count < 0xFFFFFFFFUL)
            gross_relock_count++;
        loop_mode = MANUAL_LOOP_ACQUIRE;
        acquire_stable_frames = 0U;
        acquire_commit_count = 0U;
        acquire_error_sum_millihz = 0LL;
        Control_ResetPhaseWindow();
        Control_ResetDpll();
        return;
    }

    if ((capture->relative_phase_valid == 0U) ||
        (capture->phase_harmonic != expected_harmonic))
    {
        dpll_lock_frames = 0U;
        control_status.phase_locked = 0U;
        return;
    }

    /* 积分状态使用整数缩放，避免每帧小修正直接截断。 */
    integral_denominator =
        1000000000LL * (int64_t)APP_DPLL_KI_DENOMINATOR;
    if (Control_UpdatePhaseSlope(
            capture,
            expected_harmonic,
            APP_DPLL_SLOPE_WINDOW_FRAMES,
            &slope_error_millihz) != 0U)
    {
        slope_nudge_millihz =
            (int32_t)(((int64_t)slope_error_millihz *
                       (int64_t)APP_DPLL_SLOPE_GAIN_NUMERATOR) /
                      (int64_t)APP_DPLL_SLOPE_GAIN_DENOMINATOR);
        slope_nudge_millihz = Control_Clamp32(
            slope_nudge_millihz,
            -APP_DPLL_SLOPE_MAX_NUDGE_MHZ,
            APP_DPLL_SLOPE_MAX_NUDGE_MHZ);
        dpll_integral_numerator +=
            (int64_t)slope_nudge_millihz *
            integral_denominator;
    }

    sequence = (uint16_t)capture->feedback_sequence;
    if (dpll_sequence_valid == 0U)
    {
        dpll_sequence_valid = 1U;
        dpll_last_sequence = sequence;
        dpll_last_timestamp_us = capture->sample_timestamp_us;
        sequence_delta = 0U;
        timestamp_delta_us = 0UL;
    }
    else
    {
        sequence_delta =
            (uint16_t)(sequence - dpll_last_sequence);
        if (sequence_delta == 0U)
            return;
        dpll_last_sequence = sequence;
        if (sequence_delta > APP_PHASE_MAX_SEQUENCE_GAP)
        {
            dpll_phase_filter_valid = 0U;
            dpll_lock_frames = 0U;
            control_status.phase_locked = 0U;
            return;
        }
        timestamp_delta_us =
            capture->sample_timestamp_us -
            dpll_last_timestamp_us;
        dpll_last_timestamp_us = capture->sample_timestamp_us;
        if ((timestamp_delta_us == 0UL) ||
            (timestamp_delta_us > APP_TIMESTAMP_MAX_GAP_US))
        {
            dpll_phase_filter_valid = 0U;
            dpll_lock_frames = 0U;
            control_status.phase_locked = 0U;
            return;
        }
    }

    relative_phase = Control_WrapPhase(
        capture->relative_phase_millidegrees -
        APP_DPLL_PHASE_SENSOR_OFFSET_MDEG);
    raw_phase_error = Control_WrapPhase(
        control_status.target_phase_millidegrees -
        relative_phase);
    control_status.relative_phase_millidegrees = relative_phase;

    if (dpll_phase_filter_valid == 0U)
    {
        dpll_filtered_phase_error_millidegrees =
            raw_phase_error;
        dpll_phase_filter_valid = 1U;
    }
    else
    {
        filter_delta = Control_WrapPhase(
            raw_phase_error -
            dpll_filtered_phase_error_millidegrees);
        dpll_filtered_phase_error_millidegrees =
            Control_WrapPhase(
                dpll_filtered_phase_error_millidegrees +
                filter_delta /
                APP_DPLL_PHASE_FILTER_DIVISOR);
    }
    control_status.phase_error_millidegrees =
        dpll_filtered_phase_error_millidegrees;

    /*
     * POW=90 degrees is relative to the AD9910 phase accumulator, not to the
     * external waveform on PC0.  Align the measured phase explicitly, then
     * freeze POW.  Frequency-only control below preserves that relationship.
     */
    if ((dpll_phase_aligned == 0U) &&
        (Control_Abs32(
                 dpll_filtered_phase_error_millidegrees) >
         (uint32_t)APP_PHASE_ALIGN_TRIGGER_MDEG))
    {
        if (dpll_align_confirm_frames < 255U)
            dpll_align_confirm_frames++;
    }
    else
    {
        dpll_align_confirm_frames = 0U;
    }
    if (dpll_align_confirm_frames >=
        APP_PHASE_ALIGN_CONFIRM_FRAMES)
    {
        phase_align_step_millidegrees = Control_Clamp32(
            dpll_filtered_phase_error_millidegrees,
            -APP_PHASE_ALIGN_MAX_STEP_MDEG,
            APP_PHASE_ALIGN_MAX_STEP_MDEG);
        dpll_command_phase_millidegrees = Control_WrapPhase(
            dpll_command_phase_millidegrees +
            phase_align_step_millidegrees);
        control_status.command_phase_millidegrees =
            dpll_command_phase_millidegrees;
        Control_WriteToneFtw(command_ftw,
                             dpll_command_phase_millidegrees,
                             control_status.asf);
        dpll_align_confirm_frames = 0U;
        dpll_phase_aligned = 1U;
        dpll_phase_filter_valid = 0U;
        dpll_lock_frames = 0U;
        control_status.phase_locked = 0U;
        Control_ResetPhaseWindow();
        return;
    }

    control_phase_error =
        dpll_filtered_phase_error_millidegrees;
    if (Control_Abs32(control_phase_error) <=
        (uint32_t)APP_DPLL_PHASE_DEADBAND_MDEG)
        control_phase_error = 0L;
    /* 积分前限制相位误差，随后按时间戳累积；总修正还受幅度和单次变化量限制。 */
    integral_phase_error = Control_Clamp32(
        control_phase_error,
        -APP_DPLL_INTEGRAL_ERROR_MDEG,
        APP_DPLL_INTEGRAL_ERROR_MDEG);
    if (timestamp_delta_us != 0UL)
    {
        dpll_integral_numerator +=
            (int64_t)integral_phase_error *
            (int64_t)timestamp_delta_us *
            (int64_t)APP_DPLL_KI_NUMERATOR;
    }
    correction_limit_millihz =
        Control_DpllCorrectionLimit();
    control_status.dpll_correction_limit_millihz =
        correction_limit_millihz;
    integral_limit =
        (int64_t)correction_limit_millihz *
        integral_denominator;
    if (dpll_integral_numerator > integral_limit)
        dpll_integral_numerator = integral_limit;
    if (dpll_integral_numerator < -integral_limit)
        dpll_integral_numerator = -integral_limit;
    integral_millihz =
        (int32_t)(dpll_integral_numerator /
                  integral_denominator);

    proportional_millihz =
        (int32_t)(((int64_t)
                   control_phase_error *
                   (int64_t)APP_DPLL_KP_NUMERATOR) /
                  (1000LL *
                   (int64_t)APP_DPLL_KP_DENOMINATOR));
    requested_correction_millihz =
        proportional_millihz + integral_millihz;
    desired_correction_millihz = Control_Clamp32(
        requested_correction_millihz,
        -correction_limit_millihz,
        correction_limit_millihz);
    control_status.dpll_correction_saturated =
        (desired_correction_millihz !=
         requested_correction_millihz) ? 1U : 0U;
    correction_step_millihz =
        desired_correction_millihz -
        dpll_applied_correction_millihz;
    correction_step_millihz = Control_Clamp32(
        correction_step_millihz,
        -APP_DPLL_MAX_STEP_MHZ,
        APP_DPLL_MAX_STEP_MHZ);
    dpll_applied_correction_millihz +=
        correction_step_millihz;
    control_status.proportional_term_millihz =
        proportional_millihz;
    control_status.integral_term_millihz =
        integral_millihz;

    if (Control_Abs32(
            dpll_filtered_phase_error_millidegrees) <=
        (uint32_t)APP_DPLL_PHASE_LOCK_MDEG)
    {
        if (dpll_lock_frames < 255U)
            dpll_lock_frames++;
    }
    else
    {
        dpll_lock_frames = 0U;
    }
    control_status.phase_locked =
        (dpll_lock_frames >= APP_DPLL_PHASE_LOCK_FRAMES) ?
        1U : 0U;
    Control_ApplyDpllOutput(
        dpll_applied_correction_millihz);
}

static uint32_t Control_DefaultTarget(App_Shape shape)
{
    uint32_t target;

    target = APP_DEFAULT_REFERENCE_MHZ;
    if (shape == APP_SHAPE_INFINITY)
        target *= 2UL;
    if (target > APP_MAX_TARGET_MHZ)
        target = APP_MAX_TARGET_MHZ;
    return target;
}

void ManualControl_Init(void)
{
    control_active = 0U;
    learned_clock_ppb = 0L;
    phase_slope_update_count = 0UL;
    feedback_outlier_frames = 0UL;
    gross_relock_count = 0UL;
    feedback_sequence_valid = 0U;
    last_feedback_sequence = 0U;
    last_command_ftw = 0xFFFFFFFFUL;
    last_command_pow = 0xFFFFU;
    last_command_asf = 0xFFFFU;
    command_ftw = 0UL;
    io_update_count = 0UL;
    Control_ResetFeedbackFilter();
    Control_ResetPhaseWindow();
    Control_ResetDpll();

    control_status.state = MANUAL_CONTROL_STOPPED;
    control_status.shape = APP_SHAPE_DIAGONAL;
    control_status.voltage = APP_VOLTAGE_4V;
    control_status.target_frequency_millihz = 0UL;
    control_status.command_frequency_millihz = 0UL;
    control_status.feedback_frequency_millihz = 0UL;
    control_status.target_phase_millidegrees = 0L;
    control_status.command_phase_millidegrees = 0L;
    control_status.frequency_error_millihz = 0L;
    control_status.proportional_term_millihz = 0L;
    control_status.integral_term_millihz = 0L;
    control_status.frequency_correction_millihz = 0L;
    control_status.dpll_applied_correction_millihz = 0L;
    control_status.dpll_correction_limit_millihz =
        APP_DPLL_MAX_CORRECTION_MHZ;
    control_status.relative_phase_millidegrees = 0L;
    control_status.phase_error_millidegrees = 0L;
    control_status.phase_slip_error_millihz = 0L;
    control_status.learned_clock_ppb = 0L;
    control_status.filtered_feedback_frequency_millihz = 0UL;
    control_status.command_ftw = 0UL;
    control_status.io_update_count = 0UL;
    control_status.phase_slope_update_count = 0UL;
    control_status.feedback_outlier_frames = 0UL;
    control_status.gross_relock_count = 0UL;
    control_status.asf = 0U;
    control_status.nominal_vpp_mv_x10 = 0U;
    control_status.pll_locked = 0U;
    control_status.reference_held = 1U;
    control_status.phase_locked = 0U;
    control_status.phase_aligned = 0U;
    control_status.dpll_correction_saturated = 0U;
    control_status.loop_mode = MANUAL_LOOP_ACQUIRE;
}

void ManualControl_Start(App_Shape shape, App_Voltage voltage)
{
    uint32_t default_target;

    if (shape >= APP_SHAPE_COUNT)
        shape = APP_SHAPE_DIAGONAL;
    if (voltage >= APP_VOLTAGE_COUNT)
        voltage = APP_VOLTAGE_4V;
    control_status.shape = shape;
    control_status.voltage = voltage;
    control_status.asf = voltage_settings[voltage].asf;
    control_status.nominal_vpp_mv_x10 =
        voltage_settings[voltage].nominal_vpp_mv_x10;
    control_status.target_phase_millidegrees =
        (shape == APP_SHAPE_CIRCLE) ? 90000L : 0L;
    control_status.command_phase_millidegrees =
        dpll_command_phase_millidegrees;
    control_status.feedback_frequency_millihz = 0UL;
    control_status.pll_locked = AD9910_IsPllLocked();
    control_status.reference_held = 1U;
    control_status.state = MANUAL_CONTROL_HOLD_REFERENCE;
    control_status.phase_locked = 0U;
    feedback_outlier_frames = 0UL;
    gross_relock_count = 0UL;
    control_status.feedback_outlier_frames = 0UL;
    control_status.gross_relock_count = 0UL;

    feedback_sequence_valid = 0U;
    default_target = Control_DefaultTarget(shape);
    control_active = 1U;
    Control_SetTarget(default_target);
}

void ManualControl_Stop(void)
{
    uint32_t muted_ftw;

    control_active = 0U;
    control_status.state = MANUAL_CONTROL_STOPPED;
    control_status.command_frequency_millihz = 0UL;
    control_status.phase_locked = 0U;
    control_status.dpll_correction_saturated = 0U;
    muted_ftw = AD9910_FtwFromMilliHz(APP_DEFAULT_REFERENCE_MHZ);
    Control_WriteToneFtw(muted_ftw, 0L, 0U);
}

/* 外部调用入口：更新目标与反馈，按 loop_mode 分派控制阶段，再汇总诊断状态。
 * 参考无效时保持最近目标；反馈无效时不推进闭环。 */
void ManualControl_Update(const SignalCapture_Snapshot *capture)
{
    uint8_t expected_harmonic;
    uint8_t reference_valid;
    uint32_t target_frequency;
    int32_t direct_error;
    uint8_t new_filtered_feedback;

    if ((control_active == 0U) || (capture == 0))
        return;
    control_status.pll_locked = AD9910_IsPllLocked();
    control_status.feedback_frequency_millihz =
        capture->feedback_frequency_millihz;

    reference_valid =
        ((capture->reference_valid != 0U) &&
         (capture->reference_frequency_millihz >=
          APP_MIN_REFERENCE_MHZ) &&
         (capture->reference_frequency_millihz <=
          APP_MAX_REFERENCE_MHZ)) ? 1U : 0U;
    expected_harmonic =
        (control_status.shape == APP_SHAPE_INFINITY) ? 2U : 1U;
    if (reference_valid != 0U)
    {
        target_frequency =
            capture->reference_frequency_millihz *
            (uint32_t)expected_harmonic;
        if (target_frequency > APP_MAX_TARGET_MHZ)
            target_frequency = APP_MAX_TARGET_MHZ;
        if (target_frequency !=
            control_status.target_frequency_millihz)
            Control_SetTarget(target_frequency);
        control_status.reference_held = 0U;
    }
    else
    {
        target_frequency =
            control_status.target_frequency_millihz;
        control_status.reference_held = 1U;
    }

    direct_error = 0L;
    new_filtered_feedback =
        Control_UpdateFeedbackFilter(capture, &direct_error);
    if ((capture->feedback_valid != 0U) &&
        (new_filtered_feedback != 0U))
    {
        if (loop_mode == MANUAL_LOOP_ACQUIRE)
            Control_UpdateAcquire(direct_error);
        else if (loop_mode == MANUAL_LOOP_FINE)
            Control_UpdateFine(capture, expected_harmonic);
        else
            Control_UpdateDpll(capture,
                               expected_harmonic,
                               direct_error);
    }
    else if (capture->feedback_valid == 0U)
    {
        control_status.frequency_error_millihz = 0L;
        control_status.filtered_feedback_frequency_millihz = 0UL;
        Control_ResetPhaseWindow();
    }

    control_status.loop_mode = loop_mode;
    if (loop_mode != MANUAL_LOOP_LOCKED)
        control_status.phase_locked = 0U;
    control_status.io_update_count = io_update_count;
    control_status.phase_slope_update_count = phase_slope_update_count;
    control_status.feedback_outlier_frames = feedback_outlier_frames;
    control_status.gross_relock_count = gross_relock_count;
    control_status.learned_clock_ppb = learned_clock_ppb;
    control_status.command_phase_millidegrees =
        (loop_mode == MANUAL_LOOP_LOCKED) ?
        dpll_command_phase_millidegrees :
        control_status.target_phase_millidegrees;
    control_status.phase_aligned = dpll_phase_aligned;
    if (control_status.reference_held != 0U)
        control_status.state = MANUAL_CONTROL_HOLD_REFERENCE;
    else if (capture->feedback_valid != 0U)
        control_status.state = MANUAL_CONTROL_CLOSED_LOOP;
    else
        control_status.state = MANUAL_CONTROL_OPEN_LOOP;
}

void ManualControl_GetStatus(ManualControl_Status *status)
{
    if (status != 0)
        *status = control_status;
}

uint8_t ManualControl_IsActive(void)
{
    return control_active;
}

const char *ManualControl_ShapeName(App_Shape shape)
{
    static const char *names[APP_SHAPE_COUNT] =
    {
        "DIAGONAL",
        "CIRCLE",
        "INFINITY"
    };

    if (shape >= APP_SHAPE_COUNT)
        return "UNKNOWN";
    return names[shape];
}

const char *ManualControl_VoltageName(App_Voltage voltage)
{
    static const char *names[APP_VOLTAGE_COUNT] =
    {
        "4V",
        "3V",
        "2V",
        "1V"
    };

    if (voltage >= APP_VOLTAGE_COUNT)
        return "?";
    return names[voltage];
}
