#include "motor.h"
#include "stm32f10x.h"

// TB6612引脚定义
#define MOTOR_GPIO GPIOA

// 左电机控制引脚
#define AIN1_PIN GPIO_Pin_0
#define AIN2_PIN GPIO_Pin_1
#define PWMA_PIN GPIO_Pin_6

// 右电机控制引脚  
#define BIN1_PIN GPIO_Pin_2
#define BIN2_PIN GPIO_Pin_3
#define PWMB_PIN GPIO_Pin_7

// PWM定时器
#define PWM_TIM TIM3
#define PWMA_CH TIM_Channel_1
#define PWMB_CH TIM_Channel_2

void Motor_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_BaseInitStruct;
    TIM_OCInitTypeDef TIM_OCInitStruct;
    
    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    // 配置方向控制引脚 - 分开配置左右电机
    GPIO_InitStruct.GPIO_Pin = AIN1_PIN | AIN2_PIN; // 左电机方向
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MOTOR_GPIO, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = BIN1_PIN | BIN2_PIN; // 右电机方向
    GPIO_Init(MOTOR_GPIO, &GPIO_InitStruct);
    
    // 配置PWM引脚
    GPIO_InitStruct.GPIO_Pin = PWMA_PIN | PWMB_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MOTOR_GPIO, &GPIO_InitStruct);
    
    // 配置定时器 - 10kHz PWM
    TIM_BaseInitStruct.TIM_Period = 999;
    TIM_BaseInitStruct.TIM_Prescaler = 71;
    TIM_BaseInitStruct.TIM_ClockDivision = 0;
    TIM_BaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(PWM_TIM, &TIM_BaseInitStruct);
    
    // 配置PWM输出
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStruct.TIM_Pulse = 0;
    
    TIM_OC1Init(PWM_TIM, &TIM_OCInitStruct); // PA6 - 左电机PWM
    TIM_OC2Init(PWM_TIM, &TIM_OCInitStruct); // PA7 - 右电机PWM
    
    // 启动定时器
    TIM_Cmd(PWM_TIM, ENABLE);
    TIM_CtrlPWMOutputs(PWM_TIM, ENABLE);
    
    // 初始状态：停止
    GPIO_ResetBits(MOTOR_GPIO, AIN1_PIN | AIN2_PIN | BIN1_PIN | BIN2_PIN);
}

void Motor_SetSpeed(int16_t left, int16_t right) {
    // 限制速度范围
    if (left > 1000) left = 1000;
    if (left < -1000) left = -1000;
    if (right > 1000) right = 1000;
    if (right < -1000) right = -1000;
    
    // 调试：确保左右电机都能接收到速度值
    // 可以在实际使用中通过LED或其他方式验证
    
    // 左电机控制 - 确保这部分能正确执行
    if (left > 0) {
        // 正转
        GPIO_SetBits(MOTOR_GPIO, AIN1_PIN);
        GPIO_ResetBits(MOTOR_GPIO, AIN2_PIN);
        TIM_SetCompare1(PWM_TIM, left);
    } else if (left < 0) {
        // 反转
        GPIO_ResetBits(MOTOR_GPIO, AIN1_PIN);
        GPIO_SetBits(MOTOR_GPIO, AIN2_PIN);
        TIM_SetCompare1(PWM_TIM, -left);
    } else {
        // 停止 - 确保能进入这个分支
        GPIO_ResetBits(MOTOR_GPIO, AIN1_PIN);
        GPIO_ResetBits(MOTOR_GPIO, AIN2_PIN);
        TIM_SetCompare1(PWM_TIM, 0);
    }
    
    // 右电机控制
    if (right > 0) {
        // 正转
        GPIO_SetBits(MOTOR_GPIO, BIN1_PIN);
        GPIO_ResetBits(MOTOR_GPIO, BIN2_PIN);
        TIM_SetCompare2(PWM_TIM, right);
    } else if (right < 0) {
        // 反转
        GPIO_ResetBits(MOTOR_GPIO, BIN1_PIN);
        GPIO_SetBits(MOTOR_GPIO, BIN2_PIN);
        TIM_SetCompare2(PWM_TIM, -right);
    } else {
        // 停止
        GPIO_ResetBits(MOTOR_GPIO, BIN1_PIN);
        GPIO_ResetBits(MOTOR_GPIO, BIN2_PIN);
        TIM_SetCompare2(PWM_TIM, 0);
    }
}