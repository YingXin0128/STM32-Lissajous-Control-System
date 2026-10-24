/*
 * 控制端阅读入口：测量接收 → 控制器更新 → LCD/按键/诊断调度。
 * 核心算法在 APP/manual_control.c；本文件主要负责初始化顺序和任务调用。
 */
#include "stm32_config.h"
#include "stdio.h"
#include "usart.h"
#include "debug_uart.h"
#include "board.h"
#include "AD9910.h"
#include "signal_capture.h"
#include "manual_control.h"
#include "auto_mode.h"
#include "menu.h"
#include "app_config.h"

#define CONTROL_STALE_CHECK_MS    10UL
#define DISPLAY_SLICE_MS          20UL
#define DIAGNOSTIC_UPDATE_MS      100UL

static volatile uint32_t app_millis;

static void App_TimebaseInit(void)
{
    TIM_TimeBaseInitTypeDef timer;
    NVIC_InitTypeDef nvic;

    app_millis = 0UL;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    TIM_TimeBaseStructInit(&timer);
    timer.TIM_Prescaler = 71U;
    timer.TIM_CounterMode = TIM_CounterMode_Up;
    timer.TIM_Period = 999U;
    timer.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM6, &timer);
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

    nvic.NVIC_IRQChannel = TIM6_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 2U;
    nvic.NVIC_IRQChannelSubPriority = 1U;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
    TIM_Cmd(TIM6, ENABLE);
}

void TIM6_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
        app_millis++;
    }
}

static uint8_t App_TimeReached(uint32_t now, uint32_t deadline)
{
    return (((int32_t)(now - deadline)) >= 0L) ? 1U : 0U;
}

static void App_SendCsvHeader(void)
{
    static const char header[] =
        "ms,seq,timestamp_us,loop,phase_valid,harmonic,phase_mdeg,"
        "phase_error_mdeg,slope_mhz,target_mhz,command_mhz,feedback_mhz,"
        "p_mhz,i_mhz,correction_mhz,slope_updates,phase_aligned,"
        "phase_locked,ad9910_pll,shape,voltage,asf,ref_vpp,fb_vpp,"
        "ref_min,ref_max,fb_min,fb_max,ref_clip,fb_clip,f407_analysis_us,"
        "f407_tag,f103_tag,crc_errors,"
        "missing_frames,rx_overflow_bytes,queue_overflows,f407_dma_errors,"
        "f407_dropped,rejected_frames,ref_clip_frames,fb_clip_frames,"
        "main_gap_ms,max_main_gap_ms,debug_dropped,"
        "dpll_applied_mhz,dpll_limit_mhz,dpll_saturated,"
        "ref_raw_mhz,ref_valid,fb_valid,clip_recovery,phase_reject_flags,"
        "ref_invalid_frames,fb_invalid_frames,phase_invalid_frames,"
        "clip_recovery_frames,feedback_outlier_frames,"
        "gross_relock_count\r\n";

    DebugUart_Send(header, (uint16_t)(sizeof(header) - 1U));
}

static void App_SendDiagnosticCsv(uint32_t now,
                                  const SignalCapture_Snapshot *capture,
                                  uint32_t main_gap_ms,
                                  uint32_t max_main_gap_ms)
{
    ManualControl_Status status;
    SignalCapture_Diagnostics diagnostics;
    char line[DEBUG_UART_BUFFER_SIZE];
    int length;

    ManualControl_GetStatus(&status);
    SignalCapture_GetDiagnostics(&diagnostics);
    length = snprintf(
        line,
        sizeof(line),
        "%lu,%lu,%lu,%u,%u,%u,%ld,%ld,%ld,%lu,%lu,%lu,%ld,%ld,%ld,"
        "%lu,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,"
        "%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
        "%ld,%ld,%u,%lu,%u,%u,%u,%u,%lu,%lu,%lu,%lu,%lu,%lu\r\n",
        (unsigned long)now,
        (unsigned long)capture->feedback_sequence,
        (unsigned long)capture->sample_timestamp_us,
        (unsigned int)status.loop_mode,
        (unsigned int)capture->relative_phase_valid,
        (unsigned int)capture->phase_harmonic,
        (long)status.relative_phase_millidegrees,
        (long)status.phase_error_millidegrees,
        (long)status.phase_slip_error_millihz,
        (unsigned long)status.target_frequency_millihz,
        (unsigned long)status.command_frequency_millihz,
        (unsigned long)capture->feedback_frequency_millihz,
        (long)status.proportional_term_millihz,
        (long)status.integral_term_millihz,
        (long)status.frequency_correction_millihz,
        (unsigned long)status.phase_slope_update_count,
        (unsigned int)status.phase_aligned,
        (unsigned int)status.phase_locked,
        (unsigned int)status.pll_locked,
        (unsigned int)status.shape,
        (unsigned int)status.voltage,
        (unsigned int)status.asf,
        (unsigned int)capture->reference_adc_vpp_counts,
        (unsigned int)capture->feedback_adc_vpp_counts,
        (unsigned int)capture->reference_adc_min,
        (unsigned int)capture->reference_adc_max,
        (unsigned int)capture->feedback_adc_min,
        (unsigned int)capture->feedback_adc_max,
        (unsigned int)capture->reference_clipped,
        (unsigned int)capture->feedback_clipped,
        (unsigned int)capture->f407_analysis_time_us,
        (unsigned int)capture->f407_firmware_tag,
        (unsigned int)APP_F103_FIRMWARE_TAG,
        (unsigned long)diagnostics.crc_errors,
        (unsigned long)diagnostics.missing_frames,
        (unsigned long)diagnostics.uart_dma_overflow_bytes,
        (unsigned long)diagnostics.queue_overflows,
        (unsigned long)diagnostics.f407_dma_overrun_frames,
        (unsigned long)diagnostics.f407_dropped_frames,
        (unsigned long)diagnostics.rejected_control_frames,
        (unsigned long)diagnostics.reference_clipped_frames,
        (unsigned long)diagnostics.feedback_clipped_frames,
        (unsigned long)main_gap_ms,
        (unsigned long)max_main_gap_ms,
        (unsigned long)DebugUart_GetDroppedMessages(),
        (long)status.dpll_applied_correction_millihz,
        (long)status.dpll_correction_limit_millihz,
        (unsigned int)status.dpll_correction_saturated,
        (unsigned long)capture->reference_raw_frequency_millihz,
        (unsigned int)capture->reference_valid,
        (unsigned int)capture->feedback_valid,
        (unsigned int)capture->clip_recovery,
        (unsigned int)capture->phase_reject_flags,
        (unsigned long)diagnostics.reference_invalid_frames,
        (unsigned long)diagnostics.feedback_invalid_frames,
        (unsigned long)diagnostics.phase_invalid_frames,
        (unsigned long)diagnostics.clip_recovery_frames,
        (unsigned long)status.feedback_outlier_frames,
        (unsigned long)status.gross_relock_count);
    if (length <= 0)
        return;
    if (length >= (int)sizeof(line))
        length = (int)sizeof(line) - 1;
    DebugUart_Send(line, (uint16_t)length);
}

