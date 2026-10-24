#ifndef __BOARD_H
#define __BOARD_H

#include "stm32f10x.h"

typedef enum
{
    BOARD_KEY_NONE = 0,
    BOARD_KEY_UP,
    BOARD_KEY_DOWN,
    BOARD_KEY_BACK,
    BOARD_KEY_CONFIRM
} Board_KeyEvent;

void Board_Init(void);
void Board_CalibrateConfirmIdle(void);
Board_KeyEvent Board_KeysPoll1ms(void);
uint8_t Board_GetConfirmIdleLevel(void);
void Board_MarkerSet(BitAction level);
void Board_MarkerToggle(void);
void Board_RelaySet(BitAction level);
uint8_t Board_RelayIsOn(void);

#endif
