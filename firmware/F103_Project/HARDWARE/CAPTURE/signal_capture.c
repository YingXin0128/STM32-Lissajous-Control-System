/*
 * 接收端：UART 字节流 → 48 字节帧检查 → 测量快照队列。
 * 发送端是 F407_Project/MY/protocol.c；main 消费快照后才调用控制算法。
 */
#include "signal_capture.h"
#include "usart.h"

#define LINK_SYNC0                    0xA5U
#define LINK_SYNC1                    0x5AU
#define LINK_VERSION                  0x04U
#define LINK_TYPE_MEASUREMENT         0x10U
#define LINK_PAYLOAD_SIZE             41U
#define LINK_FRAME_SIZE               48U
#define LINK_STALE_MS                 100U

#define LINK_FLAG_REFERENCE_VALID     0x01U
#define LINK_FLAG_PHASE_VALID         0x02U
#define LINK_FLAG_FEEDBACK_VALID      0x04U
#define LINK_FLAG_DMA_OVERRUN         0x08U
#define LINK_FLAG_PHASE_HARMONIC_2    0x10U
#define LINK_FLAG_REFERENCE_CLIPPED   0x20U
#define LINK_FLAG_FEEDBACK_CLIPPED    0x40U
#define LINK_FLAG_CLIP_RECOVERY       0x80U

static SignalCapture_Snapshot current_snapshot;
static SignalCapture_Snapshot snapshot_queue[SIGNAL_CAPTURE_QUEUE_SIZE];
static uint8_t snapshot_queue_head;
static uint8_t snapshot_queue_tail;
static SignalCapture_Diagnostics link_diagnostics;
static uint8_t parser_frame[LINK_FRAME_SIZE];
static uint8_t parser_index;
static uint8_t last_sequence_valid;
static uint16_t last_sequence;

static void Link_IncrementSaturated(uint32_t *counter, uint32_t amount)
{
    if ((0xFFFFFFFFUL - *counter) < amount)
        *counter = 0xFFFFFFFFUL;
    else
        *counter += amount;
}

static void Link_QueueSnapshot(const SignalCapture_Snapshot *snapshot)
{
    uint8_t next_head;

    next_head = (uint8_t)((snapshot_queue_head + 1U) %
                          SIGNAL_CAPTURE_QUEUE_SIZE);
    if (next_head == snapshot_queue_tail)
    {
        snapshot_queue_tail =
            (uint8_t)((snapshot_queue_tail + 1U) %
                      SIGNAL_CAPTURE_QUEUE_SIZE);
        Link_IncrementSaturated(&link_diagnostics.queue_overflows, 1UL);
    }
    snapshot_queue[snapshot_queue_head] = *snapshot;
    snapshot_queue_head = next_head;
}

static uint16_t Link_GetU16(const uint8_t *source)
{
    return (uint16_t)source[0] |
           ((uint16_t)source[1] << 8);
}

static uint32_t Link_GetU32(const uint8_t *source)
{
    return (uint32_t)source[0] |
           ((uint32_t)source[1] << 8) |
           ((uint32_t)source[2] << 16) |
           ((uint32_t)source[3] << 24);
}

static uint16_t Link_Crc16(const uint8_t *data, uint16_t length)
{
    uint16_t crc;
    uint16_t index;
    uint8_t bit;

    crc = 0xFFFFU;
    for (index = 0U; index < length; index++)
    {
        crc ^= (uint16_t)data[index] << 8;
        for (bit = 0U; bit < 8U; bit++)
        {
            if ((crc & 0x8000U) != 0U)
                crc = (uint16_t)((crc << 1) ^ 0x1021U);
            else
                crc <<= 1;
        }
    }
    return crc;
}

