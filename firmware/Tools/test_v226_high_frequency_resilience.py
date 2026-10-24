"""Focused regressions for v2.2.6 high-frequency resilience."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
F407_CONFIG = ROOT / "F407_Project" / "MY" / "app_config.h"
MEASUREMENT = ROOT / "F407_Project" / "MY" / "measurement.c"
F103_CONFIG = ROOT / "F103_Project" / "USER" / "app_config.h"
CONTROL = ROOT / "F103_Project" / "APP" / "manual_control.c"
CAPTURE_H = ROOT / "F103_Project" / "HARDWARE" / "CAPTURE" / "signal_capture.h"
CAPTURE_C = ROOT / "F103_Project" / "HARDWARE" / "CAPTURE" / "signal_capture.c"


def wrap_mdeg(value):
    while value > 180_000:
        value -= 360_000
    while value <= -180_000:
        value += 360_000
    return value


def reported_relative(physical_reference, physical_feedback, harmonic):
    detector_reference = physical_reference - 90_000
    detector_feedback = physical_feedback - 90_000
    return wrap_mdeg(
        detector_feedback
        - harmonic * detector_reference
        - (harmonic - 1) * 90_000
    )


def dpll_limit(shape, target_millihz):
    if target_millihz >= 50_000_000:
        return 1000
    return 500


def feedback_valid(frequency_millihz):
    return 950_000 <= frequency_millihz <= 205_000_000


def clip_recovery(clipped_sequence, required_clean=3):
    clean = required_clean
    states = []
    for clipped in clipped_sequence:
        if clipped:
            clean = 0
        elif clean < required_clean:
            clean += 1
        states.append(clean < required_clean)
    return states


def outlier_acceptance(deltas_millihz, threshold=5_000, confirm=5):
    direction = 0
    count = 0
    accepted = []
    for delta in deltas_millihz:
        if abs(delta) <= threshold:
            direction = 0
            count = 0
            accepted.append(True)
            continue
        next_direction = 1 if delta > 0 else -1
        if next_direction == direction:
            count += 1
        else:
            direction = next_direction
            count = 1
        accepted.append(count >= confirm)
        if count >= confirm:
            direction = 0
            count = 0
    return accepted


def phase_gap_is_bridgeable(sequence_delta, timestamp_delta_us):
    return sequence_delta <= 5 and timestamp_delta_us <= 110_000


def main():
    f407_config = F407_CONFIG.read_text(encoding="utf-8")
    measurement = MEASUREMENT.read_text(encoding="utf-8")
    f103_config = F103_CONFIG.read_text(encoding="utf-8")
    control = CONTROL.read_text(encoding="utf-8")
    capture_h = CAPTURE_H.read_text(encoding="utf-8")
    capture_c = CAPTURE_C.read_text(encoding="utf-8")

    assert "APP_FIRMWARE_TAG                0x0226U" in f407_config
    assert "APP_F103_FIRMWARE_TAG             0x0226U" in f103_config
    assert "APP_FEEDBACK_MIN_MHZ            950000UL" in f407_config
    assert "APP_ADC_CLIP_RECOVERY_FRAMES    3U" in f407_config
    assert "APP_FLAG_CLIP_RECOVERY          0x80U" in f407_config
    assert "((*harmonic) - 1U) * 90000L" in measurement
    assert "clip_clean_frames < APP_ADC_CLIP_RECOVERY_FRAMES" in measurement
    assert "APP_DPLL_HIGH_FREQ_LIMIT_MHZ       1000L" in f103_config
    assert "APP_DPLL_HIGH_FREQ_THRESHOLD_MHZ   50000000UL" in f103_config
    limit_start = control.index("static int32_t Control_DpllCorrectionLimit")
    limit_end = control.index("static uint32_t Control_FtwToMilliHz", limit_start)
    limit_body = control[limit_start:limit_end]
    assert "control_status.shape" not in limit_body
    assert "control_status.dpll_correction_saturated" in control
    assert "APP_DPLL_KP_NUMERATOR              2L" in f103_config
    assert "APP_DPLL_KI_DENOMINATOR            5L" in f103_config
    assert "APP_DPLL_PHASE_LOCK_MDEG           1500L" in f103_config
    assert "APP_DPLL_PHASE_LOCK_FRAMES         75U" in f103_config
    assert "APP_LOCK_GROSS_ERROR_MHZ          750L" in f103_config
    assert "APP_LOCK_GROSS_FRAMES             3U" in f103_config
    assert "APP_LOCKED_FEEDBACK_OUTLIER_MHZ   5000L" in f103_config
    assert "APP_FEEDBACK_OUTLIER_CONFIRM_FRAMES 5U" in f103_config
    assert "APP_PHASE_MAX_SEQUENCE_GAP        5U" in f103_config
    assert "APP_TIMESTAMP_MAX_GAP_US          110000UL" in f103_config
    assert "feedback_outlier_frames++" in control
    assert "gross_relock_count++" in control

    slope_start = control.index("static uint8_t Control_UpdatePhaseSlope")
    slope_end = control.index("static void Control_UpdateAcquire", slope_start)
    slope_body = control[slope_start:slope_end]
    invalid_start = slope_body.index("capture->relative_phase_valid == 0U")
    settle_start = slope_body.index("settle_frames_remaining", invalid_start)
    assert "Control_ResetPhaseWindow" not in slope_body[invalid_start:settle_start]

    dpll_start = control.index("static void Control_UpdateDpll")
    dpll_end = control.index("integral_denominator =", dpll_start)
    invalid_dpll = control[dpll_start:dpll_end]
    assert "dpll_sequence_valid = 0U" not in invalid_dpll
    assert "dpll_phase_filter_valid = 0U" not in invalid_dpll
    assert "SIGNAL_CAPTURE_REJECT_CLIP_RECOVERY" in capture_h
    assert "link_diagnostics.feedback_invalid_frames" in capture_c
    assert "link_diagnostics.phase_invalid_frames" in capture_c

    assert reported_relative(23_000, 71_000, 1) == 48_000
    assert reported_relative(23_000, 71_000, 2) == 25_000
    assert dpll_limit("infinity", 20_000_000) == 500
    assert dpll_limit("infinity", 50_000_000) == 1000
    assert dpll_limit("infinity", 100_000_000) == 1000
    assert dpll_limit("circle", 50_000_000) == 1000
    assert dpll_limit("diagonal", 75_000_000) == 1000
    assert feedback_valid(999_750)
    assert feedback_valid(950_000)
    assert not feedback_valid(949_999)
    assert clip_recovery([False, True, False, False, False, False]) == [
        False, True, True, True, False, False
    ]
    assert outlier_acceptance([0, 46_000, 0]) == [True, False, True]
    assert outlier_acceptance([46_000] * 5) == [
        False, False, False, False, True
    ]
    assert outlier_acceptance([46_000, -46_000] * 3) == [False] * 6
    assert phase_gap_is_bridgeable(4, 80_000)
    assert phase_gap_is_bridgeable(5, 100_000)
    assert not phase_gap_is_bridgeable(6, 120_000)
    print("PHASE_1F_MDEG=48000")
    print("PHASE_2F_MDEG=25000")
    print("OUTLIER_GATE_AND_PHASE_GAP_BRIDGE=PASS")
    print("RESULT=PASS")


if __name__ == "__main__":
    main()
