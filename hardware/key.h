#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

// °´¼ü×´Ì¬Ã¶¾Ù
typedef enum {
    KEY_RELEASED = 0,
    KEY_PRESSED = 1,
    KEY_JUST_PRESSED = 2,
    KEY_JUST_RELEASED = 3
} KeyState;

void Key_Init(void);
KeyState Key_GetState(void);
uint8_t Key_IsPressed(void);
uint8_t Key_IsJustPressed(void);

#endif