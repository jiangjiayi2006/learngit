#include "SystemTick.h"
#include "stm32f10x.h"  // 确保包含STM32头文件
#include "stm32f10x_tim.h"  // 添加定时器头文件
#include "stm32f10x_rcc.h"  // 添加时钟头文件
#include "misc.h"  // 添加NVIC头文件

volatile uint32_t g_system_tick = 0;

void SystemTick_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // TIM2配置为1ms中断
    TIM_TimeBaseStructure.TIM_Period = 1000 - 1;  // 自动重装载值
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1; // 72MHz/72 = 1MHz, 1MHz/1000 = 1kHz
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM2, ENABLE);
}

uint32_t Get_SystemTick(void)
{
    return g_system_tick;
}

// TIM2中断处理函数
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        g_system_tick++;
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}