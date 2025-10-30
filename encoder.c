#include "encoder.h"

// 移除未使用的变量
static int32_t total_count[2] = {0, 0};
static int16_t motor_speed[2] = {0, 0};

void Encoder_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4, ENABLE);
    
    // 电机1编码器 PA6, PA7 - TIM3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 电机2编码器 PB6, PB7 - TIM4
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 定时器3配置（电机1）
    TIM_TimeBaseStructure.TIM_Period = 65535;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    
    // 编码器接口模式
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    
    // 定时器4配置（电机2）
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_ICInit(TIM4, &TIM_ICInitStructure);
    
    // 使能定时器
    TIM_Cmd(TIM3, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
}

int32_t Encoder_GetCount(uint8_t motor_id)
{
    int32_t count;
    
    if (motor_id == MOTOR1) {
        count = (int32_t)TIM_GetCounter(TIM3);
        TIM_SetCounter(TIM3, 0);
    } else {
        count = (int32_t)TIM_GetCounter(TIM4);
        TIM_SetCounter(TIM4, 0);
    }
    
    total_count[motor_id] += count;
    
    return count;
}

int16_t Encoder_GetSpeed(uint8_t motor_id)
{
    return motor_speed[motor_id];
}

void Encoder_ClearCount(uint8_t motor_id)
{
    if (motor_id == MOTOR1) {
        TIM_SetCounter(TIM3, 0);
    } else {
        TIM_SetCounter(TIM4, 0);
    }
    total_count[motor_id] = 0;
}

int32_t Encoder_GetTotalCount(uint8_t motor_id)
{
    return total_count[motor_id];
}

// 在10ms中断中调用此函数计算速度
void Encoder_UpdateSpeed(void)
{
    for (int i = 0; i < 2; i++) {
        int32_t current_count = Encoder_GetCount(i);
        // 转换为速度值（脉冲/10ms）
        motor_speed[i] = (int16_t)current_count;
    }
}