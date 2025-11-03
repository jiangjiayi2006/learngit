#include "MyHeader.h"

void Motor_Init(void)
{
    GPIO_InitTypeDef GPIO_IS;
    TIM_TimeBaseInitTypeDef TIM_TBIS;
    TIM_OCInitTypeDef TIM_OCIS;
    TIM_ICInitTypeDef TIM_ICIS;

    /* 电机1 PWM初始化 - PWMA -> PA2 TIM2_CH3 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /* 配置PA2为TIM2_CH3 */
    GPIO_IS.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_IS.GPIO_Pin = GPIO_Pin_2;
    GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_IS);

    /* TIM2基础配置 */
    TIM_TBIS.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TBIS.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TBIS.TIM_Period = 100 - 1;
    TIM_TBIS.TIM_Prescaler = 72 - 1;
    TIM_TBIS.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TBIS);

    /* TIM2通道3 PWM配置 */
    TIM_OCStructInit(&TIM_OCIS);
    TIM_OCIS.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCIS.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCIS.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCIS.TIM_Pulse = 0;
    TIM_OC3Init(TIM2, &TIM_OCIS);

    /* 电机1方向控制 - AIN1 -> PB12, AIN2 -> PB13 */
    GPIO_IS.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_IS.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_IS);
    GPIO_SetBits(GPIOB, GPIO_Pin_12);
    GPIO_SetBits(GPIOB, GPIO_Pin_13);

    /* 电机1编码器 - PA6 TIM3_CH1, PA7 TIM3_CH2 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    GPIO_IS.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_IS.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_IS);

    TIM_TBIS.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TBIS.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TBIS.TIM_Period = 65535;
    TIM_TBIS.TIM_Prescaler = 0;
    TIM_TBIS.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TBIS);

    TIM_ICStructInit(&TIM_ICIS);
    TIM_ICIS.TIM_Channel = TIM_Channel_1;
    TIM_ICIS.TIM_ICFilter = 0xF;
    TIM_ICInit(TIM3, &TIM_ICIS);
    TIM_ICIS.TIM_Channel = TIM_Channel_2;
    TIM_ICInit(TIM3, &TIM_ICIS);

    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    TIM_Cmd(TIM3, ENABLE);

    /* 电机2 PWM初始化 - PWMB -> PA3 TIM2_CH4 */
    GPIO_IS.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_IS.GPIO_Pin = GPIO_Pin_3;
    GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_IS);

    /* TIM2通道4 PWM配置 */
    TIM_OCIS.TIM_Pulse = 0;
    TIM_OC4Init(TIM2, &TIM_OCIS);

    /* 电机2方向控制 - BIN1 -> PB14, BIN2 -> PB15 */
    GPIO_IS.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_IS.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_IS);
    GPIO_SetBits(GPIOB, GPIO_Pin_14);
    GPIO_SetBits(GPIOB, GPIO_Pin_15);

    /* 电机2编码器 - PB6 TIM4_CH1, PB7 TIM4_CH2 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    GPIO_IS.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_IS.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_IS);

    TIM_TBIS.TIM_Period = 65535;
    TIM_TimeBaseInit(TIM4, &TIM_TBIS);

    TIM_ICStructInit(&TIM_ICIS);
    TIM_ICIS.TIM_Channel = TIM_Channel_1;
    TIM_ICIS.TIM_ICFilter = 0xF;
    TIM_ICInit(TIM4, &TIM_ICIS);
    TIM_ICIS.TIM_Channel = TIM_Channel_2;
    TIM_ICInit(TIM4, &TIM_ICIS);

    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    TIM_Cmd(TIM4, ENABLE);

    /* 启动TIM2 */
    TIM_Cmd(TIM2, ENABLE);
}

void TIM1_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    TIM_TimeBaseInitStruct.TIM_Prescaler = 7199;
    TIM_TimeBaseInitStruct.TIM_Period = 99;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);

    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = TIM1_UP_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    TIM_Cmd(TIM1, ENABLE);
}

int16_t Motor1_getSpeed(void)
{
    int16_t Temp = TIM_GetCounter(TIM3);
    TIM_SetCounter(TIM3, 0);
    return Temp;
}

int16_t Motor2_getSpeed(void)
{
    int16_t Temp = TIM_GetCounter(TIM4);
    TIM_SetCounter(TIM4, 0);
    return Temp;
}

