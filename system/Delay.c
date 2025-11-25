#include "delay.h"

// 使用TIM4作为系统滴答计数器（TIM4通常较少被使用）
volatile uint32_t system_tick = 0;

void Delay_Init(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // 开启TIM4时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    
    // 配置TIM4为1ms中断
    // 72MHz / (72 * 1000) = 1ms
    TIM_TimeBaseStructure.TIM_Period = 1000 - 1;     // 自动重装载值
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;    // 预分频器
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    
    // 启用TIM4更新中断
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    
    // 配置TIM4中断
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // 启动TIM4
    TIM_Cmd(TIM4, ENABLE);
}

// TIM4中断处理函数
void TIM4_IRQHandler(void) {
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
        system_tick++;
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }
}

void Delay_ms(uint32_t ms) {
    uint32_t start_tick = GetSystemTick();
    while ((GetSystemTick() - start_tick) < ms) {
        // 空循环等待
    }
}

uint32_t GetSystemTick(void) {
    return system_tick;
}