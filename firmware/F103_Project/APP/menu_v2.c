#include "menu.h"
#include "lcd.h"
#include "menu_chinese.h"
#include "manual_control.h"
#include "auto_mode.h"
#include <stdio.h>

typedef enum
{
    MENU_PAGE_HOME = 0,
    MENU_PAGE_MANUAL_SHAPE,
    MENU_PAGE_MANUAL_VOLTAGE,
    MENU_PAGE_MANUAL_RUN,
    MENU_PAGE_AUTO_SHAPE,
    MENU_PAGE_AUTO_PLACEHOLDER,
    MENU_PAGE_BYPASS_ACTIVE
} MenuPage;

static MenuPage current_page;
static uint8_t current_selection;
static App_Shape selected_shape;
static App_Voltage selected_voltage;

#define MENU_RUNTIME_FIELD_COUNT       11U
#define MENU_RUNTIME_TEXT_CAPACITY     40U
#define MENU_RUNTIME_MODEL_SLICES      20U

typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t color;
    uint8_t size;
    uint8_t max_chars;
    char target[MENU_RUNTIME_TEXT_CAPACITY];
    char drawn[MENU_RUNTIME_TEXT_CAPACITY];
} MenuRuntimeField;

static MenuRuntimeField runtime_fields[MENU_RUNTIME_FIELD_COUNT];
static uint8_t runtime_field_cursor;
static uint8_t runtime_character_cursor;
static uint8_t runtime_model_countdown;

static void Menu_RuntimePrepareLayout(void);

static const MenuTextId home_options[] =
{
    MENU_TEXT_HOME_MANUAL,
    MENU_TEXT_HOME_AUTO,
    MENU_TEXT_HOME_BYPASS
};

static const MenuTextId shape_options[] =
{
    MENU_TEXT_SHAPE_DIAGONAL,
    MENU_TEXT_SHAPE_CIRCLE,
    MENU_TEXT_SHAPE_INFINITY
};

static const MenuTextId voltage_options[] =
{
    MENU_TEXT_LEVEL_4V,
    MENU_TEXT_LEVEL_3V,
    MENU_TEXT_LEVEL_2V,
    MENU_TEXT_LEVEL_1V
};

static void Menu_ShowAscii(uint16_t x,
                           uint16_t y,
                           uint16_t right_x,
                           uint8_t size,
                           uint16_t color,
                           const char *text)
{
    uint16_t character_width;

    if (text == 0)
        return;

    character_width = (uint16_t)(size / 2U);
    while (*text != '\0')
    {
        if ((uint16_t)(x + character_width) > right_x)
            break;
        if ((*text >= ' ') && (*text <= '~'))
            LCD_ShowChar(x, y, (u8)*text, size, color, 1U);
        x = (uint16_t)(x + character_width);
        text++;
    }
}

static void Menu_ShowTextId(uint16_t x,
                            uint16_t y,
                            uint16_t right_x,
                            uint16_t color,
                            MenuTextId text_id)
{
    MenuChinese_DrawText(x,
                         y,
                         right_x,
                         color,
                         MenuChinese_GetText(text_id));
}

static void Menu_DrawHeader(MenuTextId title)
{
    LCD_Fill(0U, 0U, 239U, 39U, DARKBLUE);
    Menu_ShowTextId(10U, 11U, 234U, WHITE, title);
}

static void Menu_DrawFooter(MenuTextId hint)
{
    LCD_Fill(0U, 286U, 239U, 319U, GRAY0);
    Menu_ShowTextId(6U, 296U, 236U, BLACK, hint);
}

static uint8_t Menu_OptionCount(void)
{
    switch (current_page)
    {
        case MENU_PAGE_HOME:
            return 3U;

        case MENU_PAGE_MANUAL_SHAPE:
        case MENU_PAGE_AUTO_SHAPE:
            return 3U;

        case MENU_PAGE_MANUAL_VOLTAGE:
            return 4U;

        default:
            return 0U;
    }
}

