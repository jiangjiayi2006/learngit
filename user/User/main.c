#include "stm32f10x.h"
#include "motor.h"
#include "oled.h"
#include "trace_sensor.h"
#include "trace_algorithm.h"
#include "system_manager.h"
#include "key.h"
#include "delay.h"
#include "watchdog.h"
#include <stdio.h>
#include <stdlib.h>

// 系统时钟配置
void SystemClock_Config(void) {
    // 初始化系统时钟
    SystemInit();
}

int main(void) {
    // 系统初始化
    SystemClock_Config();
    Delay_Init();  
    Watchdog_Init(); // 初始化看门狗
    
    // 硬件初始化
    System_Init();
    
    // 开机显示欢迎信息
    OLED_Clear();
    OLED_ShowString(20, 2, "Smart Car");
    OLED_ShowString(15, 4, "Initializing...");
    Delay_ms(1000);
    
    // 显示主菜单
    OLED_ShowMenu();
    
    uint32_t last_watchdog_feed = GetSystemTick();
    
    while (1) {
        // 喂狗，每200ms一次
        if(GetSystemTick() - last_watchdog_feed > 200) {
            Watchdog_Feed();
            last_watchdog_feed = GetSystemTick();
        }
        
        // 根据系统状态执行不同任务
        switch (GetSystemState()) {
            case STATE_MENU:
                System_HandleMenu();
                break;
                
            case STATE_TRACING:
                System_HandleTracing();
                break;
                
            case STATE_SETTINGS:
                // 处理设置菜单
                System_HandleMenu(); // 重用菜单处理
                break;
                
            case STATE_CALIBRATION:
                // 校准已在System_CalibrateSensors中处理
                break;
        }
        
        // 短暂延时，使用非阻塞方式
        Delay_ms(5);
    }
}

// 断言失败处理
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
    OLED_Clear();
    OLED_ShowString(0, 0, "Assert Failed!");
    
    char file_info[20];
    sprintf(file_info, "File: %s", file);
    OLED_ShowString(0, 2, file_info);
    
    char line_info[20];
    sprintf(line_info, "Line: %lu", line);
    OLED_ShowString(0, 4, line_info);
    
    while (1) {
        Watchdog_Feed(); // 即使断言失败也要喂狗
        Delay_ms(100);
    }
}
#endif