/* 通过格式和 CRC 的帧在此解包，更新有效位、测量值及链路统计。 */
static void Link_AcceptFrame(const uint8_t *frame)
{
    uint16_t sequence;
    uint16_t sequence_delta;
    uint16_t reference_vpp;
    uint16_t feedback_vpp;
    uint8_t flags;

    sequence = Link_GetU16(&frame[5]);
    reference_vpp = Link_GetU16(&frame[27]);
    feedback_vpp = Link_GetU16(&frame[29]);
    flags = frame[31];

    if (last_sequence_valid != 0U)
    {
        sequence_delta = (uint16_t)(sequence - last_sequence);
        if ((sequence_delta > 1U) && (sequence_delta < 0x8000U))
        {
            Link_IncrementSaturated(
                &link_diagnostics.sequence_gap_events, 1UL);
            Link_IncrementSaturated(
                &link_diagnostics.missing_frames,
                (uint32_t)(sequence_delta - 1U));
        }
    }
    last_sequence_valid = 1U;
    last_sequence = sequence;

    current_snapshot.sample_timestamp_us =
        Link_GetU32(&frame[7]);
    current_snapshot.reference_raw_frequency_millihz =
        Link_GetU32(&frame[11]);
    current_snapshot.reference_frequency_millihz =
        Link_GetU32(&frame[15]);
    current_snapshot.relative_phase_millidegrees =
        (int32_t)Link_GetU32(&frame[19]);
    current_snapshot.feedback_frequency_millihz =
        Link_GetU32(&frame[23]);
    current_snapshot.reference_sequence = sequence;
    current_snapshot.feedback_sequence = sequence;
    current_snapshot.reference_age_ms = 0U;
    current_snapshot.feedback_age_ms = 0U;
    current_snapshot.reference_adc_vpp_counts = reference_vpp;
    current_snapshot.reference_adc_min = Link_GetU16(&frame[34]);
    current_snapshot.reference_adc_max = Link_GetU16(&frame[36]);
    current_snapshot.reference_adc_mean =
        (uint16_t)(((uint32_t)current_snapshot.reference_adc_min +
                    (uint32_t)current_snapshot.reference_adc_max) / 2UL);
    current_snapshot.feedback_adc_min = Link_GetU16(&frame[38]);
    current_snapshot.feedback_adc_max = Link_GetU16(&frame[40]);
    current_snapshot.feedback_adc_vpp_counts = feedback_vpp;
    current_snapshot.f407_analysis_time_us = Link_GetU16(&frame[42]);
    current_snapshot.f407_firmware_tag = Link_GetU16(&frame[44]);

    current_snapshot.reference_valid =
        ((flags & LINK_FLAG_REFERENCE_VALID) != 0U) ? 1U : 0U;
    current_snapshot.relative_phase_valid =
        ((flags & LINK_FLAG_PHASE_VALID) != 0U) ? 1U : 0U;
    current_snapshot.feedback_valid =
        ((flags & LINK_FLAG_FEEDBACK_VALID) != 0U) ? 1U : 0U;
    current_snapshot.phase_harmonic =
        ((flags & LINK_FLAG_PHASE_HARMONIC_2) != 0U) ? 2U : 1U;
    current_snapshot.reference_clipped =
        ((flags & LINK_FLAG_REFERENCE_CLIPPED) != 0U) ? 1U : 0U;
    current_snapshot.feedback_clipped =
        ((flags & LINK_FLAG_FEEDBACK_CLIPPED) != 0U) ? 1U : 0U;
    current_snapshot.clip_recovery =
        ((flags & LINK_FLAG_CLIP_RECOVERY) != 0U) ? 1U : 0U;
    current_snapshot.phase_reject_flags = 0U;
    current_snapshot.dma_overrun =
        ((flags & LINK_FLAG_DMA_OVERRUN) != 0U) ? 1U : 0U;

    if (current_snapshot.dma_overrun != 0U)
    {
        Link_IncrementSaturated(
            &link_diagnostics.f407_dma_overrun_frames, 1UL);
        current_snapshot.reference_valid = 0U;
        current_snapshot.relative_phase_valid = 0U;
        current_snapshot.feedback_valid = 0U;
    }
    if (current_snapshot.reference_clipped != 0U)
    {
        Link_IncrementSaturated(
            &link_diagnostics.reference_clipped_frames, 1UL);
        current_snapshot.relative_phase_valid = 0U;
    }
    if (current_snapshot.feedback_clipped != 0U)
    {
        Link_IncrementSaturated(
            &link_diagnostics.feedback_clipped_frames, 1UL);
        current_snapshot.relative_phase_valid = 0U;
    }
    if (current_snapshot.clip_recovery != 0U)
    {
        Link_IncrementSaturated(
            &link_diagnostics.clip_recovery_frames, 1UL);
        current_snapshot.relative_phase_valid = 0U;
    }

    if (current_snapshot.reference_valid == 0U)
    {
        current_snapshot.phase_reject_flags |=
            SIGNAL_CAPTURE_REJECT_REFERENCE_INVALID;
        Link_IncrementSaturated(
            &link_diagnostics.reference_invalid_frames, 1UL);
    }
    if (current_snapshot.feedback_valid == 0U)
    {
        current_snapshot.phase_reject_flags |=
            SIGNAL_CAPTURE_REJECT_FEEDBACK_INVALID;
        Link_IncrementSaturated(
            &link_diagnostics.feedback_invalid_frames, 1UL);
    }
    if (current_snapshot.reference_clipped != 0U)
        current_snapshot.phase_reject_flags |=
            SIGNAL_CAPTURE_REJECT_REFERENCE_CLIP;
    if (current_snapshot.feedback_clipped != 0U)
        current_snapshot.phase_reject_flags |=
            SIGNAL_CAPTURE_REJECT_FEEDBACK_CLIP;
    if (current_snapshot.clip_recovery != 0U)
        current_snapshot.phase_reject_flags |=
            SIGNAL_CAPTURE_REJECT_CLIP_RECOVERY;
    if (current_snapshot.dma_overrun != 0U)
        current_snapshot.phase_reject_flags |=
            SIGNAL_CAPTURE_REJECT_DMA_OVERRUN;
    if ((current_snapshot.relative_phase_valid == 0U) &&
        (current_snapshot.phase_reject_flags == 0U))
        current_snapshot.phase_reject_flags |=
            SIGNAL_CAPTURE_REJECT_PHASE_ESTIMATOR;
    if (current_snapshot.relative_phase_valid == 0U)
    {
        Link_IncrementSaturated(
            &link_diagnostics.phase_invalid_frames, 1UL);
        Link_IncrementSaturated(
            &link_diagnostics.rejected_control_frames, 1UL);
    }
    link_diagnostics.f407_dropped_frames =
        (uint32_t)Link_GetU16(&frame[32]);

    if (current_snapshot.reference_raw_frequency_millihz != 0UL)
        current_snapshot.reference_period_ticks =
            (uint32_t)(800000000ULL /
                       current_snapshot.reference_raw_frequency_millihz);
    else
        current_snapshot.reference_period_ticks = 0UL;
    if ((current_snapshot.feedback_frequency_millihz != 0UL) &&
        (feedback_vpp != 0U))
        current_snapshot.feedback_period_ticks =
            (uint32_t)(800000000ULL /
                       current_snapshot.feedback_frequency_millihz);
    else
        current_snapshot.feedback_period_ticks = 0UL;

    Link_IncrementSaturated(&link_diagnostics.accepted_frames, 1UL);
    Link_QueueSnapshot(&current_snapshot);
}

