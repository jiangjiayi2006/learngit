#include "MyHeader.h"

volatile int g_oled_clear_request = 0;
volatile int g_encoder_init = 0;
volatile int g_encoder_deinit = 0;
volatile int g_oled_show_oneplus_request = 0;

enum TEST test = TEST_1;
volatile int16_t Encoder_Count = 0;
enum Motor_Mode Motor1_Mode = Motor_Mode_break;
enum Motor_Mode Motor2_Mode = Motor_Mode_break;

volatile int16_t Motor1_Speed;
volatile int16_t Motor2_Speed;
volatile int32_t Motor1_Position = 0;
volatile int32_t Motor2_Position = 0;

volatile float pid_subject = 0;
volatile float Target, Actual, Out;
volatile float Kp = 0.4, Ki = 0.22, Kd = 0;
volatile float Error0, Error1, Error2;

volatile float OuterTarget, OuterActual, OuterOut;
volatile float OuterKp = 0.3, OuterKi = 0, OuterKd = 0.12;
volatile float OuterError0, OuterError1, OuterError2;

volatile float Motor2_Target, Motor2_Actual, Motor2_Out;
volatile float Motor2_Kp = 0.4, Motor2_Ki = 0.22, Motor2_Kd = 0;
volatile float Motor2_Error0, Motor2_Error1, Motor2_Error2;

// TIM1中断处理函数
void TIM1_UP_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
    {
        // 读取电机速度
        Motor1_Speed = Motor1_getSpeed();
        Motor2_Speed = Motor2_getSpeed();
        
        // 更新电机位置
        Motor1_Position += Motor1_Speed;
        Motor2_Position += Motor2_Speed;
        
        // PID控制
        PIDControl();
        
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}

int main()
{
    // 系统初始化
    SystemTick_Init();
    Serial_Init();
    TIM1_Init();
    OLED_Init();
    Motor_Init();
    Motor_SetMode(1, Motor_Mode_stop);
    Motor_SetMode(2, Motor_Mode_stop);
    Motor1_SetPrescaler(72-1);
    Motor2_SetPrescaler(72-1);
    ButtonInit();
    PID_Init(); // 初始化PID控制器

    while (1)
    {
        OLED_ShowChar(1, 1, (test == TEST_1 ? '1' : '2'));
        
        if(test == TEST_1)
        {
            if(g_oled_clear_request == 1) OLED_Clear();
            g_oled_clear_request = 0;
            
            if(g_encoder_deinit == 1) Encoder_DeInit();
            g_encoder_deinit = 0;
            
            if(g_oled_show_oneplus_request == 1) OLED_ShowChar(1, 2, '+');
            else OLED_ShowChar(1, 2, ' ');
            g_oled_show_oneplus_request = 0;
            
            OLED_ShowSignedNum(1, 7, Motor1_Speed, 5);
            OLED_ShowNum(2, 1, (int)Motor1_Mode, 1);
            OLED_ShowNum(3, 1, Target, 3);
            OLED_ShowFloat(2, 8, Kp, 3);
            OLED_ShowFloat(3, 8, Ki, 3);
            OLED_ShowFloat(4, 8, Kd, 3);
            
            processCmd();
            if(test == TEST_1 && Target == 0) OLED_ShowChar(1, 2, '+');
        }
        else if(test == TEST_2)
        {
            if(g_oled_clear_request == 1) OLED_Clear();
            g_oled_clear_request = 0;
            
            if(g_encoder_init == 1) Encoder_Init();
            g_encoder_init = 0;
            
            OLED_ShowSignedNum(1, 7, Motor1_Speed, 5);
            OLED_ShowSignedNum(2, 7, Motor2_Speed, 5);
            OLED_ShowFloat(2, 1, OuterKp, 3);
            OLED_ShowFloat(3, 1, OuterKi, 3);
            OLED_ShowFloat(4, 1, OuterKd, 3);
            OLED_ShowFloat(2, 8, Motor2_Kp, 3);
            OLED_ShowFloat(3, 8, Motor2_Ki, 3);
            OLED_ShowFloat(4, 8, Motor2_Kd, 3);
            
            processCmd();
        }
    }
}