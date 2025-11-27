#ifndef __SYSTEM_MANAGER_H
#define __SYSTEM_MANAGER_H

#include "stm32f10x.h"

// 系统状态
typedef enum {
    STATE_WAIT_START,
    STATE_TRACING
} SystemState;

// 函数声明
void Hardware_Init(void);
void Tracing_Handler(void);
SystemState GetSystemState(void);
void SetSystemState(SystemState new_state);

#endif