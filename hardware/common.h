#ifndef __COMMON_H
#define __COMMON_H

#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 电机ID定义
#define MOTOR1 0
#define MOTOR2 1

// 函数声明
void delay_ms(uint32_t nms);

#endif