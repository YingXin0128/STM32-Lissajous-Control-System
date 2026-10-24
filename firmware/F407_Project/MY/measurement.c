/*
 * 采集与测量算法：本文件是 F407 的核心。
 * 阅读顺序：Measurement_Init → TakeReadyFrame → Analyze → AnalyzeChannel / CalculateRelativePhase。
 * 单位约定：频率 millihz 为毫赫兹，相位 millidegrees 为毫度，Vpp 为 ADC 计数。
 */
#include "measurement.h"
#include "app_config.h"
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct
{
    uint32_t frequency_millihz;
    uint32_t first_cross_q16;
    uint16_t minimum;
    uint16_t maximum;
    uint16_t mean;
    uint16_t vpp;
    uint8_t valid;
} Channel_Result;

static uint32_t adc_dma_buffer[APP_DMA_WORD_COUNT];
static uint32_t analysis_buffer[APP_FRAME_SAMPLES];
static volatile uint8_t frame_ready;
static volatile uint8_t overrun_latched;
static volatile uint16_t dropped_frames;
static volatile uint16_t dma_frame_sequence;
static volatile uint16_t pending_frame_sequence;
static uint16_t analysis_frame_sequence;
static volatile uint32_t dma_sample_end_timestamp_us;
static volatile uint32_t pending_sample_timestamp_us;
static uint32_t analysis_sample_timestamp_us;
static uint8_t clip_clean_frames;

static uint16_t Measurement_GetSample(const uint32_t *samples,
                                      uint16_t index,
                                      uint8_t channel)
{
    uint32_t word;

    word = samples[index];
    if (channel == 0U)
        return (uint16_t)(word & 0xFFFFU);
    return (uint16_t)(word >> 16);
}

/* 单通道估频：先统计均值/峰峰值，再用滞回抑制重复穿越。
 * 穿越位置用 Q16 插值，首尾穿越间的周期数用于计算频率；不计算 RMS。 */
static void Measurement_AnalyzeChannel(const uint32_t *samples,
                                       uint8_t channel,
                                       Channel_Result *result)
{
    uint32_t sum;
    uint32_t first_cross;
    uint32_t last_cross;
    uint32_t crossing;
    uint32_t numerator;
    uint32_t denominator;
    uint16_t index;
    uint16_t sample;
    uint16_t previous;
    uint16_t minimum;
    uint16_t maximum;
    uint16_t mean;
    uint16_t low_threshold;
    uint16_t high_threshold;
    uint16_t crossing_count;
    uint8_t armed;
    uint64_t frequency_numerator;

    minimum = 4095U;
    maximum = 0U;
    sum = 0UL;
    for (index = 0U; index < APP_FRAME_SAMPLES; index++)
    {
        sample = Measurement_GetSample(samples, index, channel);
        if (sample < minimum)
            minimum = sample;
        if (sample > maximum)
            maximum = sample;
        sum += sample;
    }

    mean = (uint16_t)((sum + (APP_FRAME_SAMPLES / 2U)) /
                      APP_FRAME_SAMPLES);
    result->minimum = minimum;
    result->maximum = maximum;
    result->mean = mean;
    result->vpp = (uint16_t)(maximum - minimum);
    result->frequency_millihz = 0UL;
    result->first_cross_q16 = 0UL;
    result->valid = 0U;
    if (result->vpp < APP_MIN_VPP_COUNTS)
        return;

    low_threshold = (uint16_t)(mean - (result->vpp / 8U));
    high_threshold = (uint16_t)(mean + (result->vpp / 8U));
    armed = 0U;
    crossing_count = 0U;
    first_cross = 0UL;
    last_cross = 0UL;
    previous = Measurement_GetSample(samples, 0U, channel);

    for (index = 1U; index < APP_FRAME_SAMPLES; index++)
    {
        sample = Measurement_GetSample(samples, index, channel);
        if (sample <= low_threshold)
            armed = 1U;
        if ((armed != 0U) &&
            (previous < mean) &&
            (sample >= mean) &&
            (sample > previous))
        {
            numerator = (uint32_t)(mean - previous) << 16;
            denominator = (uint32_t)(sample - previous);
            crossing = ((uint32_t)(index - 1U) << 16) +
                       (numerator / denominator);
            if (crossing_count == 0U)
                first_cross = crossing;
            last_cross = crossing;
            crossing_count++;
            armed = 0U;
        }
        else if (sample >= high_threshold)
        {
            armed = 0U;
        }
        previous = sample;
    }

    if ((crossing_count < 3U) || (last_cross <= first_cross))
        return;
    frequency_numerator =
        (uint64_t)APP_ADC_SAMPLE_RATE_HZ * 1000ULL *
        (uint64_t)(crossing_count - 1U) * 65536ULL;
    result->frequency_millihz =
        (uint32_t)((frequency_numerator +
                    ((uint64_t)(last_cross - first_cross) / 2ULL)) /
                   (uint64_t)(last_cross - first_cross));
    result->first_cross_q16 = first_cross;
    result->valid = 1U;
}

