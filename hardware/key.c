#include "key.h"
#include "system.h"

static uint8_t key_state = 0;
static uint32_t last_press_time = 0;

void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // PA0 ????
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void KEY_Process(void)
{
    static uint8_t last_key = 1;
    uint8_t current_key = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
    
    // 按键按下检测（含消抖）
    if (last_key == 1 && current_key == 0) {
        if (system_tick - last_press_time > 200) { // 200ms??
            key_state = 1;
            last_press_time = system_tick;
        }
    } else {
        key_state = 0;
    }
    
    last_key = current_key;
}

uint8_t KEY_GetState(void)
{
    return key_state;
}