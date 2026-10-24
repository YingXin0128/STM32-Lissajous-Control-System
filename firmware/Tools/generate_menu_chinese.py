from pathlib import Path

from PIL import Image, ImageDraw, ImageFont


TEXTS = {
    "TITLE": "AD9910 \u7A0B\u63A7\u7CFB\u7EDF",
    "HOME_MANUAL": "\u624B\u52A8\u6A21\u5F0F",
    "HOME_AUTO": "\u81EA\u52A8\u6A21\u5F0F",
    "HOME_BYPASS": "\u76F4\u901A\u6A21\u5F0F",
    "FOOTER_NAV": "PE13\u4E0A  PE14\u4E0B  \u5916\u63A5\u952E\u786E\u5B9A",
    "TITLE_MANUAL_SHAPE": "\u624B\u52A8 / \u5F62\u72B6",
    "SHAPE_DIAGONAL": "\u5BF9\u89D2\u7EBF",
    "SHAPE_CIRCLE": "\u5706",
    "SHAPE_INFINITY": "\u65E0\u7A77",
    "FOOTER_BACK_OK": "PE15\u8FD4\u56DE  \u5916\u63A5\u952E\u786E\u5B9A",
    "TITLE_MANUAL_LEVEL": "\u624B\u52A8 / \u5E45\u5EA6",
    "LEVEL_4V": "4V -> 666.0 mVpp",
    "LEVEL_3V": "3V -> 500.0 mVpp",
    "LEVEL_2V": "2V -> 333.0 mVpp",
    "LEVEL_1V": "1V -> 166.5 mVpp",
    "FOOTER_BACK_START": "PE15\u8FD4\u56DE  \u5916\u63A5\u952E\u542F\u52A8",
    "TITLE_MANUAL_RUN": "\u624B\u52A8\u8FD0\u884C",
    "FOOTER_BACK_MUTE": "PE15\u8FD4\u56DE / \u8F93\u51FA\u5173\u95ED",
    "TITLE_AUTO_SHAPE": "\u81EA\u52A8 / \u5F62\u72B6",
    "TITLE_AUTO_MODE": "\u81EA\u52A8\u6A21\u5F0F",
    "SELECTED": "\u5DF2\u9009\u62E9:",
    "NOT_IMPLEMENTED": "\u529F\u80FD\u6682\u672A\u5B9E\u73B0",
    "RESERVED": "\u5DF2\u9884\u7559\u6269\u5C55\u63A5\u53E3",
    "FOOTER_BACK": "PE15\u8FD4\u56DE",
    "TITLE_BYPASS": "\u76F4\u901A\u6A21\u5F0F",
    "RELAY_ON": "\u7EE7\u7535\u5668: \u5F00\u542F",
    "PD2_HIGH": "PD2 = \u9AD8\u7535\u5E73",
    "SIGNAL_TO_SCOPE": "\u4FE1\u53F7\u76F4\u63A5\u9001\u81F3\u793A\u6CE2\u5668",
    "FOOTER_RELAY_OFF": "PE15\u8FD4\u56DE / \u7EE7\u7535\u5668\u5173\u95ED",
    "LABEL_OUTPUT": "\u8F93\u51FA ",
    "LABEL_STATE": "\u72B6\u6001: ",
    "LABEL_RAW": "\u539F\u59CB: ",
    "LABEL_USED": "\u4F7F\u7528: ",
    "LABEL_TARGET": "\u76EE\u6807: ",
    "LABEL_COMMAND": "\u6307\u4EE4: ",
    "LABEL_FEEDBACK": "\u53CD\u9988: ",
    "LABEL_PHASE": "\u76F8\u4F4D: ",
    "LABEL_CORRECTION": "PI\u4FEE\u6B63: ",
    "LABEL_ERROR": "\u8BEF\u5DEE: ",
    "LABEL_PLL": "PLL\u9501\u5B9A: ",
    "STATE_HOLD": "\u53C2\u8003\u4E22\u5931 / \u4FDD\u6301\u8F93\u51FA",
    "STATE_OPEN_LOOP": "\u5F00\u73AF",
    "STATE_CLOSED_LOOP": "\u9891\u7387\u95ED\u73AF",
    "STATE_STOPPED": "\u505C\u6B62",
    "HIGH": "\u9AD8",
    "LOW": "\u4F4E",
}


