#include "stm32f10x.h"
#include "motor.h"
#include "key.h"
#include "delay.h"
#include "SystemTick.h"

// 最简单的测试 - 绕过所有复杂逻辑
void Basic_Motor_Test(void) {
    // 直接设置电机速度，测试电机驱动是否正常
    Motor_SetSpeed(800, 800);
    Delay_ms(2000);  // 运行2秒
    Motor_SetSpeed(0, 0);  // 停止
    Delay_ms(1000);
    Motor_SetSpeed(-800, -800); // 反转测试
    Delay_ms(2000);
    Motor_SetSpeed(0, 0);
}

int main(void) {
    SystemInit();
    SystemTick_Init();
    Delay_Init();
    
    // 初始化电机和按键
    Motor_Init();
    Key_Init();
    
    // 等待2秒后开始测试
    Delay_ms(2000);
    
    while (1) {
        // 循环运行基础测试
        Basic_Motor_Test();
        Delay_ms(3000); // 等待3秒后重复
    }
}