int main(void)
{
    Board_KeyEvent key;
    SignalCapture_Snapshot capture;
    SignalCapture_Snapshot queued_capture;
    uint32_t now;
    uint32_t last_tick;
    uint32_t elapsed_ms;
    uint32_t next_control;
    uint32_t next_display;
    uint32_t next_diagnostic;
    uint32_t last_loop_entry;
    uint32_t main_gap_ms;
    uint32_t max_main_gap_ms;
    uint8_t manual_active;
    uint8_t manual_was_active;

    MY_NVIC_PriorityGroup_Config(NVIC_PriorityGroup_2);
    delay_init(72);
    Board_Init();
    DebugUart_Init();

    delay_ms(100U);
    Board_CalibrateConfirmIdle();

    ManualControl_Init();
    AutoMode_Init();
    Menu_Init();

    Init_AD9910();
    AD9910_SetToneMilliHz(10000000UL, 0L, 0U);
    /*
     * Start the binary measurement receiver only after all blocking LCD,
     * key-calibration and AD9910 startup work has completed.  Otherwise the
     * circular DMA legitimately fills before the parser can run and reports
     * thousands of artificial startup overflow bytes.
     */
    SignalCapture_Init();
    USARTx_Init(921600UL);
    App_TimebaseInit();
    App_SendCsvHeader();

    SignalCapture_GetSnapshot(&capture);
    last_tick = 0UL;
    next_control = CONTROL_STALE_CHECK_MS;
    next_display = DISPLAY_SLICE_MS;
    next_diagnostic = DIAGNOSTIC_UPDATE_MS;
    last_loop_entry = 0UL;
    main_gap_ms = 0UL;
    max_main_gap_ms = 0UL;
    manual_was_active = 0U;

    while (1)
    {
        now = app_millis;
        main_gap_ms = now - last_loop_entry;
        last_loop_entry = now;
        manual_active = ManualControl_IsActive();
        if ((manual_active != 0U) && (manual_was_active == 0U))
        {
            main_gap_ms = 0UL;
            max_main_gap_ms = 0UL;
        }
        else if ((manual_active != 0U) &&
                 (main_gap_ms > max_main_gap_ms))
        {
            max_main_gap_ms = main_gap_ms;
        }
        manual_was_active = manual_active;
        elapsed_ms = now - last_tick;
        if (elapsed_ms != 0UL)
        {
            SignalCapture_TickMs(elapsed_ms);
            last_tick = now;
            while (elapsed_ms != 0UL)
            {
                key = Board_KeysPoll1ms();
                if (key != BOARD_KEY_NONE)
                {
                    Board_MarkerToggle();
                    Menu_HandleKey(key);
                }
                elapsed_ms--;
            }
        }

        /* 先排空接收 DMA 数据并解析，再逐帧消费快照，避免仅取最新值丢掉控制输入。 */
        SignalCapture_Service();
        while (SignalCapture_TakeSnapshot(&queued_capture) != 0U)
        {
            capture = queued_capture;
            if (ManualControl_IsActive() != 0U)
                ManualControl_Update(&queued_capture);
            if (AutoMode_IsActive() != 0U)
                AutoMode_Service(&queued_capture);
        }
        SignalCapture_GetSnapshot(&capture);

        /* 无新帧时也周期检查数据是否过期；控制器内部通过序号避免重复积分。 */
        if (App_TimeReached(now, next_control) != 0U)
        {
            next_control = now + CONTROL_STALE_CHECK_MS;
            if (ManualControl_IsActive() != 0U)
                ManualControl_Update(&capture);
            if (AutoMode_IsActive() != 0U)
                AutoMode_Service(&capture);
        }

        /* 显示按片段刷新，缩短每次绘制占用主循环的时间。 */
        if (App_TimeReached(now, next_display) != 0U)
        {
            next_display = now + DISPLAY_SLICE_MS;
            Menu_RefreshRuntime(&capture);
        }

        /* CSV 走独立诊断串口，便于观察锁定、削顶、丢帧与主循环间隔。 */
        if (App_TimeReached(now, next_diagnostic) != 0U)
        {
            next_diagnostic = now + DIAGNOSTIC_UPDATE_MS;
            App_SendDiagnosticCsv(now,
                                  &capture,
                                  main_gap_ms,
                                  max_main_gap_ms);
        }

        __WFI();
    }
}
