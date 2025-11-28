#include "stm32f10x.h"
#include "motor.h"
#include "trace_sensor.h"
#include "delay.h"
#include "key.h"
#include "SystemTick.h"

// 状态LED
#define LED_GPIO GPIOC
#define LED_PIN GPIO_Pin_14

uint8_t running = 0;

void System_Init(void) {
    SystemInit();
    SystemTick_Init();
    
    // 初始化LED
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = LED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_GPIO, &GPIO_InitStructure);
    GPIO_ResetBits(LED_GPIO, LED_PIN);
}

// 修复的循迹算法 - 确保左右电机都能正确响应
void Fixed_Trace(void) {
    uint8_t sensors = TraceSensor_Read();
    int16_t left_speed = 400;  // 基础速度
    int16_t right_speed = 400; // 基础速度
    
    // 根据传感器状态调整速度
    switch(sensors) {
        // 右边传感器检测到黑线 - 左转
        case 0x01: // 00001 - 最右边传感器
        case 0x03: // 00011 - 最右边+右边传感器
            left_speed = 500;   // 左电机加速
            right_speed = 100;  // 右电机大幅减速
            break;
            
        case 0x02: // 00010 - 右边传感器
            left_speed = 450;   // 左电机稍加速
            right_speed = 200;  // 右电机减速
            break;
            
        // 左边传感器检测到黑线 - 右转  
        case 0x10: // 10000 - 最左边传感器
        case 0x18: // 11000 - 最左边+左边传感器
            left_speed = 100;   // 左电机大幅减速
            right_speed = 500;  // 右电机加速
            break;
            
        case 0x08: // 01000 - 左边传感器
            left_speed = 200;   // 左电机减速
            right_speed = 450;  // 右电机稍加速
            break;
            
        // 中间传感器在线 - 直行
        case 0x04: // 00100
            left_speed = 400;
            right_speed = 400;
            break;
            
        // 十字路口 - 直行
        case 0x1F: // 11111
        case 0x0E: // 01110
            left_speed = 400;
            right_speed = 400;
            break;
            
        // 脱轨 - 慢速直行寻找路线
        case 0x00: // 00000
            left_speed = 250;
            right_speed = 250;
            break;
            
        // 其他组合情况
        default:
            // 偏右的情况
            if(sensors & 0x03) { // 00001, 00010, 00011
                left_speed = 480;
                right_speed = 150;
            }
            // 偏左的情况
            else if(sensors & 0x18) { // 01000, 10000, 11000
                left_speed = 150;
                right_speed = 480;
            }
            // 其他情况直行
            else {
                left_speed = 400;
                right_speed = 400;
            }
            break;
    }
    
    // 设置电机速度
    Motor_SetSpeed(left_speed, right_speed);
}

int main(void) {
    System_Init();
    Delay_Init();
    
    Motor_Init();
    TraceSensor_Init();
    Key_Init();
    
    // 初始状态：停止
    Motor_SetSpeed(0, 0);
    
    // 电机方向测试（可选，测试时取消注释）
    // Motor_SetSpeed(300, 300); Delay_ms(1000);
    // Motor_SetSpeed(0, 0); Delay_ms(500);
    // Motor_SetSpeed(-300, -300); Delay_ms(1000);
    // Motor_SetSpeed(0, 0);
    
    while (1) {
        // 按键控制
        if (Key_Scan() == 1) {
            running = !running;
            if (running) {
                GPIO_SetBits(LED_GPIO, LED_PIN);
            } else {
                GPIO_ResetBits(LED_GPIO, LED_PIN);
                Motor_SetSpeed(0, 0);
            }
            Delay_ms(300);
        }
        
        // 循迹运行
        if (running) {
            Fixed_Trace();
        }
        
        Delay_ms(20);
    }
}