static MenuTextId Menu_OptionText(uint8_t index)
{
    switch (current_page)
    {
        case MENU_PAGE_HOME:
            return home_options[index];

        case MENU_PAGE_MANUAL_SHAPE:
        case MENU_PAGE_AUTO_SHAPE:
            return shape_options[index];

        case MENU_PAGE_MANUAL_VOLTAGE:
            return voltage_options[index];

        default:
            return MENU_TEXT_TITLE;
    }
}

static void Menu_DrawOptionRow(uint8_t index)
{
    uint16_t y;
    uint16_t background;
    uint16_t foreground;
    uint16_t text_width;
    uint16_t text_x;
    const uint8_t *text;

    y = (uint16_t)(62U + ((uint16_t)index * 46U));
    if (index == current_selection)
    {
        background = DARKBLUE;
        foreground = WHITE;
    }
    else
    {
        background = GRAY0;
        foreground = BLACK;
    }

    LCD_Fill(15U, y, 224U, (uint16_t)(y + 34U), background);

    text = MenuChinese_GetText(Menu_OptionText(index));
    text_width = MenuChinese_TextWidth(text);
    if (text_width < 210U)
        text_x = (uint16_t)(15U + ((210U - text_width) / 2U));
    else
        text_x = 20U;

    /*
     * Text is drawn transparently over the already completed row background.
     * This prevents LCD_ShowString() from painting white character boxes over
     * the blue selection bar.
     */
    MenuChinese_DrawText(text_x,
                         (uint16_t)(y + 9U),
                         220U,
                         foreground,
                         text);
}

static void Menu_DrawOptions(void)
{
    uint8_t index;
    uint8_t count;

    count = Menu_OptionCount();
    for (index = 0U; index < count; index++)
        Menu_DrawOptionRow(index);
}

static void Menu_UpdateSelection(uint8_t previous_selection)
{
    if (previous_selection == current_selection)
        return;
    Menu_DrawOptionRow(previous_selection);
    Menu_DrawOptionRow(current_selection);
}

static void Menu_DrawHome(void)
{
    LCD_Clear(WHITE);
    Menu_DrawHeader(MENU_TEXT_TITLE);
    Menu_DrawOptions();
    Menu_DrawFooter(MENU_TEXT_FOOTER_NAV);
}

static void Menu_DrawShape(uint8_t automatic)
{
    LCD_Clear(WHITE);
    Menu_DrawHeader((automatic != 0U) ?
                    MENU_TEXT_TITLE_AUTO_SHAPE :
                    MENU_TEXT_TITLE_MANUAL_SHAPE);
    Menu_DrawOptions();
    Menu_DrawFooter(MENU_TEXT_FOOTER_BACK_OK);
}

static void Menu_DrawVoltage(void)
{
    LCD_Clear(WHITE);
    Menu_DrawHeader(MENU_TEXT_TITLE_MANUAL_LEVEL);
    Menu_DrawOptions();
    Menu_DrawFooter(MENU_TEXT_FOOTER_BACK_START);
}

static void Menu_DrawManualRunFrame(void)
{
    LCD_Clear(WHITE);
    Menu_DrawHeader(MENU_TEXT_TITLE_MANUAL_RUN);
    Menu_DrawFooter(MENU_TEXT_FOOTER_BACK_MUTE);
    Menu_RuntimePrepareLayout();
}

static MenuTextId Menu_ShapeText(App_Shape shape)
{
    if (shape == APP_SHAPE_CIRCLE)
        return MENU_TEXT_SHAPE_CIRCLE;
    if (shape == APP_SHAPE_INFINITY)
        return MENU_TEXT_SHAPE_INFINITY;
    return MENU_TEXT_SHAPE_DIAGONAL;
}

static void Menu_DrawAutoPlaceholder(void)
{
    LCD_Clear(WHITE);
    Menu_DrawHeader(MENU_TEXT_TITLE_AUTO_MODE);
    Menu_ShowTextId(15U, 70U, 225U, BLACK, MENU_TEXT_SELECTED);
    Menu_ShowTextId(15U,
                    100U,
                    225U,
                    BLUE,
                    Menu_ShapeText(AutoMode_GetShape()));
    Menu_ShowTextId(15U,
                    150U,
                    225U,
                    RED,
                    MENU_TEXT_NOT_IMPLEMENTED);
    Menu_ShowTextId(15U,
                    184U,
                    225U,
                    BLACK,
                    MENU_TEXT_RESERVED);
    Menu_DrawFooter(MENU_TEXT_FOOTER_BACK);
}

