#ifndef __AUTO_MODE_H
#define __AUTO_MODE_H

#include "app_types.h"
#include "signal_capture.h"

void AutoMode_Init(void);
void AutoMode_Enter(App_Shape shape);
void AutoMode_Service(const SignalCapture_Snapshot *capture);
void AutoMode_Exit(void);
uint8_t AutoMode_IsActive(void);
App_Shape AutoMode_GetShape(void);

#endif
