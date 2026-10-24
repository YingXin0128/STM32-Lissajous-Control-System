"""Host checks for the v2.2.6 F407 estimator and protocol v4."""

import math
import random
import struct


FS = 800_000
N = 8_000
FRAME_SECONDS = N / FS
PUBLISH_SECONDS = 2 * FRAME_SECONDS
SYNC = b"\xA5\x5A"


def crc16(data):
    crc = 0xFFFF
    for value in data:
        crc ^= value << 8
        for _ in range(8):
            crc = (
                ((crc << 1) ^ 0x1021) & 0xFFFF
                if crc & 0x8000
                else (crc << 1) & 0xFFFF
            )
    return crc


def wrap_mdeg(value):
    while value > 180_000:
        value -= 360_000
    while value <= -180_000:
        value += 360_000
    return value


def estimate_frequency(samples):
    minimum = min(samples)
    maximum = max(samples)
    mean = (sum(samples) + N // 2) // N
    vpp = maximum - minimum
    assert vpp >= 80
    low = mean - vpp // 8
    high = mean + vpp // 8
    armed = False
    crossings = []
    previous = samples[0]
    for index, sample in enumerate(samples[1:], 1):
        if sample <= low:
            armed = True
        if armed and previous < mean <= sample and sample > previous:
            fraction_q16 = ((mean - previous) << 16) // (sample - previous)
            crossings.append(((index - 1) << 16) + fraction_q16)
            armed = False
        elif sample >= high:
            armed = False
        previous = sample
    assert len(crossings) >= 3
    frequency_millihz = round(
        FS * 1000 * (len(crossings) - 1) * 65536
        / (crossings[-1] - crossings[0])
    )
    used_millihz = round(frequency_millihz / 100_000) * 100_000
    return frequency_millihz, used_millihz, mean, vpp


def iq_phase_mdeg(samples, mean, frequency_hz):
    step = 2 * math.pi * frequency_hz / FS
    step_cos = math.cos(step)
    step_sin = math.sin(step)
    oscillator_cos = 1.0
    oscillator_sin = 0.0
    in_phase = 0.0
    quadrature = 0.0
    for sample in samples:
        value = sample - mean
        in_phase += value * oscillator_cos
        quadrature += value * oscillator_sin
        next_cos = oscillator_cos * step_cos - oscillator_sin * step_sin
        oscillator_sin = (
            oscillator_sin * step_cos + oscillator_cos * step_sin
        )
        oscillator_cos = next_cos
    return wrap_mdeg(round(math.degrees(math.atan2(-quadrature, in_phase)) * 1000))


def sine_frame(frequency_hz, phase_degrees, frame_index, noise=1.5):
    # v2.2.2 publishes the completed second 10 ms window every 20 ms.
    start = frame_index * PUBLISH_SECONDS + FRAME_SECONDS
    values = []
    for index in range(N):
        time = start + index / FS
        value = 2048 + 1200 * math.sin(
            2 * math.pi * frequency_hz * time
            + math.radians(phase_degrees)
        )
        value += random.uniform(-noise, noise)
        values.append(max(0, min(4095, round(value))))
    return values


def relative_phase(reference, feedback, reference_hz, harmonic):
    _, _, reference_mean, _ = estimate_frequency(reference)
    _, _, feedback_mean, _ = estimate_frequency(feedback)
    reference_phase = iq_phase_mdeg(reference, reference_mean, reference_hz)
    feedback_phase = iq_phase_mdeg(
        feedback,
        feedback_mean,
        reference_hz * harmonic,
    )
    return wrap_mdeg(
        feedback_phase
        - harmonic * reference_phase
        - (harmonic - 1) * 90_000
    )


def make_frame(seq, timestamp_us, raw, used, relative_phase_value, feedback,
               ref_vpp, fb_vpp, flags, dropped, ref_min, ref_max, fb_min,
               fb_max, analysis_us, firmware_tag):
    frame = bytearray(48)
    frame[:5] = SYNC + bytes((4, 0x10, 41))
    struct.pack_into(
        "<HIIIIIHHBHHHHHHH",
        frame,
        5,
        seq,
        timestamp_us,
        raw,
        used,
        relative_phase_value & 0xFFFFFFFF,
        feedback,
        ref_vpp,
        fb_vpp,
        flags,
        dropped,
        ref_min,
        ref_max,
        fb_min,
        fb_max,
        analysis_us,
        firmware_tag,
    )
    struct.pack_into("<H", frame, 46, crc16(frame[2:46]))
    return bytes(frame)


def check_phase_slope(reference_hz, feedback_hz, harmonic):
    phases = []
    for frame_index in range(6):
        reference = sine_frame(reference_hz, 23, frame_index)
        feedback = sine_frame(feedback_hz, 71, frame_index)
        phases.append(
            relative_phase(reference, feedback, reference_hz, harmonic)
        )
    unwrapped = 0
    for previous, current in zip(phases, phases[1:]):
        unwrapped += wrap_mdeg(current - previous)
    elapsed_us = (len(phases) - 1) * 20_000
    slip_error_millihz = round(
        -unwrapped * 1_000_000 / (360 * elapsed_us)
    )
    expected = round((reference_hz * harmonic - feedback_hz) * 1000)
    assert abs(slip_error_millihz - expected) < 25
    return phases, slip_error_millihz


def main():
    random.seed(407)
    # Regression for the v2.2.1 32-bit overflow: 8,000,000,000 / 800,000
    # incorrectly became 4,631 us.  The DMA transfer is actually 20,000 us.
    dma_period_us = (1_000_000 * (2 * N)) // FS
    assert dma_period_us == 20_000
    assert [dma_period_us * (index + 1) - 5_000 for index in range(3)] == [
        15_000,
        35_000,
        55_000,
    ]

    # Capture margins accept edge jitter but clamp commands to legal limits.
    def map_reference(raw_millihz):
        if not 950_000 <= raw_millihz <= 105_000_000:
            return 0
        quantized = ((raw_millihz + 50_000) // 100_000) * 100_000
        return min(100_000_000, max(1_000_000, quantized))

    assert map_reference(999_900) == 1_000_000
    assert map_reference(100_000_100) == 100_000_000
    assert map_reference(949_999) == 0
    assert map_reference(105_000_001) == 0

    for frequency in (1_000, 2_000, 10_000, 50_000, 100_000):
        samples = sine_frame(frequency, 37, 0)
        raw, used, _, vpp = estimate_frequency(samples)
        assert abs(raw / 1000 - frequency) < 60
        assert used == frequency * 1000
        print(
            f"f={frequency:6d}Hz measured={raw/1000:10.3f}Hz "
            f"vpp={vpp}"
        )

    # The sine-IQ detector returns physical phase -90 degrees per channel.
    # Confirm that the 1F common offset still cancels and that v2.2.4 removes
    # the residual +90-degree bias from the 2F relative phase.
    reference = sine_frame(10_000, 23, 0, noise=0)
    feedback_1f = sine_frame(10_000, 71, 0, noise=0)
    feedback_2f = sine_frame(20_000, 71, 0, noise=0)
    phase_1f = relative_phase(reference, feedback_1f, 10_000, 1)
    phase_2f = relative_phase(reference, feedback_2f, 10_000, 2)
    assert abs(wrap_mdeg(phase_1f - 48_000)) < 100
    assert abs(wrap_mdeg(phase_2f - 25_000)) < 100

    phases_1f, slip_1f = check_phase_slope(10_000, 10_001, 1)
    phases_2f, slip_2f = check_phase_slope(10_000, 20_001, 2)
    assert abs(slip_1f + 1000) < 25
    assert abs(slip_2f + 1000) < 25
    print(f"1F phases={phases_1f} slip_error={slip_1f}mHz")
    print(f"2F phases={phases_2f} slip_error={slip_2f}mHz")

    frame = make_frame(
        7,
        1_235_000,
        10_000_000,
        10_000_000,
        phases_1f[-1],
        10_001_000,
        2400,
        2300,
        0x07,
        3,
        848,
        3248,
        900,
        3200,
        4820,
        0x0226,
    )
    assert len(frame) == 48
    assert frame[2] == 4
    assert struct.unpack_from("<I", frame, 7)[0] == 1_235_000
    assert struct.unpack_from("<H", frame, 32)[0] == 3
    assert struct.unpack_from("<H", frame, 42)[0] == 4820
    assert struct.unpack_from("<H", frame, 44)[0] == 0x0226
    assert crc16(frame[2:46]) == struct.unpack_from("<H", frame, 46)[0]
    corrupt = bytearray(frame)
    corrupt[15] ^= 1
    assert crc16(corrupt[2:46]) != struct.unpack_from("<H", corrupt, 46)[0]
    print("RESULT=PASS")


if __name__ == "__main__":
    main()