static void Menu_DrawBypassActive(void)
{
    LCD_Clear(WHITE);
    Menu_DrawHeader(MENU_TEXT_TITLE_BYPASS);
    Menu_ShowTextId(18U, 82U, 224U, GREEN, MENU_TEXT_RELAY_ON);
    Menu_ShowTextId(18U, 128U, 224U, BLACK, MENU_TEXT_PD2_HIGH);
    Menu_ShowTextId(18U, 168U, 224U, BLACK, MENU_TEXT_SIGNAL_TO_SCOPE);
    Menu_DrawFooter(MENU_TEXT_FOOTER_RELAY_OFF);
}

static void Menu_Redraw(void)
{
    switch (current_page)
    {
        case MENU_PAGE_HOME:
            Menu_DrawHome();
            break;

        case MENU_PAGE_MANUAL_SHAPE:
            Menu_DrawShape(0U);
            break;

        case MENU_PAGE_MANUAL_VOLTAGE:
            Menu_DrawVoltage();
            break;

        case MENU_PAGE_MANUAL_RUN:
            Menu_DrawManualRunFrame();
            break;

        case MENU_PAGE_AUTO_SHAPE:
            Menu_DrawShape(1U);
            break;

        case MENU_PAGE_AUTO_PLACEHOLDER:
            Menu_DrawAutoPlaceholder();
            break;

        case MENU_PAGE_BYPASS_ACTIVE:
            Menu_DrawBypassActive();
            break;

        default:
            current_page = MENU_PAGE_HOME;
            current_selection = 0U;
            Menu_DrawHome();
            break;
    }
}

void Menu_Init(void)
{
    current_page = MENU_PAGE_HOME;
    current_selection = 0U;
    selected_shape = APP_SHAPE_DIAGONAL;
    selected_voltage = APP_VOLTAGE_4V;
    runtime_field_cursor = 0U;
    runtime_character_cursor = 0U;
    runtime_model_countdown = 0U;
    Board_RelaySet(Bit_RESET);

    LCD_Init();
    LCD_Clear(WHITE);
    Menu_Redraw();
}

