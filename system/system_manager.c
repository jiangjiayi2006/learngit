#include "system_manager.h"
#include "oled.h"
#include "motor.h"
#include "trace_algorithm.h"
#include "key.h"
#include "delay.h"
#include "trace_sensor.h"
#include <stdio.h>

// 状态变量 - 添加volatile修饰
volatile SystemState system_state = STATE_MENU;
volatile uint8_t menu_selection = 1; // 默认选择1

void System_Init(void) {
    // 初始化所有模块
    Motor_Init();
    OLED_Init();
    TraceSensor_Init();
    Key_Init();
    
    // 初始显示菜单
    OLED_ShowMenu();
    OLED_ShowChar(56, 6, '1'); // 默认选择1
}

void System_HandleMenu(void) {
    uint8_t key = Key_Scan();
    
    if(key != 0) {
        uint8_t old_selection = menu_selection;
        
        switch(key) {
            case 1: // 上键
                if(menu_selection > 1) {
                    menu_selection--;
                } else {
                    menu_selection = 4; // 循环到最后一个
                }
                break;
                
            case 2: // 下键
                if(menu_selection < 4) {
                    menu_selection++;
                } else {
                    menu_selection = 1; // 循环到第一个
                }
                break;
                
            case 3: // 确认键
                System_ExecuteSelection(menu_selection);
                break;
        }
        
        // 只有选择变化时才更新显示
        if(old_selection != menu_selection) {
            OLED_ShowMenu();
            OLED_ShowChar(56, 6, '0' + menu_selection);
        }
    }
}

void System_ExecuteSelection(uint8_t selection) {
    // 退出当前状态前的清理工作
    switch(system_state) {
        case STATE_TRACING:
            Motor_SetSpeed(0, 0); // 停止电机
            break;
        default:
            break;
    }
    
    switch(selection) {
        case 1: // 循迹模式
            system_state = STATE_TRACING;
            OLED_Clear();
            OLED_ShowString(0, 0, "Tracing Mode");
            OLED_ShowString(0, 2, "Cross: 0");
            OLED_ShowString(0, 4, "Status: Ready");
            ResetCrossCount(); // 重置计数器
            break;
            
        case 2: // 设置
            system_state = STATE_SETTINGS;
            System_ShowSettings();
            break;
            
        case 3: // 校准
            system_state = STATE_CALIBRATION;
            System_CalibrateSensors();
            break;
            
        case 4: // 退出/返回
            system_state = STATE_MENU;
            OLED_ShowMenu();
            OLED_ShowChar(56, 6, '0' + menu_selection);
            break;
            
        default: // 无效选择保护
            system_state = STATE_MENU;
            break;
    }
}

void System_HandleTracing(void) {
    static uint32_t last_update = 0;
    static uint32_t last_control = 0;
    uint32_t current_time = GetSystemTick();
    
    // 控制频率：每20ms执行一次循迹算法
    if(current_time - last_control >= 20) {
        Trace_Algorithm();
        last_control = current_time;
    }
    
    // 每200ms更新一次显示
    if(current_time - last_update > 200) {
        char buffer[16];
        
        // 更新十字路口计数
        OLED_ShowString(48, 2, "    ");
        sprintf(buffer, "%d", GetCrossCount());
        OLED_ShowString(48, 2, buffer);
        
        // 更新路径状态
        OLED_ShowString(48, 4, "        ");
        switch(GetCurrentPathType()) {
            case PATH_STRAIGHT:
                OLED_ShowString(48, 4, "Straight");
                break;
            case PATH_CURVE:
                OLED_ShowString(48, 4, "Curve   ");
                break;
            case PATH_CROSS:
                OLED_ShowString(48, 4, "Cross   ");
                break;
            case PATH_LOST:
                OLED_ShowString(48, 4, "Lost    ");
                break;
        }
        
        last_update = current_time;
    }
    
    // 检查退出条件
    uint8_t key = Key_Scan();
    if(key == 3) { // 确认键退出
        Motor_SetSpeed(0, 0);
        system_state = STATE_MENU;
        OLED_ShowMenu();
        OLED_ShowChar(56, 6, '0' + menu_selection);
    }
}

void System_ShowSettings(void) {
    OLED_Clear();
    OLED_ShowString(0, 0, "Settings Menu");
    OLED_ShowString(0, 2, "1.PID Params");
    OLED_ShowString(0, 3, "2.Speed Set");
    OLED_ShowString(0, 4, "3.Back");
    OLED_ShowString(0, 6, "Select: 1");
}

// 修复：实现完整的传感器校准
void System_CalibrateSensors(void) {
    OLED_Clear();
    OLED_ShowString(0, 0, "Calibration");
    OLED_ShowString(0, 2, "Place on white");
    OLED_ShowString(0, 3, "Press OK...");
    
    // 等待按键，带超时检测
    uint32_t start_time = GetSystemTick();
    while(Key_Scan() != 3) {
        if(GetSystemTick() - start_time > 10000) { // 10秒超时
            OLED_ShowString(0, 5, "Timeout!     ");
            Delay_ms(1000);
            system_state = STATE_MENU;
            OLED_ShowMenu();
            return;
        }
    }
    
    // 执行白场校准
    OLED_ShowString(0, 4, "White sampling...");
    TraceSensor_Calibrate(); // 现在这个函数是实际实现的
    
    OLED_ShowString(0, 5, "Calibration OK!");
    Delay_ms(1000);
    
    system_state = STATE_MENU;
    OLED_ShowMenu();
    OLED_ShowChar(56, 6, '0' + menu_selection);
}

SystemState GetSystemState(void) {
    return system_state;
}