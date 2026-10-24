#!/usr/bin/env python3
"""Noisy delayed plant model for the v2.1.2 phase-hold DPLL."""

from __future__ import annotations

import math
import random


NOMINAL_SYSCLK_HZ = 1_000_000_000.0
ACTUAL_SYSCLK_HZ = 999_988_085.597352
TARGET_HZ = 10_000.0
TARGET_PHASE_DEG = 90.0
FRAME_SECONDS = 0.010
CENTER_FTW = 42_950
FILTER_DIVISOR = 16.0
KP_HZ_PER_DEG = 0.001
KI_HZ_PER_DEG_SECOND = 0.00005
DEADBAND_DEG = 0.2
INTEGRAL_ERROR_LIMIT_DEG = 10.0
MAX_CORRECTION_HZ = 0.5
MAX_CORRECTION_STEP_HZ = 0.001
SLOPE_WINDOW_FRAMES = 400
SLOPE_GAIN = 0.5
SLOPE_MAX_NUDGE_HZ = 0.020
ALIGN_TRIGGER_DEG = 4.0
ALIGN_CONFIRM_FRAMES = 20
ALIGN_MAX_STEP_DEG = 30.0
ALIGN_HOLDOFF_FRAMES = 400


def wrap_degrees(value: float) -> float:
    while value > 180.0:
        value -= 360.0
    while value <= -180.0:
        value += 360.0
    return value


