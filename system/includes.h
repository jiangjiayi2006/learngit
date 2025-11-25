#ifndef __INCLUDES_H
#define __INCLUDES_H

#include "stm32f10x.h"
#include "motor.h"
#include "oled.h"
#include "trace_sensor.h"
#include "trace_algorithm.h"
#include "system_manager.h"
#include "key.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 通用定义
#define ENABLE  1
#define DISABLE 0
#define ON      1
#define OFF     0

// 函数声明
void SystemClock_Config(void);
void Delay_ms(uint32_t ms);
uint32_t GetSystemTick(void);

#endif