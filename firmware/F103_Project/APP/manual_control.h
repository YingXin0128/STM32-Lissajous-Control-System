#ifndef __MANUAL_CONTROL_H
#define __MANUAL_CONTROL_H

#include "app_types.h"
#include "signal_capture.h"

typedef enum
{
    MANUAL_CONTROL_STOPPED = 0,
    MANUAL_CONTROL_HOLD_REFERENCE,
    MANUAL_CONTROL_OPEN_LOOP,
    MANUAL_CONTROL_CLOSED_LOOP
} ManualControl_State;

typedef enum
{
    MANUAL_LOOP_ACQUIRE = 0,
    MANUAL_LOOP_FINE,
    MANUAL_LOOP_LOCKED
} ManualControl_LoopMode;

typedef struct
{
    ManualControl_State state;
    App_Shape shape;
    App_Voltage voltage;
    uint32_t target_frequency_millihz;
    uint32_t command_frequency_millihz;
    uint32_t feedback_frequency_millihz;
    int32_t target_phase_millidegrees;
    int32_t command_phase_millidegrees;
    int32_t frequency_error_millihz;
    int32_t proportional_term_millihz;
    int32_t integral_term_millihz;
    int32_t frequency_correction_millihz;
    int32_t dpll_applied_correction_millihz;
    int32_t dpll_correction_limit_millihz;
    int32_t relative_phase_millidegrees;
    int32_t phase_error_millidegrees;
    int32_t phase_slip_error_millihz;
    int32_t learned_clock_ppb;
    uint32_t filtered_feedback_frequency_millihz;
    uint32_t command_ftw;
    uint32_t io_update_count;
    uint32_t phase_slope_update_count;
    uint32_t feedback_outlier_frames;
    uint32_t gross_relock_count;
    uint16_t asf;
    uint16_t nominal_vpp_mv_x10;
    uint8_t pll_locked;
    uint8_t reference_held;
    uint8_t phase_locked;
    uint8_t phase_aligned;
    uint8_t dpll_correction_saturated;
    ManualControl_LoopMode loop_mode;
} ManualControl_Status;

void ManualControl_Init(void);
void ManualControl_Start(App_Shape shape, App_Voltage voltage);
void ManualControl_Stop(void);
void ManualControl_Update(const SignalCapture_Snapshot *capture);
void ManualControl_GetStatus(ManualControl_Status *status);
uint8_t ManualControl_IsActive(void);

const char *ManualControl_ShapeName(App_Shape shape);
const char *ManualControl_VoltageName(App_Voltage voltage);

#endif
