#ifndef __WATCHDOG_H
#define __WATCHDOG_H

#include "stm32f10x.h"

void Watchdog_Init(void);
void Watchdog_Feed(void);

#endif
/* 确保最后有换行符 */