#include "system_manager.h"
#include "motor.h"
#include "trace_algorithm.h"
#include "key.h"
#include "trace_sensor.h"
#include "Encoder.h"

static volatile SystemState system_state = STATE_WAIT_START;

void Hardware_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 状态LED
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOC, GPIO_Pin_14);
    
    // 初始化所有硬件模块
    Motor_Init();
    TraceSensor_Init();
    Key_Init();
    Encoder_Left_Init();
    Encoder_Right_Init();
    
    Motor_SetSpeed(0, 0);
}

void Tracing_Handler(void) {
    static uint32_t last_control = 0;
    uint32_t current_time = Get_SystemTick();
    
    if(current_time - last_control >= 20) {
        Trace_Algorithm();
        last_control = current_time;
    }
}

SystemState GetSystemState(void) {
    return system_state;
}

void SetSystemState(SystemState new_state) {
    system_state = new_state;
}