void Menu_HandleKey(Board_KeyEvent key)
{
    uint8_t option_count;
    uint8_t previous_selection;
    SignalCapture_Snapshot discarded_capture;

    if (key == BOARD_KEY_NONE)
        return;

    option_count = Menu_OptionCount();
    if ((key == BOARD_KEY_UP) && (option_count != 0U))
    {
        previous_selection = current_selection;
        if (current_selection == 0U)
            current_selection = (uint8_t)(option_count - 1U);
        else
            current_selection--;
        Menu_UpdateSelection(previous_selection);
        return;
    }

    if ((key == BOARD_KEY_DOWN) && (option_count != 0U))
    {
        previous_selection = current_selection;
        current_selection++;
        if (current_selection >= option_count)
            current_selection = 0U;
        Menu_UpdateSelection(previous_selection);
        return;
    }

    if (key == BOARD_KEY_BACK)
    {
        switch (current_page)
        {
            case MENU_PAGE_MANUAL_SHAPE:
            case MENU_PAGE_AUTO_SHAPE:
                current_page = MENU_PAGE_HOME;
                current_selection = 0U;
                break;

            case MENU_PAGE_MANUAL_VOLTAGE:
                current_page = MENU_PAGE_MANUAL_SHAPE;
                current_selection = (uint8_t)selected_shape;
                break;

            case MENU_PAGE_MANUAL_RUN:
                ManualControl_Stop();
                current_page = MENU_PAGE_MANUAL_VOLTAGE;
                current_selection = (uint8_t)selected_voltage;
                break;

            case MENU_PAGE_AUTO_PLACEHOLDER:
                AutoMode_Exit();
                current_page = MENU_PAGE_AUTO_SHAPE;
                current_selection = (uint8_t)selected_shape;
                break;

            case MENU_PAGE_BYPASS_ACTIVE:
                Board_RelaySet(Bit_RESET);
                current_page = MENU_PAGE_HOME;
                current_selection = 2U;
                break;

            default:
                break;
        }
        Menu_Redraw();
        return;
    }

    if (key != BOARD_KEY_CONFIRM)
        return;

    switch (current_page)
    {
        case MENU_PAGE_HOME:
            if (current_selection == 0U)
            {
                Board_RelaySet(Bit_RESET);
                current_page = MENU_PAGE_MANUAL_SHAPE;
            }
            else if (current_selection == 1U)
            {
                Board_RelaySet(Bit_RESET);
                current_page = MENU_PAGE_AUTO_SHAPE;
            }
            else
            {
                ManualControl_Stop();
                AutoMode_Exit();
                Board_RelaySet(Bit_SET);
                current_page = MENU_PAGE_BYPASS_ACTIVE;
            }
            current_selection = 0U;
            break;

        case MENU_PAGE_MANUAL_SHAPE:
            selected_shape = (App_Shape)current_selection;
            current_page = MENU_PAGE_MANUAL_VOLTAGE;
            current_selection = (uint8_t)selected_voltage;
            break;

        case MENU_PAGE_MANUAL_VOLTAGE:
            selected_voltage = (App_Voltage)current_selection;
            current_page = MENU_PAGE_MANUAL_RUN;
            current_selection = 0U;
            /*
             * Complete the slow, one-time LCD page construction before the
             * control loop starts.  Then consume any UART data accumulated
             * during that redraw and establish a fresh diagnostics baseline.
             */
            Menu_Redraw();
            SignalCapture_Service();
            while (SignalCapture_TakeSnapshot(&discarded_capture) != 0U)
            {
            }
            SignalCapture_ClearDiagnostics();
            ManualControl_Start(selected_shape, selected_voltage);
            return;

        case MENU_PAGE_AUTO_SHAPE:
            selected_shape = (App_Shape)current_selection;
            AutoMode_Enter(selected_shape);
            current_page = MENU_PAGE_AUTO_PLACEHOLDER;
            current_selection = 0U;
            break;

        default:
            return;
    }

    Menu_Redraw();
}

static void Menu_FormatFrequency(char *buffer,
                                 uint32_t frequency_millihz)
{
    sprintf(buffer,
            "%lu.%03lu Hz",
            (unsigned long)(frequency_millihz / 1000UL),
            (unsigned long)(frequency_millihz % 1000UL));
}

static void Menu_FormatPhase(char *buffer,
                             int32_t phase_millidegrees)
{
    uint32_t magnitude;
    char sign;

    if (phase_millidegrees < 0L)
    {
        sign = '-';
        magnitude = (uint32_t)(-phase_millidegrees);
    }
    else
    {
        sign = '+';
        magnitude = (uint32_t)phase_millidegrees;
    }

    sprintf(buffer,
            "%c%lu.%03lu deg",
            sign,
            (unsigned long)(magnitude / 1000UL),
            (unsigned long)(magnitude % 1000UL));
}

static uint16_t Menu_DrawRuntimeLabel(uint16_t y,
                                      MenuTextId label,
                                      uint16_t color)
{
    const uint8_t *text;

    LCD_Fill(0U, y, 239U, (uint16_t)(y + 18U), WHITE);
    text = MenuChinese_GetText(label);
    MenuChinese_DrawText(6U, y, 236U, color, text);
    return (uint16_t)(8U + MenuChinese_TextWidth(text));
}

static uint16_t Menu_DiagnosticDisplayCount(uint32_t value)
{
    return (value > 9999UL) ? 9999U : (uint16_t)value;
}

static const char *Menu_ControlStateName(ManualControl_State state)
{
    if (state == MANUAL_CONTROL_HOLD_REFERENCE)
        return "HOLD";
    if (state == MANUAL_CONTROL_OPEN_LOOP)
        return "OPEN";
    if (state == MANUAL_CONTROL_CLOSED_LOOP)
        return "CLOSED";
    return "STOP";
}

