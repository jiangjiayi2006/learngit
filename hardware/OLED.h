#ifndef __OLED_H
#define __OLED_H

#include "stm32f10x.h"

void OLED_Init(void);
void OLED_Clear(void);
void OLED_Refresh(void);
void OLED_ShowString(uint8_t x, uint8_t y, char *str);
void OLED_ShowChar(uint8_t x, uint8_t y, char chr);
void OLED_ShowMenu(void);

#endif