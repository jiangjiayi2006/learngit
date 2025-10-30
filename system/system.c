#include "system.h"
#include "oled.h"
#include "pid.h"
#include "encoder.h"
#include "key.h"

volatile uint32_t system_tick = 0;
static SystemMode_t current_mode = MODE_SPEED_CONTROL;

void System_Init(void)
{
    // 配置系统时钟
    SystemInit();
    
    // 配置SysTick定时器 1ms中断
    if (SysTick_Config(SystemCoreClock / 1000)) {
        while (1);
    }
    
    current_mode = MODE_SPEED_CONTROL;
}

void System_Update(void)
{
    static uint32_t last_update = 0;
    
    // 每100ms更新一次
    if (system_tick - last_update >= 100) {
        last_update = system_tick;
        
        // 更新OLED显示
        switch (current_mode) {
            case MODE_SPEED_CONTROL:
                OLED_ShowString(0, 2, "Mode: Speed Ctrl ");
                break;
            case MODE_POSITION_FOLLOW:
                OLED_ShowString(0, 2, "Mode: Pos Follow");
                break;
        }
        
        // 更新速度显示
        char buffer[16];
        sprintf(buffer, "S1:%4d S2:%4d", 
                Encoder_GetSpeed(MOTOR1), 
                Encoder_GetSpeed(MOTOR2));
        OLED_ShowString(0, 4, buffer);
        
        sprintf(buffer, "T1:%4.0f T2:%4.0f", 
                PID_GetTargetSpeed(MOTOR1), 
                PID_GetTargetSpeed(MOTOR2));
        OLED_ShowString(0, 6, buffer);
    }
}

void System_ChangeMode(SystemMode_t new_mode)
{
    current_mode = new_mode;
    
    // 模式切换时重置PID
    PID_Reset();
    
    // 更新显示
    System_Update();
}

SystemMode_t System_GetCurrentMode(void)
{
    return current_mode;
}

uint32_t System_GetTick(void)
{
    return system_tick;
}

// 10ms任务处理函数（在SysTick中断中调用）
void System_10ms_Task(void)
{
    static uint32_t last_10ms = 0;
    
    if (system_tick - last_10ms >= 10) {
        last_10ms = system_tick;
        
        // 10ms任务
        Encoder_UpdateSpeed();
        PID_Update();
        
        // 处理按键模式切换
        if (KEY_GetState()) {
            SystemMode_t new_mode = (System_GetCurrentMode() == MODE_SPEED_CONTROL) ? 
                                   MODE_POSITION_FOLLOW : MODE_SPEED_CONTROL;
            System_ChangeMode(new_mode);
        }
    }
}

__attribute__((weak)) void SysTick_Handler(void)
{
   
    system_tick++;
    System_10ms_Task();
}