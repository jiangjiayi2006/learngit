#ifndef __SYSTEM_MANAGER_H
#define __SYSTEM_MANAGER_H

#include "stm32f10x.h"

typedef enum {
    STATE_MENU,
    STATE_TRACING,
    STATE_SETTINGS,
    STATE_CALIBRATION
} SystemState;

void System_Init(void);
void System_HandleMenu(void);
void System_HandleTracing(void);
SystemState GetSystemState(void);

// 添加缺失的函数声明
void System_ExecuteSelection(uint8_t selection);
void System_ShowSettings(void);
void System_CalibrateSensors(void);

#endif