static void Menu_RuntimeInitField(uint8_t field_index,
                                  uint16_t x,
                                  uint16_t y,
                                  uint8_t size,
                                  uint16_t color)
{
    MenuRuntimeField *field;
    uint16_t character_width;
    uint16_t capacity;
    uint8_t index;

    if (field_index >= MENU_RUNTIME_FIELD_COUNT)
        return;
    field = &runtime_fields[field_index];
    character_width = (uint16_t)(size / 2U);
    capacity = (character_width == 0U) ? 0U :
               (uint16_t)((236U - x) / character_width);
    if (capacity >= MENU_RUNTIME_TEXT_CAPACITY)
        capacity = MENU_RUNTIME_TEXT_CAPACITY - 1U;
    field->x = x;
    field->y = y;
    field->color = color;
    field->size = size;
    field->max_chars = (uint8_t)capacity;
    for (index = 0U; index < MENU_RUNTIME_TEXT_CAPACITY; index++)
    {
        field->target[index] = ' ';
        field->drawn[index] = ' ';
    }
    field->target[field->max_chars] = '\0';
    field->drawn[field->max_chars] = '\0';
}

static void Menu_RuntimeInitLabelField(uint8_t field_index,
                                       uint16_t y,
                                       MenuTextId label)
{
    uint16_t x;

    x = Menu_DrawRuntimeLabel(y, label, BLACK);
    Menu_RuntimeInitField(field_index,
                          x,
                          (uint16_t)(y + 2U),
                          12U,
                          BLACK);
}

static void Menu_RuntimePrepareLayout(void)
{
    char line[24];
    uint16_t x;

    LCD_Fill(0U, 48U, 239U, 66U, WHITE);
    x = MenuChinese_DrawText(
        6U, 49U, 236U, BLUE,
        MenuChinese_GetText(Menu_ShapeText(selected_shape)));
    sprintf(line, " / %s", ManualControl_VoltageName(selected_voltage));
    Menu_ShowAscii((uint16_t)(x + 4U), 49U,
                   236U, 16U, BLUE, line);

    Menu_RuntimeInitLabelField(0U, 68U, MENU_TEXT_LABEL_OUTPUT);
    Menu_RuntimeInitLabelField(1U, 88U, MENU_TEXT_LABEL_STATE);
    Menu_RuntimeInitLabelField(2U, 108U, MENU_TEXT_LABEL_RAW);
    Menu_RuntimeInitLabelField(3U, 128U, MENU_TEXT_LABEL_USED);
    Menu_RuntimeInitLabelField(4U, 148U, MENU_TEXT_LABEL_TARGET);
    Menu_RuntimeInitLabelField(5U, 168U, MENU_TEXT_LABEL_COMMAND);
    Menu_RuntimeInitLabelField(6U, 188U, MENU_TEXT_LABEL_FEEDBACK);
    Menu_RuntimeInitLabelField(7U, 208U, MENU_TEXT_LABEL_PHASE);
    Menu_RuntimeInitLabelField(8U, 228U, MENU_TEXT_LABEL_CORRECTION);
    Menu_RuntimeInitLabelField(9U, 248U, MENU_TEXT_LABEL_ERROR);
    LCD_Fill(0U, 268U, 239U, 285U, WHITE);
    Menu_RuntimeInitField(10U, 2U, 270U, 12U, GREEN);
    runtime_field_cursor = 0U;
    runtime_character_cursor = 0U;
    runtime_model_countdown = 0U;
}

static void Menu_RuntimeSetText(uint8_t field_index,
                                const char *text,
                                uint16_t color)
{
    MenuRuntimeField *field;
    uint8_t index;
    char value;

    if ((field_index >= MENU_RUNTIME_FIELD_COUNT) || (text == 0))
        return;
    field = &runtime_fields[field_index];
    if (field->color != color)
    {
        field->color = color;
        for (index = 0U; index < field->max_chars; index++)
            field->drawn[index] = 0x7F;
    }
    for (index = 0U; index < field->max_chars; index++)
    {
        value = text[index];
        if (value == '\0')
        {
            while (index < field->max_chars)
                field->target[index++] = ' ';
            break;
        }
        field->target[index] = value;
    }
    field->target[field->max_chars] = '\0';
}

