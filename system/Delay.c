#include "delay.h"

// 使用SystemTick.c中的全局变量
extern volatile uint32_t g_system_tick;

void Delay_Init(void) {
    // 不再需要初始化，使用SystemTick.c中的初始化
    // 这个函数保留是为了兼容性，但什么都不做
}

void Delay_ms(uint32_t ms) {
    uint32_t start_tick = GetSystemTick();
    while ((GetSystemTick() - start_tick) < ms) {
        // 空循环等待
    }
}

uint32_t GetSystemTick(void) {
    return g_system_tick;  // 使用SystemTick.c中的全局变量
}