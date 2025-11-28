#include "stm32f10x.h"
#include "trace_sensor.h"
#include "delay.h"
#include "motor.h"

void Sensor_Debug_Test(void) {
    // 测试传感器原始值
    while(1) {
        for(int i = 0; i < 5; i++) {
            uint16_t raw_value = TraceSensor_ReadSingle(sensor_channels[i]);
            // 这里可以通过串口输出，或者用LED闪烁次数表示数值
            // 简单方法：观察数值变化趋势
        }
        Delay_ms(500);
    }
}

// 简单直接的控制测试
void Direct_Control_Test(void) {
    while(1) {
        uint8_t sensor_state = TraceSensor_Read();
        
        // 直接根据中间传感器控制
        if(sensor_state & 0x04) { // 中间传感器检测到线
            Motor_SetSpeed(600, 600); // 直行
        } 
        else if(sensor_state & 0x03) { // 右侧有检测
            Motor_SetSpeed(800, 200); // 左转
        }
        else if(sensor_state & 0x18) { // 左侧有检测  
            Motor_SetSpeed(200, 800); // 右转
        }
        else { // 没检测到线
            Motor_SetSpeed(400, 400); // 慢速直行寻找
        }
        
        Delay_ms(50);
    }
}