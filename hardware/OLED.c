#include "oled.h"
#include "stm32f10x.h"
#include "font.h"

// I2C硬件定义 - 请根据实际连线修改
#define OLED_I2C I2C1
#define OLED_ADDRESS 0x78  // 通常为0x78或0x7A
#define I2C_TIMEOUT 100000 // I2C操作超时计数

// OLED命令定义
#define OLED_CMD  0x00
#define OLED_DATA 0x40

uint8_t OLED_GRAM[128][8];  // 显示缓存

// 添加超时机制的I2C写入函数
uint8_t OLED_I2C_WriteByte(uint8_t type, uint8_t data) {
    uint32_t timeout = I2C_TIMEOUT;
    
    // 等待总线空闲，带超时
    while(I2C_GetFlagStatus(OLED_I2C, I2C_FLAG_BUSY)) {
        if(timeout-- == 0) return 0; // 超时返回错误
    }
    
    // 生成START条件
    I2C_GenerateSTART(OLED_I2C, ENABLE);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_MODE_SELECT)) {
        if(timeout-- == 0) return 0;
    }
    
    // 发送地址
    I2C_Send7bitAddress(OLED_I2C, OLED_ADDRESS, I2C_Direction_Transmitter);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
        if(timeout-- == 0) return 0;
    }
    
    // 发送类型(命令/数据)
    I2C_SendData(OLED_I2C, type);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) {
        if(timeout-- == 0) return 0;
    }
    
    // 发送数据
    I2C_SendData(OLED_I2C, data);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
        if(timeout-- == 0) return 0;
    }
    
    I2C_GenerateSTOP(OLED_I2C, ENABLE);
    return 1; // 成功
}

void OLED_WriteCmd(uint8_t cmd) {
    if(!OLED_I2C_WriteByte(OLED_CMD, cmd)) {
        // I2C错误处理 - 可以重置I2C或记录错误
    }
}

void OLED_WriteData(uint8_t data) {
    if(!OLED_I2C_WriteByte(OLED_DATA, data)) {
        // I2C错误处理
    }
}

void OLED_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;
    
    // 开启I2C时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    
    // 配置I2C引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // SCL, SDA
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 配置I2C
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 400000; // 400kHz
    
    I2C_Init(OLED_I2C, &I2C_InitStructure);
    I2C_Cmd(OLED_I2C, ENABLE);
    
    // OLED初始化序列
    OLED_WriteCmd(0xAE); // 关闭显示
    OLED_WriteCmd(0x20); // 内存地址模式
    OLED_WriteCmd(0x10); // 页地址模式
    OLED_WriteCmd(0xB0); // 设置页起始地址
    OLED_WriteCmd(0xC8); // 设置扫描方向
    OLED_WriteCmd(0x00); // 设置列低地址
    OLED_WriteCmd(0x10); // 设置列高地址
    OLED_WriteCmd(0x40); // 设置显示起始行
    OLED_WriteCmd(0x81); // 对比度设置
    OLED_WriteCmd(0xFF); // 对比度值
    OLED_WriteCmd(0xA1); // 设置段重定向
    OLED_WriteCmd(0xA6); // 正常显示
    OLED_WriteCmd(0xA8); // 多路复用率
    OLED_WriteCmd(0x3F); // 1/64 duty
    OLED_WriteCmd(0xA4); // 全部显示开
    OLED_WriteCmd(0xD3); // 设置显示偏移
    OLED_WriteCmd(0x00); // 无偏移
    OLED_WriteCmd(0xD5); // 设置振荡器频率
    OLED_WriteCmd(0x80); // 设置分频比
    OLED_WriteCmd(0xD9); // 设置预充电周期
    OLED_WriteCmd(0xF1); 
    OLED_WriteCmd(0xDA); // 设置COM硬件配置
    OLED_WriteCmd(0x12);
    OLED_WriteCmd(0xDB); // 设置VCOMH
    OLED_WriteCmd(0x40);
    OLED_WriteCmd(0x8D); // 电荷泵设置
    OLED_WriteCmd(0x14);
    OLED_WriteCmd(0xAF); // 开启显示
    
    OLED_Clear();
}

void OLED_Clear(void) {
    for(uint8_t i=0; i<8; i++) {
        for(uint8_t j=0; j<128; j++) {
            OLED_GRAM[j][i] = 0x00;
        }
    }
    OLED_Refresh();
}

void OLED_Refresh(void) {
    for(uint8_t i=0; i<8; i++) {
        OLED_WriteCmd(0xB0 + i); // 设置页地址
        OLED_WriteCmd(0x00);     // 设置列低地址
        OLED_WriteCmd(0x10);     // 设置列高地址
        
        for(uint8_t j=0; j<128; j++) {
            OLED_WriteData(OLED_GRAM[j][i]);
        }
    }
}

// 修复：添加边界检查的字符串显示
void OLED_ShowString(uint8_t x, uint8_t y, char *str) {
    if(y >= 8) return; // 防止页地址越界
    
    while(*str && x < 128) {
        OLED_ShowChar(x, y, *str);
        x += 8;
        if(x > 120 && y < 7) { // 换行检查
            x = 0;
            y++;
        }
        str++;
    }
    OLED_Refresh();
}

// 修复：添加字符范围检查
void OLED_ShowChar(uint8_t x, uint8_t y, char chr) {
    if(x >= 128 || y >= 8) return; // 边界检查
    
    uint8_t c;
    if(chr >= ' ' && chr <= '~') { // 可显示字符范围
        c = chr - ' ';
    } else {
        c = 0; // 不可显示字符显示空格
    }
    
    for(uint8_t i=0; i<8; i++) {
        if(x < 128 && y < 8) {
            OLED_GRAM[x][y] = F8X16[c*16 + i];
        }
        if(x+1 < 128 && y < 8) {
            OLED_GRAM[x+1][y] = F8X16[c*16 + i + 8];
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