void Motor1_SetCompare1(uint16_t Compare)
{
    TIM_SetCompare3(TIM2, Compare);
}

void Motor2_SetCompare1(uint16_t Compare)
{
    TIM_SetCompare4(TIM2, Compare);
}

void Motor_SetMode(int numMotor, enum Motor_Mode Mode)
{
    if(numMotor == 1){
        switch (Mode)
        {
        case Motor_Mode_break:
            GPIO_SetBits(GPIOB, GPIO_Pin_12);
            GPIO_SetBits(GPIOB, GPIO_Pin_13);
            break;
        case Motor_Mode_stop:
            GPIO_ResetBits(GPIOB, GPIO_Pin_12);
            GPIO_ResetBits(GPIOB, GPIO_Pin_13);
            break;
        case Motor_Mode_frd_rotation:
            GPIO_SetBits(GPIOB, GPIO_Pin_12);
            GPIO_ResetBits(GPIOB, GPIO_Pin_13);
            break;
        case Motor_Mode_rvs_rotation:
            GPIO_ResetBits(GPIOB, GPIO_Pin_12);
            GPIO_SetBits(GPIOB, GPIO_Pin_13);
            break;
        default:
            break;
        }
        Motor1_Mode = Mode;
    }
    else if(numMotor == 2){
        switch (Mode)
        {
        case Motor_Mode_break:
            GPIO_SetBits(GPIOB, GPIO_Pin_14);
            GPIO_SetBits(GPIOB, GPIO_Pin_15);
            break;
        case Motor_Mode_stop:
            GPIO_ResetBits(GPIOB, GPIO_Pin_14);
            GPIO_ResetBits(GPIOB, GPIO_Pin_15);
            break;
        case Motor_Mode_frd_rotation:
            GPIO_SetBits(GPIOB, GPIO_Pin_14);
            GPIO_ResetBits(GPIOB, GPIO_Pin_15);
            break;
        case Motor_Mode_rvs_rotation:
            GPIO_ResetBits(GPIOB, GPIO_Pin_14);
            GPIO_SetBits(GPIOB, GPIO_Pin_15);
            break;
        default:
            break;
        }
        Motor2_Mode = Mode;
    }
}

void Motor_SetPWM(int8_t PWM)
{
    if (PWM >= 0)
    {
        Motor_SetMode(1, Motor_Mode_frd_rotation);
        Motor1_SetCompare1(PWM);
    }
    else
    {
        Motor_SetMode(1, Motor_Mode_rvs_rotation);
        Motor1_SetCompare1(-PWM);
    }
}

void Motor2_SetPWM(int8_t PWM)
{
    if (PWM >= 0)
    {
        Motor_SetMode(2, Motor_Mode_frd_rotation);
        Motor2_SetCompare1(PWM);
    }
    else
    {
        Motor_SetMode(2, Motor_Mode_rvs_rotation);
        Motor2_SetCompare1(-PWM);
    }
}

void Motor1_SetPrescaler(uint16_t Prescaler)
{
    TIM_PrescalerConfig(TIM2, Prescaler, TIM_PSCReloadMode_Immediate);
}

void Motor2_SetPrescaler(uint16_t Prescaler)
{
    TIM_PrescalerConfig(TIM2, Prescaler, TIM_PSCReloadMode_Immediate);
}

void ButtonInit(void)
{
    GPIO_InitTypeDef GPIO_IS;
    EXTI_InitTypeDef EXTI_IS;
    NVIC_InitTypeDef NVIC_IS;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    GPIO_IS.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_IS.GPIO_Pin = GPIO_Pin_0;
    GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_IS);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

    EXTI_IS.EXTI_Line = EXTI_Line0;
    EXTI_IS.EXTI_LineCmd = ENABLE;
    EXTI_IS.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_IS.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Init(&EXTI_IS);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_IS.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_IS.NVIC_IRQChannelCmd = ENABLE;
    NVIC_IS.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_IS.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_IS);
}

void EXTI0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) == SET)
    {
        Delay_ms(20);
        while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0);
        Delay_ms(20);
        
        g_encoder_init = (test == TEST_1 ? 1 : 0);
        g_encoder_deinit = (test == TEST_2 ? 1 : 0);
        test = (test == TEST_1 ? TEST_2 : TEST_1);
        g_oled_clear_request = 1;
        
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}