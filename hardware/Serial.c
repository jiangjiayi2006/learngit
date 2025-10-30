#include "serial.h"
#include "pid.h"
#include "encoder.h"
#include "system.h"

#define RX_BUFFER_SIZE 64

static char rx_buffer[RX_BUFFER_SIZE];
static uint8_t rx_index = 0;

// 添加缺失的函数声明
void PID_SetTargetSpeed(uint8_t motor_id, float speed);

void Serial_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    
    // USART1 TX - PA9, RX - PA10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // USART配置
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    
    // 使能串口接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
    // NVIC配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // 使能串口
    USART_Cmd(USART1, ENABLE);
}

void Serial_SendString(char *str)
{
    while (*str) {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
        USART_SendData(USART1, *str++);
    }
}

void Serial_SendData(float *data, uint8_t len)
{
    char buffer[100];
    char temp[20];
    
    buffer[0] = '\0';
    for (int i = 0; i < len; i++) {
        sprintf(temp, "%.2f", data[i]);
        strcat(buffer, temp);
        if (i < len - 1) {
            strcat(buffer, ",");
        }
    }
    strcat(buffer, "\n");
    Serial_SendString(buffer);
}

// 解析串口指令
static void Parse_Command(char *cmd)
{
    if (strncmp(cmd, "@speed%", 7) == 0) {
        float speed = atof(cmd + 7);
        PID_SetTargetSpeed(MOTOR1, speed);
        PID_SetTargetSpeed(MOTOR2, speed);
        
        char response[50];
        sprintf(response, "Set speed to: %.1f\n", speed);
        Serial_SendString(response);
    }
}

// 串口中断服务函数
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        char received_char = USART_ReceiveData(USART1);
        
        if (received_char == '\r' || received_char == '\n') {
            if (rx_index > 0) {
                rx_buffer[rx_index] = '\0';
                Parse_Command(rx_buffer);
                rx_index = 0;
            }
        } else if (rx_index < RX_BUFFER_SIZE - 1) {
            rx_buffer[rx_index++] = received_char;
        }
        
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

// 重定向printf
int fputc(int ch, FILE *f)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    USART_SendData(USART1, (uint8_t)ch);
    return ch;
}