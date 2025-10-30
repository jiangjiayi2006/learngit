#ifndef __MOTOR_H
#define __MOTOR_H

#include "common.h"

void Motor_Init(void);
void Motor_SetSpeed(uint8_t motor_id, int16_t speed);
void Motor_Stop(uint8_t motor_id);
void Motor_Brake(uint8_t motor_id);

#endif