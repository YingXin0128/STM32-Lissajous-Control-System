#include "board.h"

typedef struct
{
    uint8_t last_sample;
    uint8_t stable_level;
    uint8_t stable_count;
} Board_KeyState;

static Board_KeyState key_up;
static Board_KeyState key_down;
static Board_KeyState key_back;
static Board_KeyState key_confirm;
static uint8_t confirm_idle_level = 1U;

static void Board_KeyStateInit(Board_KeyState *state, uint8_t level)
{
    state->last_sample = level;
    state->stable_level = level;
    state->stable_count = 0U;
}

static uint8_t Board_KeyPressed(uint16_t pin,
                                uint8_t pressed_level,
                                Board_KeyState *state)
{
    uint8_t sample;

    /*
     * This function must be called every 1 ms.
     * A level must remain unchanged for 20 calls before it is accepted.
     * Only the debounced high-to-low transition generates an event.
     */
    sample = (uint8_t)GPIO_ReadInputDataBit(GPIOE, pin);

    if (sample == state->last_sample)
    {
        if (state->stable_count < 20U)
            state->stable_count++;
    }
    else
    {
        state->last_sample = sample;
        state->stable_count = 0U;
    }

    if ((state->stable_count >= 20U) &&
        (state->stable_level != state->last_sample))
    {
        state->stable_level = state->last_sample;
        if (state->stable_level == pressed_level)
            return 1U;
    }

    return 0U;
}

void Board_Init(void)
{
    GPIO_InitTypeDef gpio;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |
                           RCC_APB2Periph_GPIOD |
                           RCC_APB2Periph_GPIOE, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_13;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &gpio);

    /* PC13 is the onboard active-low LED and the oscilloscope marker output. */
    GPIO_SetBits(GPIOC, GPIO_Pin_13);

    /*
     * PD2 controls the external bypass relay.
     * Active high: high=bypass/direct path, low=normal signal chain.
     * Program the output latch low before enabling push-pull output mode so
     * reset and initialization cannot briefly energize the relay.
     */
    GPIO_ResetBits(GPIOD, GPIO_Pin_2);
    gpio.GPIO_Pin = GPIO_Pin_2;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &gpio);

    /*
     * PE13/PE14/PE15 are active-low navigation keys.
     * PE12 is the external three-pin key module signal.  The firmware samples
     * its released level at boot, so both active-high and active-low modules
     * are accepted.  The key must be released while powering up.
     */
    gpio.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 |
                    GPIO_Pin_14 | GPIO_Pin_15;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOE, &gpio);

    Board_KeyStateInit(&key_up,
                       (uint8_t)GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_13));
    Board_KeyStateInit(&key_down,
                       (uint8_t)GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_14));
    Board_KeyStateInit(&key_back,
                       (uint8_t)GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_15));
    Board_CalibrateConfirmIdle();
}

void Board_CalibrateConfirmIdle(void)
{
    confirm_idle_level =
        (uint8_t)GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_12);
    Board_KeyStateInit(&key_confirm, confirm_idle_level);
}

Board_KeyEvent Board_KeysPoll1ms(void)
{
    if (Board_KeyPressed(GPIO_Pin_12,
                         (uint8_t)(confirm_idle_level ^ 1U),
                         &key_confirm) != 0U)
        return BOARD_KEY_CONFIRM;

    if (Board_KeyPressed(GPIO_Pin_15, 0U, &key_back) != 0U)
        return BOARD_KEY_BACK;

    if (Board_KeyPressed(GPIO_Pin_13, 0U, &key_up) != 0U)
        return BOARD_KEY_UP;

    if (Board_KeyPressed(GPIO_Pin_14, 0U, &key_down) != 0U)
        return BOARD_KEY_DOWN;

    return BOARD_KEY_NONE;
}

uint8_t Board_GetConfirmIdleLevel(void)
{
    return confirm_idle_level;
}

void Board_MarkerSet(BitAction level)
{
    GPIO_WriteBit(GPIOC, GPIO_Pin_13, level);
}

void Board_MarkerToggle(void)
{
    if ((GPIOC->ODR & GPIO_Pin_13) != 0U)
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
    else
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

void Board_RelaySet(BitAction level)
{
    GPIO_WriteBit(GPIOD, GPIO_Pin_2, level);
}

uint8_t Board_RelayIsOn(void)
{
    return ((GPIOD->ODR & GPIO_Pin_2) != 0U) ? 1U : 0U;
}
