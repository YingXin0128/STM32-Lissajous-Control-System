#ifndef __MEASUREMENT_H
#define __MEASUREMENT_H

#include "stm32f4xx.h"

typedef struct
{
    uint32_t reference_raw_frequency_millihz;
    uint32_t reference_frequency_millihz;
    uint32_t feedback_frequency_millihz;
    int32_t relative_phase_millidegrees;
    uint16_t reference_min;
    uint16_t reference_max;
    uint16_t reference_mean;
    uint16_t reference_vpp;
    uint16_t feedback_min;
    uint16_t feedback_max;
    uint16_t feedback_vpp;
    uint16_t frame_sequence;
    uint16_t dropped_frames;
    uint16_t analysis_time_us;
    uint16_t firmware_tag;
    uint32_t sample_timestamp_us;
    uint8_t flags;
} Measurement_Result;

void Measurement_Init(void);
uint8_t Measurement_TakeReadyFrame(uint8_t *frame_index);
void Measurement_Analyze(uint8_t frame_index, Measurement_Result *result);
void Measurement_DmaIrqHandler(void);

#endif
