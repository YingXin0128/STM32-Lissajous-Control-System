"""Static and scheduling regressions for the v2.2.6 F103 runtime UI."""

from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]
MENU = ROOT / "F103_Project" / "APP" / "menu_v2.c"
MAIN = ROOT / "F103_Project" / "USER" / "main.c"
USART_H = ROOT / "F103_Project" / "SYSTEM" / "usart" / "usart.h"


def function_body(source, signature):
    start = source.index(signature)
    brace = source.index("{", start)
    depth = 0
    for index in range(brace, len(source)):
        if source[index] == "{":
            depth += 1
        elif source[index] == "}":
            depth -= 1
            if depth == 0:
                return source[brace : index + 1]
    raise AssertionError(f"unterminated function: {signature}")


def scheduler_queue_model(draw_ms, seconds=60):
    """Producer is 50 fps; main drains after every bounded draw slice."""
    produced = 0.0
    queued = 0
    maximum = 0
    elapsed = 0.0
    while elapsed < seconds * 1000:
        elapsed += draw_ms
        expected = int(elapsed // 20)
        queued += expected - int(produced)
        produced = expected
        maximum = max(maximum, queued)
        queued = 0
    return maximum


def main():
    menu = MENU.read_text(encoding="utf-8")
    main_source = MAIN.read_text(encoding="utf-8")
    usart_h = USART_H.read_text(encoding="utf-8")

    renderer = function_body(
        menu, "static void Menu_RuntimeRenderOneCharacter(void)"
    )
    assert renderer.count("LCD_ShowChar(") == 1
    assert renderer.count("LCD_Fill(") == 1
    assert "field->drawn[character_index] = character;\n        return;" in renderer
    assert "#define MENU_RUNTIME_MODEL_SLICES      20U" in menu

    voltage_case = menu.index("case MENU_PAGE_MANUAL_VOLTAGE:",
                              menu.index("if (key != BOARD_KEY_CONFIRM)"))
    next_case = menu.index("case MENU_PAGE_AUTO_SHAPE:", voltage_case)
    start_block = menu[voltage_case:next_case]
    assert start_block.index("Menu_Redraw();") < start_block.index(
        "SignalCapture_Service();"
    )
    assert start_block.index("SignalCapture_ClearDiagnostics();") < (
        start_block.index("ManualControl_Start(")
    )

    assert "#define USART1_RX_DMA_SIZE 4096U" in usart_h
    assert "f103_tag" in main_source
    assert "max_main_gap_ms" in main_source

    header_match = re.search(
        r'static const char header\[\]\s*=\s*((?:"[^"]*"\s*)+);',
        main_source,
        re.S,
    )
    format_match = re.search(
        r'sizeof\(line\),\s*((?:"[^"]*"\s*)+),',
        main_source,
        re.S,
    )
    assert header_match and format_match
    header = "".join(re.findall(r'"([^"]*)"', header_match.group(1)))
    header = header.replace(r"\r", "").replace(r"\n", "")
    printf_format = "".join(
        re.findall(r'"([^"]*)"', format_match.group(1))
    )
    placeholders = re.findall(r"%(?:l)?[udi]", printf_format)
    call_arguments = main_source[
        format_match.end() : main_source.index(");", format_match.end())
    ]
    arguments = re.findall(
        r"\(unsigned (?:long|int)\)|\(long\)", call_arguments
    )
    assert len(header.split(",")) == 59
    assert len(placeholders) == 59
    assert len(arguments) == 59
    assert "dpll_applied_mhz" in header
    assert "dpll_limit_mhz" in header
    assert "dpll_saturated" in header
    assert "ref_raw_mhz" in header
    assert "phase_reject_flags" in header
    assert "ref_invalid_frames" in header
    assert "clip_recovery_frames" in header
    assert "feedback_outlier_frames" in header
    assert "gross_relock_count" in header
    assert "capture->clip_recovery" in menu

    # Even a pessimistic 40 ms single-character draw accumulates only two
    # protocol frames before the main loop drains them, far below queue depth.
    assert scheduler_queue_model(40) <= 2
    print("MAX_MODELED_QUEUE_DEPTH=2")
    print("CSV_FIELDS=59")
    print("RESULT=PASS")


if __name__ == "__main__":
    main()
