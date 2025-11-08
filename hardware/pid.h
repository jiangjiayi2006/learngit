#ifndef __PID_H
#define __PID_H

#include <stdint.h>

// PID控制器结构体
typedef struct {
    float target;
    float actual;
    float output;
    float kp, ki, kd;
    float error[3]; // error0, error1, error2
    float integral_limit;
    float output_limit;
} PID_Controller;

// 参数限制函数
float Constrain_Float(float value, float min_val, float max_val);

// PID控制函数
void PIDControl(void);

// PID参数初始化
void PID_Init(void);

#endif