/* 逐字节解析：查找 A5 5A → 收满固定帧 → 校验头字段/CRC → 接受或计错。 */
static void Link_ProcessByte(uint8_t value)
{
    uint16_t expected_crc;
    uint16_t received_crc;

    if (parser_index == 0U)
    {
        if (value == LINK_SYNC0)
            parser_frame[parser_index++] = value;
        return;
    }
    if (parser_index == 1U)
    {
        if (value == LINK_SYNC1)
            parser_frame[parser_index++] = value;
        else if (value == LINK_SYNC0)
            parser_frame[0] = value;
        else
        {
            parser_index = 0U;
            Link_IncrementSaturated(&link_diagnostics.sync_losses, 1UL);
        }
        return;
    }

    parser_frame[parser_index++] = value;
    if (parser_index < LINK_FRAME_SIZE)
        return;
    parser_index = 0U;

    if ((parser_frame[2] != LINK_VERSION) ||
        (parser_frame[3] != LINK_TYPE_MEASUREMENT) ||
        (parser_frame[4] != LINK_PAYLOAD_SIZE))
    {
        Link_IncrementSaturated(&link_diagnostics.format_errors, 1UL);
        return;
    }
    expected_crc = Link_Crc16(&parser_frame[2], 44U);
    received_crc = Link_GetU16(&parser_frame[46]);
    if (expected_crc == received_crc)
        Link_AcceptFrame(parser_frame);
    else
        Link_IncrementSaturated(&link_diagnostics.crc_errors, 1UL);
}