static void Menu_RuntimeRenderOneCharacter(void)
{
    MenuRuntimeField *field;
    uint16_t x;
    uint16_t character_width;
    uint16_t scan_count;
    uint16_t scan_limit;
    uint8_t character_index;
    char character;

    scan_limit = MENU_RUNTIME_FIELD_COUNT *
                 MENU_RUNTIME_TEXT_CAPACITY;
    for (scan_count = 0U; scan_count < scan_limit; scan_count++)
    {
        field = &runtime_fields[runtime_field_cursor];
        if (runtime_character_cursor >= field->max_chars)
        {
            runtime_character_cursor = 0U;
            runtime_field_cursor++;
            if (runtime_field_cursor >= MENU_RUNTIME_FIELD_COUNT)
                runtime_field_cursor = 0U;
            continue;
        }
        character_index = runtime_character_cursor++;
        if (field->target[character_index] ==
            field->drawn[character_index])
            continue;

        character_width = (uint16_t)(field->size / 2U);
        x = (uint16_t)(field->x +
                       ((uint16_t)character_index * character_width));
        LCD_Fill(x,
                 field->y,
                 (uint16_t)(x + character_width),
                 (uint16_t)(field->y + field->size),
                 WHITE);
        character = field->target[character_index];
        if ((character >= ' ') && (character <= '~') &&
            (character != ' '))
        {
            LCD_ShowChar(x,
                         field->y,
                         (u8)character,
                         field->size,
                         field->color,
                         1U);
        }
        field->drawn[character_index] = character;
        return;
    }
}

static void Menu_RuntimeUpdateModel(const SignalCapture_Snapshot *capture)
{
    ManualControl_Status status;
    SignalCapture_Diagnostics diagnostics;
    char line[48];
    uint16_t state_color;
    uint16_t diagnostic_color;
    int32_t displayed_error_millihz;

    ManualControl_GetStatus(&status);
    sprintf(line, "%u.%u mVpp ASF=%u",
            (unsigned int)(status.nominal_vpp_mv_x10 / 10U),
            (unsigned int)(status.nominal_vpp_mv_x10 % 10U),
            (unsigned int)status.asf);
    Menu_RuntimeSetText(0U, line, BLACK);

    if ((status.state == MANUAL_CONTROL_CLOSED_LOOP) &&
        (status.phase_locked != 0U))
        state_color = GREEN;
    else if (status.state != MANUAL_CONTROL_STOPPED)
        state_color = BLUE;
    else
        state_color = RED;
    sprintf(line, "%s L%u P%u",
            Menu_ControlStateName(status.state),
            (unsigned int)status.loop_mode,
            (unsigned int)status.phase_locked);
    Menu_RuntimeSetText(1U, line, state_color);

    Menu_FormatFrequency(line, capture->reference_raw_frequency_millihz);
    Menu_RuntimeSetText(2U, line, BLACK);
    Menu_FormatFrequency(line, capture->reference_frequency_millihz);
    Menu_RuntimeSetText(3U, line, BLACK);
    Menu_FormatFrequency(line, status.target_frequency_millihz);
    Menu_RuntimeSetText(4U, line, BLACK);
    Menu_FormatFrequency(line, status.command_frequency_millihz);
    Menu_RuntimeSetText(5U, line, BLACK);
    if (status.filtered_feedback_frequency_millihz != 0UL)
        Menu_FormatFrequency(
            line, status.filtered_feedback_frequency_millihz);
    else
        Menu_FormatFrequency(line, capture->feedback_frequency_millihz);
    Menu_RuntimeSetText(6U, line, BLACK);
    Menu_FormatPhase(line, status.relative_phase_millidegrees);
    Menu_RuntimeSetText(7U, line, BLACK);
    sprintf(line, "%ld mHz",
            (long)status.frequency_correction_millihz);
    Menu_RuntimeSetText(8U, line, BLACK);
    displayed_error_millihz =
        (status.loop_mode == MANUAL_LOOP_LOCKED) ?
        status.phase_slip_error_millihz :
        status.frequency_error_millihz;
    sprintf(line, "%ld mHz", (long)displayed_error_millihz);
    Menu_RuntimeSetText(9U, line, BLACK);

    SignalCapture_GetDiagnostics(&diagnostics);
    sprintf(line,
            "A%u H%u V%u%u%u R%u C%u S%u O%u Q%u X%u%u E%u G%u",
            (unsigned int)status.pll_locked,
            (unsigned int)capture->phase_harmonic,
            (unsigned int)capture->reference_valid,
            (unsigned int)capture->feedback_valid,
            (unsigned int)capture->relative_phase_valid,
            (unsigned int)capture->clip_recovery,
            (unsigned int)Menu_DiagnosticDisplayCount(
                diagnostics.crc_errors),
            (unsigned int)Menu_DiagnosticDisplayCount(
                diagnostics.missing_frames),
            (unsigned int)Menu_DiagnosticDisplayCount(
                diagnostics.uart_dma_overflow_bytes),
            (unsigned int)Menu_DiagnosticDisplayCount(
                diagnostics.queue_overflows),
            (unsigned int)capture->reference_clipped,
            (unsigned int)capture->feedback_clipped,
            (unsigned int)Menu_DiagnosticDisplayCount(
                status.feedback_outlier_frames),
            (unsigned int)Menu_DiagnosticDisplayCount(
                status.gross_relock_count));
    diagnostic_color =
        ((diagnostics.crc_errors == 0UL) &&
         (diagnostics.missing_frames == 0UL) &&
         (diagnostics.uart_dma_overflow_bytes == 0UL) &&
         (diagnostics.queue_overflows == 0UL) &&
         (capture->dma_overrun == 0U) &&
         (capture->reference_valid != 0U) &&
         (capture->feedback_valid != 0U) &&
         (capture->relative_phase_valid != 0U) &&
         (capture->reference_clipped == 0U) &&
         (capture->feedback_clipped == 0U) &&
         (capture->clip_recovery == 0U)) ? GREEN : RED;
    Menu_RuntimeSetText(10U, line, diagnostic_color);
}

