#include "watchdog.h"
#include "stm32f10x.h"

void Watchdog_Init(void) {
    // 开启独立看门狗时钟
    RCC_LSICmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
    
    // 设置看门狗预分频器和重装载值
    // 时钟约40kHz，4分频后10kHz，4095计数约0.4秒
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_4);
    IWDG_SetReload(4095);
    
    // 重载看门狗并启用
    IWDG_ReloadCounter();
    IWDG_Enable();
}

void Watchdog_Feed(void) {
    IWDG_ReloadCounter();
}