void SignalCapture_Init(void)
{
    uint8_t *destination;
    uint16_t index;

    destination = (uint8_t *)&current_snapshot;
    for (index = 0U; index < sizeof(current_snapshot); index++)
        destination[index] = 0U;
    parser_index = 0U;
    snapshot_queue_head = 0U;
    snapshot_queue_tail = 0U;
    last_sequence_valid = 0U;
    last_sequence = 0U;
    destination = (uint8_t *)&link_diagnostics;
    for (index = 0U; index < sizeof(link_diagnostics); index++)
        destination[index] = 0U;
    current_snapshot.reference_age_ms = 0xFFFFU;
    current_snapshot.feedback_age_ms = 0xFFFFU;
}

void SignalCapture_Service(void)
{
    uint8_t value;

    while (USART1_ReadByte(&value) != 0U)
        Link_ProcessByte(value);
}

void SignalCapture_Tick1ms(void)
{
    SignalCapture_TickMs(1UL);
}

void SignalCapture_TickMs(uint32_t elapsed_ms)
{
    uint32_t next_age;

    next_age = (uint32_t)current_snapshot.reference_age_ms + elapsed_ms;
    current_snapshot.reference_age_ms =
        (next_age > 0xFFFFUL) ? 0xFFFFU : (uint16_t)next_age;
    next_age = (uint32_t)current_snapshot.feedback_age_ms + elapsed_ms;
    current_snapshot.feedback_age_ms =
        (next_age > 0xFFFFUL) ? 0xFFFFU : (uint16_t)next_age;
}

/* 取最新快照时施加 100 ms 过期判断，避免将断链前的旧数据一直视为有效。 */
void SignalCapture_GetSnapshot(SignalCapture_Snapshot *snapshot)
{
    if (snapshot == 0)
        return;
    *snapshot = current_snapshot;
    if (snapshot->reference_age_ms > LINK_STALE_MS)
    {
        snapshot->reference_valid = 0U;
        snapshot->relative_phase_valid = 0U;
    }
    if (snapshot->feedback_age_ms > LINK_STALE_MS)
        snapshot->feedback_valid = 0U;
}

uint8_t SignalCapture_TakeSnapshot(SignalCapture_Snapshot *snapshot)
{
    if ((snapshot == 0) ||
        (snapshot_queue_tail == snapshot_queue_head))
        return 0U;

    *snapshot = snapshot_queue[snapshot_queue_tail];
    snapshot_queue_tail =
        (uint8_t)((snapshot_queue_tail + 1U) %
                  SIGNAL_CAPTURE_QUEUE_SIZE);
    return 1U;
}

void SignalCapture_GetDiagnostics(SignalCapture_Diagnostics *diagnostics)
{
    if (diagnostics == 0)
        return;
    *diagnostics = link_diagnostics;
    diagnostics->uart_dma_overflow_bytes = USART1_GetOverflowCount();
}

void SignalCapture_ClearDiagnostics(void)
{
    uint8_t *destination;
    uint16_t index;

    destination = (uint8_t *)&link_diagnostics;
    for (index = 0U; index < sizeof(link_diagnostics); index++)
        destination[index] = 0U;
    USART1_ClearOverflowCount();
}

uint16_t SignalCapture_CopyReferenceWaveform(uint16_t *destination,
                                             uint16_t capacity,
                                             uint32_t *sequence)
{
    (void)destination;
    (void)capacity;
    if (sequence != 0)
        *sequence = 0UL;
    return 0U;
}
