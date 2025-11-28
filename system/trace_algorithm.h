#ifndef __TRACE_ALGORITHM_H
#define __TRACE_ALGORITHM_H

#include "stm32f10x.h"

typedef enum {
    PATH_STRAIGHT,
    PATH_CURVE, 
    PATH_CROSS,
    PATH_LOST
} PathType;

// PID状态变量
extern volatile int16_t last_error;
extern volatile int16_t integral;

void Trace_Algorithm(void);
void Simple_Trace_Algorithm(void);  // 添加简化算法
int16_t PID_Calculate(int16_t error);
PathType GetCurrentPathType(void);
uint8_t GetCrossCount(void);
void ResetCrossCount(void);
void SetPIDParams(float kp, float ki, float kd);
void ResetPID(void);

#endif