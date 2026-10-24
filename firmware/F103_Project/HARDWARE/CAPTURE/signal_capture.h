#ifndef __SIGNAL_CAPTURE_H
#define __SIGNAL_CAPTURE_H

#include "stm32f10x.h"

#define SIGNAL_CAPTURE_ADC_SAMPLE_RATE_HZ       800000UL
#define SIGNAL_CAPTURE_ADC_FRAME_SAMPLES        8000U
#define SIGNAL_CAPTURE_PUBLISH_PERIOD_MS         20U
#define SIGNAL_CAPTURE_WAVEFORM_SAMPLES         1024U
#define SIGNAL_CAPTURE_QUEUE_SIZE               32U

#define SIGNAL_CAPTURE_REJECT_REFERENCE_INVALID 0x01U
#define SIGNAL_CAPTURE_REJECT_FEEDBACK_INVALID  0x02U
#define SIGNAL_CAPTURE_REJECT_REFERENCE_CLIP    0x04U
#define SIGNAL_CAPTURE_REJECT_FEEDBACK_CLIP     0x08U
#define SIGNAL_CAPTURE_REJECT_CLIP_RECOVERY     0x10U
#define SIGNAL_CAPTURE_REJECT_DMA_OVERRUN       0x20U
#define SIGNAL_CAPTURE_REJECT_PHASE_ESTIMATOR   0x40U

typedef struct
{
    uint32_t reference_raw_frequency_millihz;
    uint32_t reference_frequency_millihz;
    uint32_t feedback_frequency_millihz;
    uint32_t reference_period_ticks;
    uint32_t feedback_period_ticks;
    uint32_t reference_sequence;
    uint32_t feedback_sequence;
    uint32_t waveform_sequence;
    uint32_t sample_timestamp_us;
    int32_t relative_phase_millidegrees;
    uint16_t reference_age_ms;
    uint16_t feedback_age_ms;
    uint16_t reference_adc_min;
    uint16_t reference_adc_max;
    uint16_t reference_adc_mean;
    uint16_t reference_adc_vpp_counts;
    uint16_t feedback_adc_min;
    uint16_t feedback_adc_max;
    uint16_t feedback_adc_vpp_counts;
    uint16_t f407_analysis_time_us;
    uint16_t f407_firmware_tag;
    uint8_t reference_valid;
    uint8_t relative_phase_valid;
    uint8_t feedback_valid;
    uint8_t phase_harmonic;
    uint8_t reference_clipped;
    uint8_t feedback_clipped;
    uint8_t clip_recovery;
    uint8_t phase_reject_flags;
    uint8_t dma_overrun;
} SignalCapture_Snapshot;

typedef struct
{
    uint32_t accepted_frames;
    uint32_t crc_errors;
    uint32_t format_errors;
    uint32_t sync_losses;
    uint32_t sequence_gap_events;
    uint32_t missing_frames;
    uint32_t queue_overflows;
    uint32_t uart_dma_overflow_bytes;
    uint32_t f407_dma_overrun_frames;
    uint32_t f407_dropped_frames;
    uint32_t reference_clipped_frames;
    uint32_t feedback_clipped_frames;
    uint32_t reference_invalid_frames;
    uint32_t feedback_invalid_frames;
    uint32_t phase_invalid_frames;
    uint32_t clip_recovery_frames;
    uint32_t rejected_control_frames;
} SignalCapture_Diagnostics;

void SignalCapture_Init(void);
void SignalCapture_Service(void);
void SignalCapture_Tick1ms(void);
void SignalCapture_TickMs(uint32_t elapsed_ms);
void SignalCapture_GetSnapshot(SignalCapture_Snapshot *snapshot);
uint8_t SignalCapture_TakeSnapshot(SignalCapture_Snapshot *snapshot);
void SignalCapture_GetDiagnostics(SignalCapture_Diagnostics *diagnostics);
void SignalCapture_ClearDiagnostics(void);
uint16_t SignalCapture_CopyReferenceWaveform(uint16_t *destination,
                                             uint16_t capacity,
                                             uint32_t *sequence);

#endif