def run_model(duration_seconds: float = 90.0) -> dict[str, float]:
    random_source = random.Random(20260731)
    relative_phase = 35.0
    delayed_phase = relative_phase
    filtered_error = wrap_degrees(TARGET_PHASE_DEG - delayed_phase)
    integral_hz = 0.0
    applied_correction_hz = 0.0
    fraction_accumulator = 0.0
    previous_ftw = CENTER_FTW
    io_updates = 0
    pow_alignments = 0
    last_alignment_second = 0.0
    align_confirm_frames = 0
    align_holdoff_frames = 0
    slope_unwrapped_degrees = 0.0
    slope_previous_phase = delayed_phase
    slope_frames = 0
    samples: list[tuple[float, float, float, int]] = []

    frame_count = int(round(duration_seconds / FRAME_SECONDS))
    for frame in range(frame_count):
        # Inject one fixed analogue-path disturbance to exercise reacquisition.
        if frame == int(30.0 / FRAME_SECONDS):
            relative_phase = wrap_degrees(relative_phase - 12.0)

        measured_phase = wrap_degrees(
            delayed_phase + random_source.gauss(0.0, 1.5)
        )
        delayed_phase = relative_phase

        phase_delta = wrap_degrees(measured_phase - slope_previous_phase)
        slope_previous_phase = measured_phase
        slope_unwrapped_degrees += phase_delta
        slope_frames += 1
        if slope_frames >= SLOPE_WINDOW_FRAMES:
            slope_error_hz = -(
                slope_unwrapped_degrees
                / (360.0 * slope_frames * FRAME_SECONDS)
            )
            slope_nudge_hz = max(
                -SLOPE_MAX_NUDGE_HZ,
                min(SLOPE_MAX_NUDGE_HZ, SLOPE_GAIN * slope_error_hz),
            )
            integral_hz += slope_nudge_hz
            slope_unwrapped_degrees = 0.0
            slope_frames = 0

        raw_error = wrap_degrees(TARGET_PHASE_DEG - measured_phase)
        filter_delta = wrap_degrees(raw_error - filtered_error)
        filtered_error = wrap_degrees(
            filtered_error + filter_delta / FILTER_DIVISOR
        )

        if align_holdoff_frames:
            align_holdoff_frames -= 1
            align_confirm_frames = 0
        elif abs(filtered_error) > ALIGN_TRIGGER_DEG:
            align_confirm_frames += 1
        else:
            align_confirm_frames = 0
        if align_confirm_frames >= ALIGN_CONFIRM_FRAMES:
            phase_step = max(
                -ALIGN_MAX_STEP_DEG,
                min(ALIGN_MAX_STEP_DEG, filtered_error),
            )
            relative_phase = wrap_degrees(relative_phase + phase_step)
            delayed_phase = wrap_degrees(delayed_phase + phase_step)
            filtered_error = 0.0
            align_confirm_frames = 0
            align_holdoff_frames = ALIGN_HOLDOFF_FRAMES
            slope_unwrapped_degrees = 0.0
            slope_previous_phase = relative_phase
            slope_frames = 0
            pow_alignments += 1
            last_alignment_second = frame * FRAME_SECONDS

        control_error = filtered_error
        if abs(control_error) <= DEADBAND_DEG:
            control_error = 0.0
        integral_error = max(
            -INTEGRAL_ERROR_LIMIT_DEG,
            min(INTEGRAL_ERROR_LIMIT_DEG, control_error),
        )
        integral_hz += (
            KI_HZ_PER_DEG_SECOND * integral_error * FRAME_SECONDS
        )
        integral_hz = max(
            -MAX_CORRECTION_HZ,
            min(MAX_CORRECTION_HZ, integral_hz),
        )

        desired_correction_hz = (
            KP_HZ_PER_DEG * control_error + integral_hz
        )
        desired_correction_hz = max(
            -MAX_CORRECTION_HZ,
            min(MAX_CORRECTION_HZ, desired_correction_hz),
        )
        correction_step_hz = (
            desired_correction_hz - applied_correction_hz
        )
        correction_step_hz = max(
            -MAX_CORRECTION_STEP_HZ,
            min(MAX_CORRECTION_STEP_HZ, correction_step_hz),
        )
        applied_correction_hz += correction_step_hz

        desired_ftw = (
            CENTER_FTW
            + applied_correction_hz * (2.0**32) / NOMINAL_SYSCLK_HZ
        )
        base_ftw = math.floor(desired_ftw)
        fraction_accumulator += desired_ftw - base_ftw
        applied_ftw = base_ftw
        if fraction_accumulator >= 1.0:
            fraction_accumulator -= 1.0
            applied_ftw += 1
        if applied_ftw != previous_ftw:
            previous_ftw = applied_ftw
            io_updates += 1

        output_hz = applied_ftw * ACTUAL_SYSCLK_HZ / (2.0**32)
        relative_phase = wrap_degrees(
            relative_phase
            + 360.0 * (output_hz - TARGET_HZ) * FRAME_SECONDS
        )
        samples.append(
            (relative_phase, filtered_error, output_hz, applied_ftw)
        )

    settled = samples[-2000:]
    mean_output_hz = sum(item[2] for item in settled) / len(settled)
    actual_phase_errors = [
        abs(wrap_degrees(TARGET_PHASE_DEG - item[0]))
        for item in settled
    ]
    return {
        "mean_output_hz_last_20s": mean_output_hz,
        "mean_frequency_error_millihz": (
            mean_output_hz - TARGET_HZ
        )
        * 1000.0,
        "max_actual_phase_error_deg_last_20s": max(
            actual_phase_errors
        ),
        "rms_actual_phase_error_deg_last_20s": math.sqrt(
            sum(value * value for value in actual_phase_errors)
            / len(actual_phase_errors)
        ),
        "final_actual_phase_error_deg": wrap_degrees(
            TARGET_PHASE_DEG - samples[-1][0]
        ),
        "pow_alignments": float(pow_alignments),
        "last_alignment_second": last_alignment_second,
        "io_update_transitions": float(io_updates),
    }


if __name__ == "__main__":
    result = run_model()
    for key, value in result.items():
        print(f"{key}={value:.9f}")

    assert abs(result["mean_frequency_error_millihz"]) < 1.0
    assert result["max_actual_phase_error_deg_last_20s"] < 2.0
    assert result["rms_actual_phase_error_deg_last_20s"] < 1.0
    assert result["pow_alignments"] <= 10.0
    assert result["last_alignment_second"] < 70.0
    print("PASS")
