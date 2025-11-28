#include "trace_sensor.h"
#include "stm32f10x.h"

#define SENSOR_GPIO GPIOB
#define S1_PIN GPIO_Pin_0  // 最右边
#define S2_PIN GPIO_Pin_1  // 右边
#define S3_PIN GPIO_Pin_4  // 中间
#define S4_PIN GPIO_Pin_5  // 左边
#define S5_PIN GPIO_Pin_3  // 最左边

void TraceSensor_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitStruct.GPIO_Pin = S1_PIN | S2_PIN | S3_PIN | S4_PIN | S5_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SENSOR_GPIO, &GPIO_InitStruct);
}

uint8_t TraceSensor_Read(void) {
    uint8_t state = 0;
    
    // 重要：根据实际硬件测试这个逻辑
    // 如果方向不对，把 == 1 改为 == 0
    
    // 当前逻辑：检测到黑线=1，白底=0
    if (GPIO_ReadInputDataBit(SENSOR_GPIO, S1_PIN) == 0) state |= 0x01; // 最右
    if (GPIO_ReadInputDataBit(SENSOR_GPIO, S2_PIN) == 0) state |= 0x02; // 右
    if (GPIO_ReadInputDataBit(SENSOR_GPIO, S3_PIN) == 0) state |= 0x04; // 中
    if (GPIO_ReadInputDataBit(SENSOR_GPIO, S4_PIN) == 0) state |= 0x08; // 左
    if (GPIO_ReadInputDataBit(SENSOR_GPIO, S5_PIN) == 0) state |= 0x10; // 最左
    
    return state;
}

void TraceSensor_Calibrate(void) {
    // 空函数
}