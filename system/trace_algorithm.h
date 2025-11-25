#ifndef __TRACE_ALGORITHM_H
#define __TRACE_ALGORITHM_H

#include "stm32f10x.h"

typedef enum {
    PATH_STRAIGHT,
    PATH_CURVE, 
    PATH_CROSS,
    PATH_LOST
} PathType;

void Trace_Algorithm(void);
int16_t PID_Calculate(int16_t error);
PathType GetCurrentPathType(void);
uint8_t GetCrossCount(void);
void ResetCrossCount(void);
void SetPIDParams(float kp, float ki, float kd);

#endif
