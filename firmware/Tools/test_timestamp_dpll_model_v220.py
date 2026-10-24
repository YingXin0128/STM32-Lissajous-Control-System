#!/usr/bin/env python3
"""Timestamped 2 kHz plant model for the v2.2.0 centre-guard DPLL."""

from __future__ import annotations

import math
import random


NOMINAL_SYSCLK_HZ = 1_000_000_000.0
ACTUAL_SYSCLK_HZ = 999_988_085.597352
TARGET_HZ = 2_000.0
TARGET_PHASE_DEG = 90.0
CENTER_FTW = round(TARGET_HZ * 2.0**32 / NOMINAL_SYSCLK_HZ)
FILTER_DIVISOR = 8.0
KP_HZ_PER_DEG = 0.002
KI_HZ_PER_DEG_SECOND = 0.0002
MAX_CORRECTION_HZ = 0.5
MAX_STEP_HZ = 0.002
SLOPE_WINDOW_SECONDS = 1.0
SLOPE_MAX_NUDGE_HZ = 0.100


def wrap_degrees(value: float) -> float:
    while value > 180.0:
        value -= 360.0
    while value <= -180.0:
        value += 360.0
    return value


def run_model(duration_seconds: float = 60.0) -> dict[str, float]:
    random_source = random.Random(220)
    relative_phase = 145.0
    filtered_error = wrap_degrees(TARGET_PHASE_DEG - relative_phase)
    integral_hz = 0.0
    applied_correction_hz = 0.0
    fraction_accumulator = 0.0
    timestamp_us = 5_000
    last_timestamp_us = timestamp_us
    slope_start_us = timestamp_us
    slope_start_phase = relative_phase
    phase_aligned = False
    align_confirm = 0
    center_ftw_changes = 0
    results: list[tuple[float, float]] = []

    while timestamp_us < duration_seconds * 1_000_000:
        # Exercise real timestamp deltas: UART/service jitter does not alter
        # sample time, and two frames are deliberately dropped.
        delta_us = 10_000
        if 12_000_000 <= timestamp_us < 12_020_000:
            delta_us = 30_000
        timestamp_us += delta_us
        dt = (timestamp_us - last_timestamp_us) / 1_000_000.0
        last_timestamp_us = timestamp_us

        measured_phase = wrap_degrees(
            relative_phase + random_source.gauss(0.0, 1.2)
        )
        raw_error = wrap_degrees(TARGET_PHASE_DEG - measured_phase)
        filtered_error = wrap_degrees(
            filtered_error
            + wrap_degrees(raw_error - filtered_error) / FILTER_DIVISOR
        )

        if not phase_aligned and abs(filtered_error) > 4.0:
            align_confirm += 1
        else:
            align_confirm = 0
        if align_confirm >= 20:
            relative_phase = wrap_degrees(relative_phase + filtered_error)
            measured_phase = wrap_degrees(measured_phase + filtered_error)
            filtered_error = 0.0
            phase_aligned = True
            slope_start_us = timestamp_us
            slope_start_phase = measured_phase

        elapsed_us = timestamp_us - slope_start_us
        if elapsed_us >= SLOPE_WINDOW_SECONDS * 1_000_000:
            unwrapped = wrap_degrees(measured_phase - slope_start_phase)
            slip_hz = -unwrapped * 1_000_000.0 / (360.0 * elapsed_us)
            integral_hz += max(
                -SLOPE_MAX_NUDGE_HZ,
                min(SLOPE_MAX_NUDGE_HZ, 0.5 * slip_hz),
            )
            slope_start_us = timestamp_us
            slope_start_phase = measured_phase

        control_error = 0.0 if abs(filtered_error) <= 0.2 else filtered_error
        integral_error = max(-20.0, min(20.0, control_error))
        integral_hz += KI_HZ_PER_DEG_SECOND * integral_error * dt
        integral_hz = max(
            -MAX_CORRECTION_HZ, min(MAX_CORRECTION_HZ, integral_hz)
        )
        desired_correction_hz = max(
            -MAX_CORRECTION_HZ,
            min(
                MAX_CORRECTION_HZ,
                KP_HZ_PER_DEG * control_error + integral_hz,
            ),
        )
        step = max(
            -MAX_STEP_HZ,
            min(MAX_STEP_HZ, desired_correction_hz - applied_correction_hz),
        )
        applied_correction_hz += step

        desired_ftw = (
            CENTER_FTW
            + applied_correction_hz * 2.0**32 / NOMINAL_SYSCLK_HZ
        )
        base_ftw = math.floor(desired_ftw)
        fraction_accumulator += desired_ftw - base_ftw
        applied_ftw = base_ftw
        if fraction_accumulator >= 1.0:
            fraction_accumulator -= 1.0
            applied_ftw += 1

        output_hz = applied_ftw * ACTUAL_SYSCLK_HZ / 2.0**32
        relative_phase = wrap_degrees(
            relative_phase + 360.0 * (output_hz - TARGET_HZ) * dt
        )
        results.append((relative_phase, output_hz))

    settled = results[-1000:]
    mean_hz = sum(value[1] for value in settled) / len(settled)
    phase_errors = [
        abs(wrap_degrees(TARGET_PHASE_DEG - value[0])) for value in settled
    ]
    return {
        "centre_ftw": float(CENTER_FTW),
        "centre_ftw_changes": float(center_ftw_changes),
        "mean_frequency_error_millihz": (mean_hz - TARGET_HZ) * 1000.0,
        "max_phase_error_deg_last_10s": max(phase_errors),
        "rms_phase_error_deg_last_10s": math.sqrt(
            sum(value * value for value in phase_errors) / len(phase_errors)
        ),
    }


if __name__ == "__main__":
    result = run_model()
    for key, value in result.items():
        print(f"{key}={value:.9f}")
    assert result["centre_ftw_changes"] == 0.0
    assert abs(result["mean_frequency_error_millihz"]) < 5.0
    assert result["max_phase_error_deg_last_10s"] < 3.0
    assert result["rms_phase_error_deg_last_10s"] < 1.5
    print("PASS")
