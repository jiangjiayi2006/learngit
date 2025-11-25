#ifndef __TRACE_SENSOR_H
#define __TRACE_SENSOR_H

#include "stm32f10x.h"

void TraceSensor_Init(void);
uint8_t TraceSensor_Read(void);
uint16_t TraceSensor_ReadSingle(uint8_t channel);
void TraceSensor_Calibrate(void);

#endif
// Ìí¼Ó¿ÕÐÐ