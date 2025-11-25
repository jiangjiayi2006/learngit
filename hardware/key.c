#include "key.h"
#include "stm32f10x.h"
#include "delay.h"

// 按键引脚定义 - 请根据实际连接修改
#define KEY_GPIO GPIOA
#define KEY1_PIN GPIO_Pin_8
#define KEY2_PIN GPIO_Pin_9
#define KEY3_PIN GPIO_Pin_10

void Key_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = KEY1_PIN | KEY2_PIN | KEY3_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY_GPIO, &GPIO_InitStructure);
}

uint8_t Key_Scan(void) {
    static uint8_t key_up = 1;
    
    if(key_up && (GPIO_ReadInputDataBit(KEY_GPIO, KEY1_PIN)==0 || 
                  GPIO_ReadInputDataBit(KEY_GPIO, KEY2_PIN)==0 ||
                  GPIO_ReadInputDataBit(KEY_GPIO, KEY3_PIN)==0)) {
        Delay_ms(10); // 消抖
        key_up = 0;
        
        if(GPIO_ReadInputDataBit(KEY_GPIO, KEY1_PIN)==0) return 1;
        else if(GPIO_ReadInputDataBit(KEY_GPIO, KEY2_PIN)==0) return 2;
        else if(GPIO_ReadInputDataBit(KEY_GPIO, KEY3_PIN)==0) return 3;
    } else if(GPIO_ReadInputDataBit(KEY_GPIO, KEY1_PIN)==1 && 
              GPIO_ReadInputDataBit(KEY_GPIO, KEY2_PIN)==1 &&
              GPIO_ReadInputDataBit(KEY_GPIO, KEY3_PIN)==1) {
        key_up = 1;
    }
    
    return 0;
}