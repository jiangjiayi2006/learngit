#ifndef __SERIAL_H
#define __SERIAL_H

#include "common.h"

void Serial_Init(void);
void Serial_SendString(char *str);
void Serial_SendData(float *data, uint8_t len);

#endif