static uint32_t Measurement_AbsDiff(uint32_t first, uint32_t second)
{
    return (first >= second) ? (first - second) : (second - first);
}

static int32_t Measurement_WrapPhase(int32_t phase_millidegrees)
{
    while (phase_millidegrees > 180000L)
        phase_millidegrees -= 360000L;
    while (phase_millidegrees <= -180000L)
        phase_millidegrees += 360000L;
    return phase_millidegrees;
}

/*
 * Single-bin IQ correlation at a known frequency.  The oscillator is
 * advanced recursively, so sinf/cosf are evaluated only once per channel
 * and frame.  The controller uses both absolute relative phase and its
 * long-window slope, so both channels use the same reference-derived NCO.
 * For a sine input this detector's absolute result is physical phase -90
 * degrees; Measurement_CalculateRelativePhase compensates the residual
 * harmonic-dependent offset after combining the two channel phases.
 */
/* 单频 IQ 相关：输入去均值后与递推正交基准相关，atan2f 输出毫度相位。 */
static uint8_t Measurement_CalculateIqPhase(
    const uint32_t *samples,
    uint8_t channel,
    uint16_t mean,
    uint32_t frequency_millihz,
    int32_t *phase_millidegrees)
{
    float step;
    float step_cos;
    float step_sin;
    float oscillator_cos;
    float oscillator_sin;
    float next_cos;
    float in_phase;
    float quadrature;
    float value;
    float phase;
    uint16_t index;

    if ((frequency_millihz == 0UL) ||
        (phase_millidegrees == 0))
        return 0U;

    step = (2.0f * (float)M_PI *
            (float)frequency_millihz) /
           ((float)APP_ADC_SAMPLE_RATE_HZ * 1000.0f);
    step_cos = cosf(step);
    step_sin = sinf(step);
    oscillator_cos = 1.0f;
    oscillator_sin = 0.0f;
    in_phase = 0.0f;
    quadrature = 0.0f;

    for (index = 0U; index < APP_FRAME_SAMPLES; index++)
    {
        value = (float)((int32_t)Measurement_GetSample(samples,
                                                        index,
                                                        channel) -
                        (int32_t)mean);
        in_phase += value * oscillator_cos;
        quadrature += value * oscillator_sin;
        next_cos = oscillator_cos * step_cos -
                   oscillator_sin * step_sin;
        oscillator_sin = oscillator_sin * step_cos +
                         oscillator_cos * step_sin;
        oscillator_cos = next_cos;
    }

    if ((in_phase * in_phase + quadrature * quadrature) < 1000000.0f)
        return 0U;

    phase = atan2f(-quadrature, in_phase) *
            (180000.0f / (float)M_PI);
    *phase_millidegrees =
        Measurement_WrapPhase((int32_t)((phase >= 0.0f) ?
                                        (phase + 0.5f) :
                                        (phase - 0.5f)));
    return 1U;
}

/* 判断反馈接近参考的 1 倍还是 2 倍频，再组合两路 IQ 相位。
 * 二倍频需扣除检测器固有的 90 度偏置；输出折返到一个相位周期内。 */
