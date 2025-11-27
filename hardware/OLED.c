#include "oled.h"
#include "stm32f10x.h"
#include "font.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "delay.h"

// 使用硬件I2C
#define OLED_I2C I2C2
#define OLED_ADDRESS 0x78

uint8_t OLED_GRAM[128][8];

// I2C写入函数
uint8_t OLED_I2C_WriteByte(uint8_t type, uint8_t data) {
    uint32_t timeout = 100000;
    
    // 等待总线空闲，带超时
    while(I2C_GetFlagStatus(OLED_I2C, I2C_FLAG_BUSY)) {
        if(timeout-- == 0) return 0;
    }
    
    // 生成START条件
    I2C_GenerateSTART(OLED_I2C, ENABLE);
    timeout = 100000;
    while(!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_MODE_SELECT)) {
        if(timeout-- == 0) return 0;
    }
    
    // 发送地址
    I2C_Send7bitAddress(OLED_I2C, OLED_ADDRESS, I2C_Direction_Transmitter);
    timeout = 100000;
    while(!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
        if(timeout-- == 0) return 0;
    }
    
    // 发送类型(命令/数据)
    I2C_SendData(OLED_I2C, type);
    timeout = 100000;
    while(!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) {
        if(timeout-- == 0) return 0;
    }
    
    // 发送数据
    I2C_SendData(OLED_I2C, data);
    timeout = 100000;
    while(!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
        if(timeout-- == 0) return 0;
    }
    
    I2C_GenerateSTOP(OLED_I2C, ENABLE);
    return 1;
}

void OLED_WriteCmd(uint8_t cmd) {
    OLED_I2C_WriteByte(0x00, cmd);
}

void OLED_WriteData(uint8_t data) {
    OLED_I2C_WriteByte(0x40, data);
}

void OLED_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;
    
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    
    // 配置I2C引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 配置I2C - 400kHz
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 400000;
    
    I2C_Init(OLED_I2C, &I2C_InitStructure);
    I2C_Cmd(OLED_I2C, ENABLE);
    
    Delay_ms(100);
    
    // SSD1306初始化序列
    OLED_WriteCmd(0xAE); // Display Off
    OLED_WriteCmd(0x20); // Memory Addressing Mode
    OLED_WriteCmd(0x00); // Horizontal Addressing Mode
    OLED_WriteCmd(0xB0); // Set Page Start Address
    OLED_WriteCmd(0xC8); // Set COM Output Scan Direction
    OLED_WriteCmd(0x00); // Set Low Column Address
    OLED_WriteCmd(0x10); // Set High Column Address
    OLED_WriteCmd(0x40); // Set Start Line Address
    OLED_WriteCmd(0x81); // Contrast Control
    OLED_WriteCmd(0xFF); // Contrast Value
    OLED_WriteCmd(0xA1); // Set Segment Re-map
    OLED_WriteCmd(0xA6); // Set Normal Display
    OLED_WriteCmd(0xA8); // Set Multiplex Ratio
    OLED_WriteCmd(0x3F); // 1/64 Duty
    OLED_WriteCmd(0xA4); // Entire Display ON
    OLED_WriteCmd(0xD3); // Set Display Offset
    OLED_WriteCmd(0x00); // No Offset
    OLED_WriteCmd(0xD5); // Set Display Clock Divide Ratio/Oscillator Frequency
    OLED_WriteCmd(0x80); // Set Divide Ratio
    OLED_WriteCmd(0xD9); // Set Pre-charge Period
    OLED_WriteCmd(0xF1); 
    OLED_WriteCmd(0xDA); // Set COM Pins Hardware Configuration
    OLED_WriteCmd(0x12);
    OLED_WriteCmd(0xDB); // Set VCOMH Deselect Level
    OLED_WriteCmd(0x40);
    OLED_WriteCmd(0x8D); // Charge Pump Setting
    OLED_WriteCmd(0x14); // Enable Charge Pump
    OLED_WriteCmd(0xAF); // Display On
    
    Delay_ms(100);
    OLED_Clear();
}

// 快速刷新函数
void OLED_Refresh_Fast(void) {
    uint8_t page, col;
    
    for(page = 0; page < 8; page++) {
        OLED_WriteCmd(0xB0 + page); // 设置页地址
        OLED_WriteCmd(0x00);        // 设置列低地址
        OLED_WriteCmd(0x10);        // 设置列高地址
        
        for(col = 0; col < 128; col++) {
            OLED_WriteData(OLED_GRAM[col][page]);
        }
    }
}

// 普通刷新函数
void OLED_Refresh(void) {
    OLED_Refresh_Fast();
}

// 清屏函数
void OLED_Clear(void) {
    uint8_t page, col;
    
    // 清空显存
    for(page = 0; page < 8; page++) {
        for(col = 0; col < 128; col++) {
            OLED_GRAM[col][page] = 0x00;
        }
    }
    // 刷新显示
    OLED_Refresh_Fast();
}

void OLED_ShowString(uint8_t x, uint8_t y, char *str) {
    if(y >= 8) return;
    
    while(*str && x < 128) {
        OLED_ShowChar(x, y, *str);
        x += 8;
        if(x > 120 && y < 7) {
            x = 0;
            y++;
        }
        str++;
    }
    OLED_Refresh();
}

void OLED_ShowChar(uint8_t x, uint8_t y, char chr) {
    if(x >= 128 || y >= 8) return;
    
    uint8_t c;
    if(chr >= ' ' && chr <= '~') {
        c = chr - ' ';
    } else {
        c = 0;
    }
    
    uint8_t i;
    for(i = 0; i < 8; i++) {
        if(x < 128 && y < 8) {
            OLED_GRAM[x][y] = F8X16[c * 16 + i];
        }
        if(x + 1 < 128 && y < 8) {
            OLED_GRAM[x + 1][y] = F8X16[c * 16 + i + 8];
        }
    }
}

void OLED_ShowMenu(void) {
    OLED_Clear();
    OLED_ShowString(0, 0, "Car Control Menu");
    OLED_ShowString(0, 2, "1.Trace Mode");
    OLED_ShowString(0, 3, "2.Settings");
    OLED_ShowString(0, 4, "3.Calibration");
    OLED_ShowString(0, 5, "4.Exit");
    OLED_ShowString(0, 6, "Select: ");
}