#if 0

void Menu_RefreshRuntime(const SignalCapture_Snapshot *capture)
{
    ManualControl_Status status;
    SignalCapture_Diagnostics diagnostics;
    char line[48];
    uint16_t x;
    uint16_t state_color;
    int32_t displayed_error_millihz;
    MenuTextId state_text;

    if ((current_page != MENU_PAGE_MANUAL_RUN) || (capture == 0))
        return;

    ManualControl_GetStatus(&status);
    switch (runtime_row)
    {
        case 0U:
            LCD_Fill(0U, 48U, 239U, 66U, WHITE);
            x = MenuChinese_DrawText(
                6U, 49U, 236U, BLUE,
                MenuChinese_GetText(Menu_ShapeText(status.shape)));
            sprintf(line, " / %s",
                    ManualControl_VoltageName(status.voltage));
            Menu_ShowAscii((uint16_t)(x + 4U), 49U,
                           236U, 16U, BLUE, line);
            break;

        case 1U:
            sprintf(line, "%u.%u mVpp ASF=%u",
                    (unsigned int)(status.nominal_vpp_mv_x10 / 10U),
                    (unsigned int)(status.nominal_vpp_mv_x10 % 10U),
                    (unsigned int)status.asf);
            Menu_DrawRuntimeAscii(68U, MENU_TEXT_LABEL_OUTPUT,
                                  BLACK, line);
            break;

        case 2U:
            if ((status.state == MANUAL_CONTROL_CLOSED_LOOP) &&
                (status.phase_locked != 0U))
                state_color = GREEN;
            else if (status.state != MANUAL_CONTROL_STOPPED)
                state_color = BLUE;
            else
                state_color = RED;
            x = Menu_DrawRuntimeLabel(88U, MENU_TEXT_LABEL_STATE,
                                      state_color);
            state_text = Menu_ControlStateText(status.state);
            x = MenuChinese_DrawText(
                x, 88U, 176U, state_color,
                MenuChinese_GetText(state_text));
            sprintf(line, " L%u P%u",
                    (unsigned int)status.loop_mode,
                    (unsigned int)status.phase_locked);
            Menu_ShowAscii((uint16_t)(x + 2U), 90U,
                           236U, 12U, state_color, line);
            break;

        case 3U:
            Menu_FormatFrequency(
                line, capture->reference_raw_frequency_millihz);
            Menu_DrawRuntimeAscii(108U, MENU_TEXT_LABEL_RAW,
                                  BLACK, line);
            break;

        case 4U:
            Menu_FormatFrequency(
                line, capture->reference_frequency_millihz);
            Menu_DrawRuntimeAscii(128U, MENU_TEXT_LABEL_USED,
                                  BLACK, line);
            break;

        case 5U:
            Menu_FormatFrequency(line, status.target_frequency_millihz);
            Menu_DrawRuntimeAscii(148U, MENU_TEXT_LABEL_TARGET,
                                  BLACK, line);
            break;

        case 6U:
            Menu_FormatFrequency(line, status.command_frequency_millihz);
            Menu_DrawRuntimeAscii(168U, MENU_TEXT_LABEL_COMMAND,
                                  BLACK, line);
            break;

        case 7U:
            if (status.filtered_feedback_frequency_millihz != 0UL)
                Menu_FormatFrequency(
                    line, status.filtered_feedback_frequency_millihz);
            else
                Menu_FormatFrequency(
                    line, capture->feedback_frequency_millihz);
            Menu_DrawRuntimeAscii(188U, MENU_TEXT_LABEL_FEEDBACK,
                                  BLACK, line);
            break;

        case 8U:
            Menu_FormatPhase(line, status.relative_phase_millidegrees);
            Menu_DrawRuntimeAscii(208U, MENU_TEXT_LABEL_PHASE,
                                  BLACK, line);
            break;

        case 9U:
            sprintf(line, "%ld mHz",
                    (long)status.frequency_correction_millihz);
            Menu_DrawRuntimeAscii(228U, MENU_TEXT_LABEL_CORRECTION,
                                  BLACK, line);
            break;

        case 10U:
            displayed_error_millihz =
                (status.loop_mode == MANUAL_LOOP_LOCKED) ?
                status.phase_slip_error_millihz :
                status.frequency_error_millihz;
            sprintf(line, "%ld mHz", (long)displayed_error_millihz);
            Menu_DrawRuntimeAscii(248U, MENU_TEXT_LABEL_ERROR,
                                  BLACK, line);
            break;

        default:
            SignalCapture_GetDiagnostics(&diagnostics);
            LCD_Fill(0U, 268U, 239U, 285U, WHITE);
            sprintf(line, "A%u H%u C%u S%u O%u Q%u X%u%u",
                    (unsigned int)status.pll_locked,
                    (unsigned int)capture->phase_harmonic,
                    (unsigned int)Menu_DiagnosticDisplayCount(
                        diagnostics.crc_errors),
                    (unsigned int)Menu_DiagnosticDisplayCount(
                        diagnostics.missing_frames),
                    (unsigned int)Menu_DiagnosticDisplayCount(
                        diagnostics.uart_dma_overflow_bytes),
                    (unsigned int)Menu_DiagnosticDisplayCount(
                        diagnostics.queue_overflows),
                    (unsigned int)capture->reference_clipped,
                    (unsigned int)capture->feedback_clipped);
            Menu_ShowAscii(2U, 270U, 238U, 12U,
                           ((diagnostics.crc_errors == 0UL) &&
                            (diagnostics.missing_frames == 0UL) &&
                            (diagnostics.uart_dma_overflow_bytes == 0UL) &&
                            (diagnostics.queue_overflows == 0UL) &&
                            (capture->dma_overrun == 0U) &&
                            (capture->reference_clipped == 0U) &&
                            (capture->feedback_clipped == 0U)) ?
                           GREEN : RED,
                           line);
            break;
    }

    runtime_row++;
    if (runtime_row >= 12U)
        runtime_row = 0U;
}
#endif

void Menu_RefreshRuntime(const SignalCapture_Snapshot *capture)
{
    if ((current_page != MENU_PAGE_MANUAL_RUN) || (capture == 0))
        return;

    if (runtime_model_countdown == 0U)
    {
        Menu_RuntimeUpdateModel(capture);
        runtime_model_countdown = MENU_RUNTIME_MODEL_SLICES;
    }
    else
    {
        runtime_model_countdown--;
    }
    Menu_RuntimeRenderOneCharacter();
}
