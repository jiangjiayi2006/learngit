#include "key.h"
#include "stm32f10x.h"
#include "SystemTick.h"  // 添加系统滴答支持

// 按键定义 - PC13
#define KEY_GPIO GPIOC
#define START_PIN GPIO_Pin_13

// 按键状态变量
static uint8_t key_last_state = 1; // 假设初始为释放(上拉输入，释放时为1)
static uint32_t key_last_change_time = 0;
static uint32_t key_last_press_time = 0;
#define KEY_DEBOUNCE_MS 20        // 消抖时间
#define KEY_REPEAT_DELAY_MS 300   // 防止连按的最小间隔

void Key_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = START_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY_GPIO, &GPIO_InitStructure);
}

KeyState Key_GetState(void) {
    uint8_t current_state = GPIO_ReadInputDataBit(KEY_GPIO, START_PIN);
    uint32_t current_time = Get_SystemTick();
    
    // 状态变化检测
    if(current_state != key_last_state) {
        // 消抖处理
        if(current_time - key_last_change_time > KEY_DEBOUNCE_MS) {
            uint8_t old_state = key_last_state;
            key_last_state = current_state;
            key_last_change_time = current_time;
            
            if(current_state == 0) { // 按键按下(上拉输入，按下为0)
                // 防连按处理
                if(current_time - key_last_press_time > KEY_REPEAT_DELAY_MS) {
                    key_last_press_time = current_time;
                    return KEY_JUST_PRESSED;
                }
            } else {
                return KEY_JUST_RELEASED;
            }
        }
    }
    
    return current_state ? KEY_RELEASED : KEY_PRESSED;
}

uint8_t Key_IsPressed(void) {
    return (Key_GetState() == KEY_PRESSED);
}

uint8_t Key_IsJustPressed(void) {
    KeyState state = Key_GetState();
    if(state == KEY_JUST_PRESSED) {
        // 重置状态，确保只触发一次
        key_last_state = 0; // 设置为按下状态，避免重复触发
        return 1;
    }
    return 0;
}