def encode_c_bytes(text):
    encoded = text.encode("gbk")
    return ",".join("0x%02XU" % value for value in encoded) + ",0x00U"


def glyph_bitmap(font, character):
    image = Image.new("1", (16, 16), 0)
    draw = ImageDraw.Draw(image)
    draw.text((0, 0), character, font=font, fill=1)

    data = []
    for row in range(16):
        for byte_column in range(2):
            value = 0
            for bit in range(8):
                value <<= 1
                if image.getpixel((byte_column * 8 + bit, row)):
                    value |= 1
            data.append(value)
    return data


def main():
    script_dir = Path(__file__).resolve().parent
    project_dir = script_dir.parent / "Keil_Project"
    output_header = project_dir / "APP" / "menu_chinese.h"
    output_source = project_dir / "APP" / "menu_chinese.c"

    font_path = Path(r"C:\Windows\Fonts\simsun.ttc")
    if not font_path.exists():
        raise SystemExit("Required font is missing: %s" % font_path)
    font = ImageFont.truetype(str(font_path), 16)

    characters = []
    for text in TEXTS.values():
        for character in text:
            if ord(character) > 0x7F and character not in characters:
                characters.append(character)

    header_lines = [
        "#ifndef __MENU_CHINESE_H",
        "#define __MENU_CHINESE_H",
        "",
        '#include "stm32f10x.h"',
        "",
        "typedef enum",
        "{",
    ]
    for name in TEXTS:
        header_lines.append("    MENU_TEXT_%s," % name)
    header_lines.extend(
        [
            "    MENU_TEXT_COUNT",
            "} MenuTextId;",
            "",
            "const uint8_t *MenuChinese_GetText(MenuTextId text_id);",
            "uint16_t MenuChinese_TextWidth(const uint8_t *text);",
            "uint16_t MenuChinese_DrawText(uint16_t x,",
            "                              uint16_t y,",
            "                              uint16_t right_x,",
            "                              uint16_t color,",
            "                              const uint8_t *text);",
            "",
            "#endif",
            "",
        ]
    )

    source_lines = [
        '#include "menu_chinese.h"',
        '#include "lcd.h"',
        "",
        "typedef struct",
        "{",
        "    uint16_t gbk_code;",
        "    uint8_t bitmap[32];",
        "} MenuChineseGlyph;",
        "",
        "static const MenuChineseGlyph menu_glyphs[] =",
        "{",
    ]
    for character in characters:
        gbk = character.encode("gbk")
        code = (gbk[0] << 8) | gbk[1]
        bitmap = glyph_bitmap(font, character)
        source_lines.append(
            "    {0x%04XU, {%s}}, /* U+%04X */"
            % (
                code,
                ",".join("0x%02XU" % value for value in bitmap),
                ord(character),
            )
        )
    source_lines.extend(["};", ""])

    for name, text in TEXTS.items():
        source_lines.append(
            "static const uint8_t menu_text_%s[] = {%s};"
            % (name.lower(), encode_c_bytes(text))
        )

    source_lines.extend(
        [
            "",
            "static const uint8_t *const menu_texts[MENU_TEXT_COUNT] =",
            "{",
        ]
    )
    for name in TEXTS:
        source_lines.append("    menu_text_%s," % name.lower())
    source_lines.extend(
        [
            "};",
            "",
            "static const uint8_t menu_empty_text[] = {0x00U};",
            "",
            "static const uint8_t *MenuChinese_FindGlyph(uint16_t gbk_code)",
            "{",
            "    uint16_t index;",
            "    uint16_t count;",
            "",
            "    count = (uint16_t)(sizeof(menu_glyphs) / sizeof(menu_glyphs[0]));",
            "    for (index = 0U; index < count; index++)",
            "    {",
            "        if (menu_glyphs[index].gbk_code == gbk_code)",
            "            return menu_glyphs[index].bitmap;",
            "    }",
            "    return 0;",
            "}",
            "",
            "static void MenuChinese_DrawGlyph(uint16_t x,",
            "                                  uint16_t y,",
            "                                  uint16_t color,",
            "                                  const uint8_t *bitmap)",
            "{",
            "    uint8_t row;",
            "    uint8_t column;",
            "    uint16_t row_bits;",
            "",
            "    if (bitmap == 0)",
            "        return;",
            "",
            "    for (row = 0U; row < 16U; row++)",
            "    {",
            "        row_bits = (uint16_t)(((uint16_t)bitmap[row * 2U] << 8) |",
            "                              bitmap[(row * 2U) + 1U]);",
            "        for (column = 0U; column < 16U; column++)",
            "        {",
            "            if ((row_bits & (uint16_t)(0x8000U >> column)) != 0U)",
            "                LCD_Fast_DrawPoint((uint16_t)(x + column),",
            "                                   (uint16_t)(y + row),",
            "                                   color);",
            "        }",
            "    }",
            "}",
            "",
            "const uint8_t *MenuChinese_GetText(MenuTextId text_id)",
            "{",
            "    if ((uint16_t)text_id >= (uint16_t)MENU_TEXT_COUNT)",
            "        return menu_empty_text;",
            "    return menu_texts[(uint16_t)text_id];",
            "}",
            "",
            "uint16_t MenuChinese_TextWidth(const uint8_t *text)",
            "{",
            "    uint16_t width;",
            "",
            "    width = 0U;",
            "    if (text == 0)",
            "        return width;",
            "",
            "    while (*text != 0U)",
            "    {",
            "        if (*text < 0x80U)",
            "        {",
            "            width = (uint16_t)(width + 8U);",
            "            text++;",
            "        }",
            "        else if (text[1] != 0U)",
            "        {",
            "            width = (uint16_t)(width + 16U);",
            "            text += 2;",
            "        }",
            "        else",
            "        {",
            "            break;",
            "        }",
            "    }",
            "    return width;",
            "}",
            "",
            "uint16_t MenuChinese_DrawText(uint16_t x,",
            "                              uint16_t y,",
            "                              uint16_t right_x,",
            "                              uint16_t color,",
            "                              const uint8_t *text)",
            "{",
            "    uint16_t gbk_code;",
            "",
            "    if (text == 0)",
            "        return x;",
            "",
            "    while (*text != 0U)",
            "    {",
            "        if (*text < 0x80U)",
            "        {",
            "            if ((uint16_t)(x + 8U) > right_x)",
            "                break;",
            "            if ((*text >= (uint8_t)' ') && (*text <= (uint8_t)'~'))",
            "                LCD_ShowChar(x, y, *text, 16U, color, 1U);",
            "            x = (uint16_t)(x + 8U);",
            "            text++;",
            "        }",
            "        else if (text[1] != 0U)",
            "        {",
            "            if ((uint16_t)(x + 16U) > right_x)",
            "                break;",
            "            gbk_code = (uint16_t)(((uint16_t)text[0] << 8) | text[1]);",
            "            MenuChinese_DrawGlyph(x,",
            "                                  y,",
            "                                  color,",
            "                                  MenuChinese_FindGlyph(gbk_code));",
            "            x = (uint16_t)(x + 16U);",
            "            text += 2;",
            "        }",
            "        else",
            "        {",
            "            break;",
            "        }",
            "    }",
            "    return x;",
            "}",
            "",
        ]
    )

    output_header.write_text("\n".join(header_lines), encoding="ascii")
    output_source.write_text("\n".join(source_lines), encoding="ascii")
    print("Generated %s and %s (%u glyphs)" %
          (output_header, output_source, len(characters)))


if __name__ == "__main__":
    main()
