#include "auto_mode.h"

static uint8_t auto_active;
static App_Shape auto_shape;

void AutoMode_Init(void)
{
    auto_active = 0U;
    auto_shape = APP_SHAPE_DIAGONAL;
}

void AutoMode_Enter(App_Shape shape)
{
    if (shape >= APP_SHAPE_COUNT)
        shape = APP_SHAPE_DIAGONAL;
    auto_shape = shape;
    auto_active = 1U;

    /*
     * Expansion point intentionally left empty.
     * A future algorithm may inspect the capture snapshot in
     * AutoMode_Service() and issue its own requested shape/voltage command.
     */
}

void AutoMode_Service(const SignalCapture_Snapshot *capture)
{
    (void)capture;
}

void AutoMode_Exit(void)
{
    auto_active = 0U;
}

uint8_t AutoMode_IsActive(void)
{
    return auto_active;
}

App_Shape AutoMode_GetShape(void)
{
    return auto_shape;
}
