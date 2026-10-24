#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include "measurement.h"

void Protocol_Init(void);
void Protocol_SendMeasurement(const Measurement_Result *result);
void Protocol_DmaIrqHandler(void);

#endif
