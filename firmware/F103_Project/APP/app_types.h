#ifndef __APP_TYPES_H
#define __APP_TYPES_H

#include "stm32f10x.h"

typedef enum
{
    APP_SHAPE_DIAGONAL = 0,
    APP_SHAPE_CIRCLE,
    APP_SHAPE_INFINITY,
    APP_SHAPE_COUNT
} App_Shape;

typedef enum
{
    APP_VOLTAGE_4V = 0,
    APP_VOLTAGE_3V,
    APP_VOLTAGE_2V,
    APP_VOLTAGE_1V,
    APP_VOLTAGE_COUNT
} App_Voltage;

#endif
