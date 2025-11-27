#include "system_manager.h"
#include "motor.h"
#include "trace_algorithm.h"
#include "key.h"  // 包含新的按键头文件
#include "delay.h"
#include "trace_sensor.h"
#include "Encoder.h"

// 系统状态
static volatile SystemState system_state = STATE_WAIT_START;

void Hardware_Init(void) {
    // 初始化所有硬件模块
    Motor_Init();
    TraceSensor_Init();
    Key_Init();
    Encoder_Left_Init();
    Encoder_Right_Init();
    
    // 初始化状态：停止电机
    Motor_SetSpeed(0, 0);
}

void Tracing_Handler(void) {
    static uint32_t last_control = 0;
    uint32_t current_time = Get_SystemTick();
    
    // 控制周期：每20ms执行一次循迹算法
    if(current_time - last_control >= 20) {
        Trace_Algorithm();
        last_control = current_time;
    }
    
    // 停止按键检测 - 使用新的按键函数
    if(Key_IsJustPressed()) {
        Motor_SetSpeed(0, 0);
        system_state = STATE_WAIT_START;
    }
}

SystemState GetSystemState(void) {
    return system_state;
}

void SetSystemState(SystemState new_state) {
    system_state = new_state;
}