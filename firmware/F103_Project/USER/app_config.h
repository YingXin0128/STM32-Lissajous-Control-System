#ifndef __APP_CONFIG_H
#define __APP_CONFIG_H

/*
 * AD9910 amplitude calibration from the preceding oscilloscope test.
 * Nominal Vpp values use 0.1 mV units.
 */
#define APP_LEVEL_4V_ASF                  16383U
#define APP_LEVEL_4V_NOMINAL_MV_X10      6660U
#define APP_LEVEL_3V_ASF                  12300U
#define APP_LEVEL_3V_NOMINAL_MV_X10      5000U
#define APP_LEVEL_2V_ASF                  8192U
#define APP_LEVEL_2V_NOMINAL_MV_X10      3330U
#define APP_LEVEL_1V_ASF                  4096U
#define APP_LEVEL_1V_NOMINAL_MV_X10      1665U

/*
 * The contest source can only generate 1..100 kHz in 100 Hz steps.
 * The raw TIM2 measurement is rounded to the nearest legal source point.
 * A half-step margin keeps 999.9 Hz and 100000.1 Hz measurements usable,
 * while rejecting signals that cannot belong to the stated source range.
 */
#define APP_MIN_REFERENCE_MHZ             1000000UL
#define APP_MAX_REFERENCE_MHZ             100000000UL
#define APP_REFERENCE_STEP_MHZ            100000UL
#define APP_REFERENCE_HALF_STEP_MHZ       50000UL

/*
 * Manual mode begins with this legal source point and keeps output enabled.
 * If PA0 later becomes invalid, the last valid target is held instead of
 * setting ASF to zero.
 */
#define APP_DEFAULT_REFERENCE_MHZ         10000000UL

/* Main-loop service interval used by the time-aware PI integrator. */
#define APP_CONTROL_UPDATE_MS             5U

/*
 * Phase-error driven digital PLL.
 *
 * Acquisition and fine modes first bring the DDS within one FTW of the
 * requested frequency.  Phase-lock mode then compares the F407-measured
 * relative phase with target_phase_millidegrees.  On entry, or after a large
 * sustained disturbance, a bounded POW correction aligns the measured phase
 * to the external reference.  POW is then frozen and the steady-state loop
 * changes frequency only.  A first-order pulse-density modulator selects
 * adjacent FTWs so the average frequency can be finer than one 0.232831 Hz
 * FTW step at a 1 GHz SYSCLK.
 */
#define APP_AD9910_SYSCLK_HZ              1000000000UL
#define APP_F103_FIRMWARE_TAG             0x0226U
#define APP_MEASUREMENT_FRAME_MS          20U
#define APP_FEEDBACK_MEDIAN_SAMPLES       3U
#define APP_FEEDBACK_IIR_DIVISOR          4U
#define APP_LOCKED_FEEDBACK_OUTLIER_MHZ   5000L
#define APP_FEEDBACK_OUTLIER_CONFIRM_FRAMES 5U

#define APP_ACQUIRE_TO_FINE_MHZ           500L
#define APP_ACQUIRE_STABLE_FRAMES         5U
#define APP_ACQUIRE_COMMIT_FRAMES         2U
#define APP_ACQUIRE_KP_NUMERATOR          1L
#define APP_ACQUIRE_KP_DENOMINATOR        2L
#define APP_ACQUIRE_MAX_STEP_FTW          128L

#define APP_FINE_WINDOW_FRAMES            10U
#define APP_FINE_ADJUST_THRESHOLD_MHZ     130L
#define APP_FINE_MAX_STEP_FTW             4L
#define APP_FINE_LOCK_WINDOWS             5U

#define APP_LOCK_GROSS_ERROR_MHZ          750L
#define APP_LOCK_GROSS_FRAMES             3U

/*
 * Steady-state DPLL tuning in frequency-mHz per phase-degree.
 * Kp = 2 mHz/degree, Ki = 0.2 mHz/(degree*second).
 * v2.2.0 freezes the integer-FTW centre before lock.  A timestamped one-second
 * phase-slope observer estimates residual frequency error without following
 * each noisy frequency measurement.  POW is used once for initial alignment
 * and remains frozen afterwards.  v2.2.6 keeps the proven +/-500 mHz range
 * below 50 kHz and extends the correction range to +/-1000 mHz for every
 * shape at 50 kHz and above.  Kp, Ki and lock thresholds are unchanged.
 */
#define APP_DPLL_PHASE_FILTER_DIVISOR      8L
#define APP_DPLL_KP_NUMERATOR              2L
#define APP_DPLL_KP_DENOMINATOR            1L
#define APP_DPLL_KI_NUMERATOR              1L
#define APP_DPLL_KI_DENOMINATOR            5L
#define APP_DPLL_MAX_CORRECTION_MHZ        500L
#define APP_DPLL_HIGH_FREQ_LIMIT_MHZ       1000L
#define APP_DPLL_HIGH_FREQ_THRESHOLD_MHZ   50000000UL
#define APP_DPLL_PHASE_DEADBAND_MDEG       200L
#define APP_DPLL_INTEGRAL_ERROR_MDEG       20000L
#define APP_DPLL_MAX_STEP_MHZ              2L
#define APP_DPLL_SLOPE_WINDOW_FRAMES       50U
#define APP_DPLL_SLOPE_GAIN_NUMERATOR      1L
#define APP_DPLL_SLOPE_GAIN_DENOMINATOR    2L
#define APP_DPLL_SLOPE_MAX_NUDGE_MHZ       100L
#define APP_DPLL_PHASE_LOCK_MDEG           1500L
#define APP_DPLL_PHASE_LOCK_FRAMES         75U

/*
 * Measured initial phase alignment.  One full-range POW correction is made
 * after the filtered error remains outside four degrees for 200 ms.  POW is
 * then frozen for the rest of that target/mode run; the DPLL preserves phase
 * by frequency correction only.
 */
#define APP_PHASE_ALIGN_TRIGGER_MDEG       4000L
#define APP_PHASE_ALIGN_CONFIRM_FRAMES     10U
#define APP_PHASE_ALIGN_MAX_STEP_MDEG      180000L

/*
 * Static phase mismatch of the two ADC conditioning paths.
 * corrected_phase = measured_phase - offset.  Leave at zero initially.
 * If the screen locks at 90 degrees but the oscilloscope measures phi,
 * set this to (90 - phi) * 1000 for circle-mode calibration.
 */
#define APP_DPLL_PHASE_SENSOR_OFFSET_MDEG  0L

#define APP_PHASE_MAX_STEP_MDEG           30000L
#define APP_PHASE_MAX_SEQUENCE_GAP        5U
#define APP_TIMESTAMP_MAX_GAP_US          110000UL
#define APP_FTW_SETTLE_FRAMES             1U
#define APP_LEARNED_PPB_LIMIT             20000000L

#define APP_MAX_TARGET_MHZ                200000000UL
#define APP_MAX_COMMAND_MHZ               205000000UL

#endif
