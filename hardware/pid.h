#ifndef __PID_H
#define __PID_H

#include "common.h"

typedef struct {
    float target_val;     // 目标值
    float actual_val;     // 实际值
    float err;           // 当前误差
    float err_last;      // 上次误差
    float err_prev;      // 上上次误差
    float Kp, Ki, Kd;    // PID参数
    float integral;      // 积分项
    float output;        // 输出
    float output_max;    // 输出限幅
    float integral_max;  // 积分限幅
} PID_TypeDef;

void PID_Init(void);
void PID_Reset(void);
float PID_Calculate(PID_TypeDef *pid, float target, float actual);
void PID_SetParams(uint8_t motor_id, float kp, float ki, float kd);
void PID_Update(void);
void PID_SetTargetSpeed(uint8_t motor_id, float speed);
float PID_GetTargetSpeed(uint8_t motor_id);

extern PID_TypeDef speed_pid[2];
extern PID_TypeDef position_pid[2];

#endif