static uint8_t Measurement_CalculateRelativePhase(
    const uint32_t *samples,
    const Channel_Result *reference,
    const Channel_Result *feedback,
    uint32_t reference_frequency_millihz,
    int32_t *relative_phase_millidegrees,
    uint8_t *harmonic)
{
    uint32_t target_frequency;
    uint32_t difference_1f;
    uint32_t difference_2f;
    int32_t reference_phase;
    int32_t feedback_phase;

    if ((samples == 0) ||
        (reference == 0) ||
        (feedback == 0) ||
        (relative_phase_millidegrees == 0) ||
        (harmonic == 0) ||
        (reference->valid == 0U) ||
        (feedback->valid == 0U))
        return 0U;

    difference_1f = Measurement_AbsDiff(
        feedback->frequency_millihz,
        reference_frequency_millihz);
    difference_2f = 0xFFFFFFFFUL;
    if (reference_frequency_millihz <=
        (APP_FEEDBACK_MAX_MHZ / 2UL))
    {
        difference_2f = Measurement_AbsDiff(
            feedback->frequency_millihz,
            reference_frequency_millihz * 2UL);
    }

    *harmonic = (difference_2f < difference_1f) ? 2U : 1U;
    target_frequency =
        reference_frequency_millihz * (uint32_t)(*harmonic);
    if (Measurement_AbsDiff(feedback->frequency_millihz,
                            target_frequency) >
        APP_PHASE_CAPTURE_MHZ)
        return 0U;

    if (Measurement_CalculateIqPhase(samples,
                                     0U,
                                     reference->mean,
                                     reference_frequency_millihz,
                                     &reference_phase) == 0U)
        return 0U;
    if (Measurement_CalculateIqPhase(samples,
                                     1U,
                                     feedback->mean,
                                     target_frequency,
                                     &feedback_phase) == 0U)
        return 0U;

    /*
     * The IQ detector reports sine phase as (physical phase - 90 deg).
     * That common offset cancels for 1F, but leaves +(h-1)*90 deg in
     * feedback_phase - h*reference_phase.  Remove the deterministic
     * harmonic-dependent bias so 2F phase zero matches the oscilloscope.
     */
    *relative_phase_millidegrees =
        Measurement_WrapPhase(
            feedback_phase -
            ((int32_t)(*harmonic) * reference_phase) -
            ((int32_t)((*harmonic) - 1U) * 90000L));
    return 1U;
}

