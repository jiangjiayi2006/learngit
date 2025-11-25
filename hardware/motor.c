#include "motor.h"
#include "stm32f10x.h"

// TB6612引脚定义 - 请根据实际连接修改
#define MOTOR_GPIO GPIOA
#define AIN1_PIN   GPIO_Pin_0
#define AIN2_PIN   GPIO_Pin_1
#define BIN1_PIN   GPIO_Pin_2
#define BIN2_PIN   GPIO_Pin_3
#define PWMA_PIN   GPIO_Pin_6
#define PWMB_PIN   GPIO_Pin_7

#define PWMA_TIM   TIM3
#define PWMB_TIM   TIM3
#define PWMA_CH    TIM_Channel_1
#define PWMB_CH    TIM_Channel_2

void Motor_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    // 初始化方向控制引脚
    GPIO_InitStructure.GPIO_Pin = AIN1_PIN | AIN2_PIN | BIN1_PIN | BIN2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MOTOR_GPIO, &GPIO_InitStructure);
    
    // 初始化PWM引脚
    GPIO_InitStructure.GPIO_Pin = PWMA_PIN | PWMB_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MOTOR_GPIO, &GPIO_InitStructure);
    
    // 初始化定时器 - 10kHz PWM
    TIM_TimeBaseStructure.TIM_Period = 999; // ARR
    TIM_TimeBaseStructure.TIM_Prescaler = 71; // 72MHz/(71+1)=1MHz, 1MHz/1000=1kHz
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(PWMA_TIM, &TIM_TimeBaseStructure);
    
    // 初始化PWM通道
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_Pulse = 0; // 初始占空比0
    
    TIM_OC1Init(PWMA_TIM, &TIM_OCInitStructure);
    TIM_OC2Init(PWMB_TIM, &TIM_OCInitStructure);
    
    // 启动定时器
    TIM_Cmd(PWMA_TIM, ENABLE);
    TIM_CtrlPWMOutputs(PWMA_TIM, ENABLE);
    
    // 初始停止电机
    Motor_SetSpeed(0, 0);
}

void Motor_SetSpeed(int16_t left, int16_t right) {
    // 限制速度范围 -1000 ~ 1000
    left = (left < -1000) ? -1000 : (left > 1000 ? 1000 : left);
    right = (right < -1000) ? -1000 : (right > 1000 ? 1000 : right);
    
    // 左电机控制
    if (left > 0) {
        GPIO_SetBits(MOTOR_GPIO, AIN1_PIN);
        GPIO_ResetBits(MOTOR_GPIO, AIN2_PIN);
        TIM_SetCompare1(PWMA_TIM, left);
    } else if (left < 0) {
        GPIO_ResetBits(MOTOR_GPIO, AIN1_PIN);
        GPIO_SetBits(MOTOR_GPIO, AIN2_PIN);
        TIM_SetCompare1(PWMA_TIM, -left);
    } else {
        GPIO_ResetBits(MOTOR_GPIO, AIN1_PIN);
        GPIO_ResetBits(MOTOR_GPIO, AIN2_PIN);
        TIM_SetCompare1(PWMA_TIM, 0);
    }
    
    // 右电机控制
    if (right > 0) {
        GPIO_SetBits(MOTOR_GPIO, BIN1_PIN);
        GPIO_ResetBits(MOTOR_GPIO, BIN2_PIN);
        TIM_SetCompare2(PWMB_TIM, right);
    } else if (right < 0) {
        GPIO_ResetBits(MOTOR_GPIO, BIN1_PIN);
        GPIO_SetBits(MOTOR_GPIO, BIN2_PIN);
        TIM_SetCompare2(PWMB_TIM, -right);
    } else {
        GPIO_ResetBits(MOTOR_GPIO, BIN1_PIN);
        GPIO_ResetBits(MOTOR_GPIO, BIN2_PIN);
        TIM_SetCompare2(PWMB_TIM, 0);
    }
}