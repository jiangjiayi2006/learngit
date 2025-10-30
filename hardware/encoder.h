#ifndef __ENCODER_H
#define __ENCODER_H

#include "common.h"

void Encoder_Init(void);
int32_t Encoder_GetCount(uint8_t motor_id);
int16_t Encoder_GetSpeed(uint8_t motor_id);
void Encoder_ClearCount(uint8_t motor_id);
int32_t Encoder_GetTotalCount(uint8_t motor_id);
void Encoder_UpdateSpeed(void);

#endif