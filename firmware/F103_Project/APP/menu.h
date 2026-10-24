#ifndef __MENU_H
#define __MENU_H

#include "board.h"
#include "signal_capture.h"

void Menu_Init(void);
void Menu_HandleKey(Board_KeyEvent key);
void Menu_RefreshRuntime(const SignalCapture_Snapshot *capture);

#endif