/* 配置 PC0/ADC1 与 PC1/ADC2 同步转换，TIM2 触发，DMA 搬运打包采样。 */
void Measurement_Init(void)
{
    GPIO_InitTypeDef gpio;
    DMA_InitTypeDef dma;
    ADC_CommonInitTypeDef common;
    ADC_InitTypeDef adc;
    TIM_TimeBaseInitTypeDef timer;
    NVIC_InitTypeDef nvic;

    frame_ready = 0U;
    overrun_latched = 0U;
    dropped_frames = 0U;
    dma_frame_sequence = 0U;
    pending_frame_sequence = 0U;
    analysis_frame_sequence = 0U;
    dma_sample_end_timestamp_us = 0UL;
    pending_sample_timestamp_us = 0UL;
    analysis_sample_timestamp_us = 0UL;
    clip_clean_frames = APP_ADC_CLIP_RECOVERY_FRAMES;
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0UL;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC |
                           RCC_AHB1Periph_DMA2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 |
                           RCC_APB2Periph_ADC2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    gpio.GPIO_Mode = GPIO_Mode_AN;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &gpio);

    DMA_DeInit(DMA2_Stream0);
    while (DMA_GetCmdStatus(DMA2_Stream0) != DISABLE) {}
    DMA_StructInit(&dma);
    dma.DMA_Channel = DMA_Channel_0;
    dma.DMA_PeripheralBaseAddr = (uint32_t)&ADC->CDR;
    dma.DMA_Memory0BaseAddr = (uint32_t)adc_dma_buffer;
    dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
    dma.DMA_BufferSize = APP_DMA_WORD_COUNT;
    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    dma.DMA_Mode = DMA_Mode_Circular;
    dma.DMA_Priority = DMA_Priority_VeryHigh;
    dma.DMA_FIFOMode = DMA_FIFOMode_Enable;
    dma.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    dma.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream0, &dma);
    DMA_ITConfig(DMA2_Stream0, DMA_IT_TC | DMA_IT_TE, ENABLE);

    nvic.NVIC_IRQChannel = DMA2_Stream0_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1U;
    nvic.NVIC_IRQChannelSubPriority = 0U;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    ADC_DeInit();
    common.ADC_Mode = ADC_DualMode_RegSimult;
    common.ADC_Prescaler = ADC_Prescaler_Div4;
    /*
     * DMA mode 2 packs one simultaneous conversion pair in ADC_CDR:
     * ADC1 (PC0) in bits 15:0 and ADC2 (PC1) in bits 31:16.
     * The DMA is configured for one 32-bit word per timer trigger.
     */
    common.ADC_DMAAccessMode = ADC_DMAAccessMode_2;
    common.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&common);
    ADC_StructInit(&adc);
    adc.ADC_Resolution = ADC_Resolution_12b;
    adc.ADC_ScanConvMode = DISABLE;
    adc.ADC_ContinuousConvMode = DISABLE;
    adc.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
    adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;
    adc.ADC_DataAlign = ADC_DataAlign_Right;
    adc.ADC_NbrOfConversion = 1U;
    ADC_Init(ADC1, &adc);
    adc.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_Init(ADC2, &adc);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1U,
                             ADC_SampleTime_3Cycles);
    ADC_RegularChannelConfig(ADC2, ADC_Channel_11, 1U,
                             ADC_SampleTime_3Cycles);
    ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC2, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    TIM_TimeBaseStructInit(&timer);
    timer.TIM_Prescaler = 0U;
    timer.TIM_CounterMode = TIM_CounterMode_Up;
    timer.TIM_Period = 104U;
    timer.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &timer);
    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
    DMA_Cmd(DMA2_Stream0, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

/* 取就绪数据：复制 DMA 缓冲后半段供主循环分析；不是每个半缓冲都分析。 */
uint8_t Measurement_TakeReadyFrame(uint8_t *frame_index)
{
    if (frame_index == 0)
        return 0U;
    __disable_irq();
    if (frame_ready == 0U)
    {
        __enable_irq();
        return 0U;
    }
    frame_ready = 0U;
    analysis_frame_sequence = pending_frame_sequence;
    analysis_sample_timestamp_us = pending_sample_timestamp_us;
    __enable_irq();

    /*
     * The just-completed second half remains untouched while DMA fills the
     * first half.  Copy it immediately; analysis can then run independently
     * without ADC data being overwritten underneath it.
     */
    memcpy(analysis_buffer,
           &adc_dma_buffer[APP_FRAME_SAMPLES],
           sizeof(analysis_buffer));
    *frame_index = 0U;
    return 1U;
}

/* 测量总入口：两路估频 → 参考网格归整 → 削顶/恢复判断 → 相位及诊断结果。 */
void Measurement_Analyze(uint8_t frame_index, Measurement_Result *result)
{
    const uint32_t *samples;
    Channel_Result reference;
    Channel_Result feedback;
    uint32_t quantized;
    uint32_t start_cycles;
    uint32_t elapsed_cycles;
    uint32_t cycles_per_us;
    uint8_t phase_harmonic;
    uint8_t reference_clipped;
    uint8_t feedback_clipped;

    (void)frame_index;
    start_cycles = DWT->CYCCNT;
    samples = analysis_buffer;
    Measurement_AnalyzeChannel(samples, 0U, &reference);
    Measurement_AnalyzeChannel(samples, 1U, &feedback);
    result->reference_raw_frequency_millihz =
        reference.frequency_millihz;
    result->reference_frequency_millihz = 0UL;
    result->feedback_frequency_millihz =
        feedback.frequency_millihz;
    result->relative_phase_millidegrees = 0L;
    result->reference_min = reference.minimum;
    result->reference_max = reference.maximum;
    result->reference_mean = reference.mean;
    result->reference_vpp = reference.vpp;
    result->feedback_min = feedback.minimum;
    result->feedback_max = feedback.maximum;
    result->feedback_vpp = feedback.vpp;
    result->frame_sequence = analysis_frame_sequence;
    result->sample_timestamp_us = analysis_sample_timestamp_us;
    result->flags = 0U;
    result->dropped_frames = dropped_frames;
    result->firmware_tag = APP_FIRMWARE_TAG;

    reference_clipped =
        ((reference.minimum <= APP_ADC_CLIP_MARGIN_COUNTS) ||
         (reference.maximum >=
          (4095U - APP_ADC_CLIP_MARGIN_COUNTS))) ? 1U : 0U;
    feedback_clipped =
        ((feedback.minimum <= APP_ADC_CLIP_MARGIN_COUNTS) ||
         (feedback.maximum >=
          (4095U - APP_ADC_CLIP_MARGIN_COUNTS))) ? 1U : 0U;
    if (reference_clipped != 0U)
        result->flags |= APP_FLAG_REFERENCE_CLIPPED;
    if (feedback_clipped != 0U)
        result->flags |= APP_FLAG_FEEDBACK_CLIPPED;
    if ((reference_clipped != 0U) || (feedback_clipped != 0U))
    {
        clip_clean_frames = 0U;
    }
    else if (clip_clean_frames < APP_ADC_CLIP_RECOVERY_FRAMES)
    {
        clip_clean_frames++;
    }
    if (clip_clean_frames < APP_ADC_CLIP_RECOVERY_FRAMES)
        result->flags |= APP_FLAG_CLIP_RECOVERY;

    if ((reference.valid != 0U) &&
        (reference.frequency_millihz >=
         APP_REFERENCE_CAPTURE_MIN_MHZ) &&
        (reference.frequency_millihz <=
         APP_REFERENCE_CAPTURE_MAX_MHZ))
    {
        quantized =
            ((reference.frequency_millihz +
              (APP_REFERENCE_STEP_MHZ / 2UL)) /
             APP_REFERENCE_STEP_MHZ) * APP_REFERENCE_STEP_MHZ;
        if (quantized < APP_REFERENCE_MIN_MHZ)
            quantized = APP_REFERENCE_MIN_MHZ;
        if (quantized > APP_REFERENCE_MAX_MHZ)
            quantized = APP_REFERENCE_MAX_MHZ;
        result->reference_frequency_millihz = quantized;
        result->flags |= APP_FLAG_REFERENCE_VALID;
    }
    if ((feedback.valid != 0U) &&
        (feedback.frequency_millihz >= APP_FEEDBACK_MIN_MHZ) &&
        (feedback.frequency_millihz <= APP_FEEDBACK_MAX_MHZ))
        result->flags |= APP_FLAG_FEEDBACK_VALID;
    if (((result->flags & (APP_FLAG_REFERENCE_VALID |
                           APP_FLAG_FEEDBACK_VALID)) ==
         (APP_FLAG_REFERENCE_VALID |
          APP_FLAG_FEEDBACK_VALID)) &&
        (reference_clipped == 0U) &&
        (feedback_clipped == 0U) &&
        ((result->flags & APP_FLAG_CLIP_RECOVERY) == 0U) &&
        (Measurement_CalculateRelativePhase(
             samples,
             &reference,
             &feedback,
             result->reference_frequency_millihz,
             &result->relative_phase_millidegrees,
             &phase_harmonic) != 0U))
    {
        result->flags |= APP_FLAG_PHASE_VALID;
        if (phase_harmonic == 2U)
            result->flags |= APP_FLAG_PHASE_HARMONIC_2;
    }
    if (overrun_latched != 0U)
    {
        result->flags |= APP_FLAG_DMA_OVERRUN;
        result->flags &= (uint8_t)~(APP_FLAG_REFERENCE_VALID |
                                    APP_FLAG_PHASE_VALID |
                                    APP_FLAG_FEEDBACK_VALID);
        overrun_latched = 0U;
    }

    elapsed_cycles = DWT->CYCCNT - start_cycles;
    cycles_per_us = SystemCoreClock / 1000000UL;
    if (cycles_per_us == 0UL)
        cycles_per_us = 1UL;
    elapsed_cycles =
        (elapsed_cycles + (cycles_per_us / 2UL)) / cycles_per_us;
    result->analysis_time_us =
        (elapsed_cycles > 65535UL) ? 65535U :
                                     (uint16_t)elapsed_cycles;
}

static void Measurement_MarkReadyFrame(void)
{
    dma_sample_end_timestamp_us +=
        (uint32_t)(((uint64_t)1000000ULL *
                    (uint64_t)APP_DMA_WORD_COUNT) /
                   (uint64_t)APP_ADC_SAMPLE_RATE_HZ);
    pending_frame_sequence = dma_frame_sequence++;
    pending_sample_timestamp_us =
        dma_sample_end_timestamp_us - APP_PUBLISH_CENTER_OFFSET_US;

    if (frame_ready != 0U)
    {
        overrun_latched = 1U;
        if (dropped_frames < 65535U)
            dropped_frames++;
    }
    frame_ready = 1U;
}

void Measurement_DmaIrqHandler(void)
{
    if (DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0) != RESET)
    {
        DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
        Measurement_MarkReadyFrame();
    }
    if (DMA_GetITStatus(DMA2_Stream0, DMA_IT_TEIF0) != RESET)
    {
        DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TEIF0);
        overrun_latched = 1U;
        if (dropped_frames < 65535U)
            dropped_frames++;
    }
}
