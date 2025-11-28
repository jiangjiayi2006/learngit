#include "key.h"
#include "stm32f10x.h"
#include "delay.h"

#define KEY_GPIO GPIOC
#define START_PIN GPIO_Pin_13

void Key_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = START_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY_GPIO, &GPIO_InitStructure);
}

uint8_t Key_Scan(void) {
    static uint8_t key_up = 1;
    
    if(key_up && (GPIO_ReadInputDataBit(KEY_GPIO, START_PIN) == 0)) {
        Delay_ms(10);
        key_up = 0;
        return 1;
    } else if(GPIO_ReadInputDataBit(KEY_GPIO, START_PIN) == 1) {
        key_up = 1;
    }
    
    return 0;
}