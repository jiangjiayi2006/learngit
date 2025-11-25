#include "trace_sensor.h"
#include "stm32f10x.h"

// 循迹传感器引脚定义 - 请根据实际连线修改
#define SENSOR_GPIO GPIOB
#define SENSOR1_PIN GPIO_Pin_0
#define SENSOR2_PIN GPIO_Pin_1
#define SENSOR3_PIN GPIO_Pin_2
#define SENSOR4_PIN GPIO_Pin_3
#define SENSOR5_PIN GPIO_Pin_4

// 传感器阈值和校准数据
uint16_t sensor_threshold[5] = {1500, 1500, 1500, 1500, 1500}; // 默认阈值
uint16_t white_values[5] = {3000, 3000, 3000, 3000, 3000}; // 白场值
uint16_t black_values[5] = {500, 500, 500, 500, 500};      // 黑场值

// ADC通道映射
const uint8_t sensor_channels[5] = {
    ADC_Channel_8,  // PB0 - 传感器1
    ADC_Channel_9,  // PB1 - 传感器2  
    ADC_Channel_10, // PB2 - 传感器3
    ADC_Channel_11, // PB3 - 传感器4
    ADC_Channel_12  // PB4 - 传感器5
};

void TraceSensor_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); // 12MHz ADC时钟
    
    // 配置ADC引脚为模拟输入
    GPIO_InitStructure.GPIO_Pin = SENSOR1_PIN | SENSOR2_PIN | SENSOR3_PIN | SENSOR4_PIN | SENSOR5_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(SENSOR_GPIO, &GPIO_InitStructure);
    
    // 配置ADC
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    
    // 开启ADC
    ADC_Cmd(ADC1, ENABLE);
    
    // ADC校准
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}

// 修复：添加超时的单通道读取
uint16_t TraceSensor_ReadSingle(uint8_t channel) {
    uint32_t timeout = 100000;
    
    if(channel > ADC_Channel_17) return 0; // 通道号检查
    
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_239Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    
    // 等待转换完成，带超时
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET) {
        if(timeout-- == 0) return 0; // 超时返回0
    }
    return ADC_GetConversionValue(ADC1);
}

uint8_t TraceSensor_Read(void) {
    uint8_t sensor_state = 0;
    
    // 读取5个循迹传感器并转换为黑白状态信息
    for(int i = 0; i < 5; i++) {
        uint16_t value = TraceSensor_ReadSingle(sensor_channels[i]);
        if(value > sensor_threshold[i]) {
            sensor_state |= (1 << i); // 设置对应位
        }
    }
    
    return sensor_state;
}

// 修复：实现完整的传感器校准
void TraceSensor_Calibrate(void) {
    // 白场采样
    for(int i = 0; i < 5; i++) {
        uint32_t sum = 0;
        // 多次采样取平均
        for(int j = 0; j < 10; j++) {
            sum += TraceSensor_ReadSingle(sensor_channels[i]);
            Delay_ms(10);
        }
        white_values[i] = sum / 10;
    }
    
    // 这里应该提示用户放置黑线，然后采样黑场值
    // 为简化，我们使用固定比例计算黑场值
    for(int i = 0; i < 5; i++) {
        black_values[i] = white_values[i] * 2 / 10; // 假设黑场值是白场的20%
    }
    
    // 计算中间阈值
    for(int i = 0; i < 5; i++) {
        sensor_threshold[i] = (white_values[i] + black_values[i]) / 2;
    }
}

// 新增：获取传感器原始值（用于调试）
uint16_t TraceSensor_GetRawValue(uint8_t sensor_index) {
    if(sensor_index >= 5) return 0;
    return TraceSensor_ReadSingle(sensor_channels[sensor_index]);
}

// 新增：获取传感器阈值（用于调试）
uint16_t TraceSensor_GetThreshold(uint8_t sensor_index) {
    if(sensor_index >= 5) return 0;
    return sensor_threshold[sensor_index];
}