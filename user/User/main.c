#include "common.h"
#include "system.h"
#include "key.h"
#include "serial.h"
#include "encoder.h"
#include "motor.h"
#include "pid.h"
#include "vofa.h"

// 简单的OLED模拟函数（如果没有OLED硬件）
void OLED_Init(void) {}
void OLED_ShowString(uint8_t x, uint8_t y, char *str) {}
void OLED_Refresh(void) {}

int main(void)
{
    // 系统初始化
    System_Init();
    OLED_Init();
    KEY_Init();
    Serial_Init();
    Encoder_Init();
    Motor_Init();
    PID_Init();
    VOFA_Init();
    
    // 显示启动信息
    OLED_ShowString(0, 0, "ASC Motor Control");
    OLED_ShowString(0, 2, "Mode: Speed Ctrl");
    OLED_ShowString(0, 4, "Speed: 0 RPM");
    OLED_ShowString(0, 6, "Target: 0 RPM");
    
    printf("System Started!\n");
    
    while(1)
    {
        // 按键处理
        KEY_Process();
        
        // 系统状态更新
        System_Update();
        
        // OLED显示更新
        OLED_Refresh();
        
        // 数据发送到VOFA+
        VOFA_SendData();
    }
}