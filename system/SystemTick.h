#ifndef __SYSTEMTICK_H
#define __SYSTEMTICK_H

#include <stdint.h>
#include "stm32f10x.h"  // 添加STM32头文件

extern volatile uint32_t g_system_tick;

void SystemTick_Init(void);
uint32_t Get_SystemTick(void);

#endif