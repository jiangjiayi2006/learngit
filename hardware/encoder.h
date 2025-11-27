#ifndef __ENCODER_H
#define __ENCODER_H

#include <stdint.h>

void Encoder_DeInit(void);
void Encoder_Left_Init(void);
void Encoder_Right_Init(void);
int16_t Encoder_Left_Get(void);
int16_t Encoder_Right_Get(void);

#endif