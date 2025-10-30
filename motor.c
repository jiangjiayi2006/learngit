#include "motor.h"

void Motor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    // 电机控制引脚初始化
    // AIN1: PB12, AIN2: PB13, PWMA: PA2
    // BIN1: PB14, BIN2: PB15, PWMB: PA3
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 定时器2配置 - PWM输出
    TIM_TimeBaseStructure.TIM_Period = 999;  // 1kHz PWM
    TIM_TimeBaseStructure.TIM_Prescaler = 71; // 72MHz/(71+1) = 1MHz, 1MHz/1000 = 1kHz
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    // PWM通道配置
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_Pulse = 0;
    
    TIM_OC3Init(TIM2, &TIM_OCInitStructure); // PA2 - TIM2_CH3
    TIM_OC4Init(TIM2, &TIM_OCInitStructure); // PA3 - TIM2_CH4
    
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    
    // 启动定时器
    TIM_Cmd(TIM2, ENABLE);
    
    // 初始状态：停止
    Motor_Stop(MOTOR1);
    Motor_Stop(MOTOR2);
}

void Motor_SetSpeed(uint8_t motor_id, int16_t speed)
{
    // 限制PWM范围
    if (speed > 1000) speed = 1000;
    if (speed < -1000) speed = -1000;
    
    if (motor_id == MOTOR1) {
        if (speed >= 0) {
            // 正转
            GPIO_SetBits(GPIOB, GPIO_Pin_12);   // AIN1 = 1
            GPIO_ResetBits(GPIOB, GPIO_Pin_13); // AIN2 = 0
            TIM_SetCompare3(TIM2, speed);       // PWMA
        } else {
            // 反转
            GPIO_ResetBits(GPIOB, GPIO_Pin_12); // AIN1 = 0
            GPIO_SetBits(GPIOB, GPIO_Pin_13);   // AIN2 = 1
            TIM_SetCompare3(TIM2, -speed);      // PWMA
        }
    } else {
        if (speed >= 0) {
            // 正转
            GPIO_SetBits(GPIOB, GPIO_Pin_14);   // BIN1 = 1
            GPIO_ResetBits(GPIOB, GPIO_Pin_15); // BIN2 = 0
            TIM_SetCompare4(TIM2, speed);       // PWMB
        } else {
            // 反转
            GPIO_ResetBits(GPIOB, GPIO_Pin_14); // BIN1 = 0
            GPIO_SetBits(GPIOB, GPIO_Pin_15);   // BIN2 = 1
            TIM_SetCompare4(TIM2, -speed);      // PWMB
        }
    }
}

void Motor_Stop(uint8_t motor_id)
{
    if (motor_id == MOTOR1) {
        GPIO_ResetBits(GPIOB, GPIO_Pin_12); // AIN1 = 0
        GPIO_ResetBits(GPIOB, GPIO_Pin_13); // AIN2 = 0
        TIM_SetCompare3(TIM2, 0);           // PWMA = 0
    } else {
        GPIO_ResetBits(GPIOB, GPIO_Pin_14); // BIN1 = 0
        GPIO_ResetBits(GPIOB, GPIO_Pin_15); // BIN2 = 0
        TIM_SetCompare4(TIM2, 0);           // PWMB = 0
    }
}

void Motor_Brake(uint8_t motor_id)
{
    if (motor_id == MOTOR1) {
        GPIO_SetBits(GPIOB, GPIO_Pin_12); // AIN1 = 1
        GPIO_SetBits(GPIOB, GPIO_Pin_13); // AIN2 = 1
        TIM_SetCompare3(TIM2, 1000);      // PWMA = max
    } else {
        GPIO_SetBits(GPIOB, GPIO_Pin_14); // BIN1 = 1
        GPIO_SetBits(GPIOB, GPIO_Pin_15); // BIN2 = 1
        TIM_SetCompare4(TIM2, 1000);      // PWMB = max
    }
}