#include "trace_algorithm.h"
#include "motor.h"
#include "trace_sensor.h"
#include "delay.h"
#include "SystemTick.h"  // 添加SystemTick头文件
#include "stm32f10x.h"

// PID参数 - 添加volatile修饰
volatile float KP = 0.8f;
volatile float KI = 0.0f;
volatile float KD = 0.2f;

// PID状态变量 - 添加volatile修饰
volatile int16_t last_error = 0;
volatile int16_t integral = 0;
#define INTEGRAL_LIMIT 1000  // 积分限幅

// 路径状态 - 添加volatile修饰
volatile PathType current_path = PATH_STRAIGHT;
volatile uint8_t cross_count = 0;
volatile uint32_t last_cross_time = 0;
#define CROSS_DEBOUNCE_MS 500  // 十字路口去抖动时间

// 传感器状态定义（修正后的定义）
#define SENSOR_ALL_WHITE    0x00  // 00000
#define SENSOR_MIDDLE       0x04  // 00100 - 中间传感器
#define SENSOR_SLIGHT_RIGHT 0x02  // 00010 - 轻微右偏
#define SENSOR_HARD_RIGHT   0x01  // 00001 - 严重右偏
#define SENSOR_RIGHT        0x03  // 00011 - 右偏
#define SENSOR_SLIGHT_LEFT  0x08  // 01000 - 轻微左偏
#define SENSOR_HARD_LEFT    0x10  // 10000 - 严重左偏
#define SENSOR_LEFT         0x18  // 11000 - 左偏
#define SENSOR_ALL_BLACK    0x1F  // 11111 - 全黑(十字路口)
#define SENSOR_CROSS        0x0E  // 01110 - 十字路口
#define SENSOR_RIGHT_CURVE  0x06  // 00110 - 右弯
#define SENSOR_LEFT_CURVE   0x0C  // 01100 - 左弯

void Trace_Algorithm(void) {
    uint8_t sensor_state = TraceSensor_Read();
    int16_t error = 0;
    int16_t base_speed = 600;
    int16_t adjustment = 0;
    uint32_t current_time = Get_SystemTick();  // 修改这里
    
    // 根据传感器状态计算误差
    switch(sensor_state) {
        case SENSOR_ALL_WHITE: // 全部白线 - 脱轨
            // 尝试根据上次误差恢复
            adjustment = PID_Calculate(last_error * 2);
            current_path = PATH_LOST;
            break;
            
        case SENSOR_MIDDLE: // 00100 - 直线
            error = 0;
            current_path = PATH_STRAIGHT;
            break;
            
        case SENSOR_SLIGHT_RIGHT: // 00010 - 轻微右偏
            error = -1;
            current_path = PATH_CURVE;
            break;
        case SENSOR_HARD_RIGHT: // 00001 - 严重右偏
            error = -2;
            current_path = PATH_CURVE;
            break;
        case SENSOR_RIGHT: // 00011 - 右偏
            error = -3;
            current_path = PATH_CURVE;
            break;
            
        case SENSOR_SLIGHT_LEFT: // 01000 - 轻微左偏
            error = 1;
            current_path = PATH_CURVE;
            break;
        case SENSOR_HARD_LEFT: // 10000 - 严重左偏
            error = 2;
            current_path = PATH_CURVE;
            break;
        case SENSOR_LEFT: // 11000 - 左偏
            error = 3;
            current_path = PATH_CURVE;
            break;
            
        case SENSOR_ALL_BLACK: // 11111 - 十字路口
        case SENSOR_CROSS: // 01110 - 十字路口
            // 添加去抖动，防止重复计数
            if(current_time - last_cross_time > CROSS_DEBOUNCE_MS) {
                cross_count++;
                last_cross_time = current_time;
            }
            current_path = PATH_CROSS;
            // 非阻塞方式处理十字路口
            base_speed = 800; // 十字路口稍快通过
            error = 0; // 直行通过
            break;
            
        case SENSOR_RIGHT_CURVE: // 00110 - 右弯
            error = -2;
            current_path = PATH_CURVE;
            break;
        case SENSOR_LEFT_CURVE: // 01100 - 左弯
            error = 2;
            current_path = PATH_CURVE;
            break;
            
        default:
            // 其他情况使用上次误差的一半
            error = last_error / 2;
            current_path = PATH_CURVE;
            break;
    }
    
    // PID控制计算
    adjustment = PID_Calculate(error);
    
    // 根据路径类型动态调整基础速度
    switch(current_path) {
        case PATH_STRAIGHT:
            base_speed = 800; // 直道加速
            break;
        case PATH_CURVE:
            base_speed = 600; // 弯道减速
            break;
        case PATH_CROSS:
            base_speed = 700; // 路口中等速度
            break;
        case PATH_LOST:
            base_speed = 300; // 脱轨慢速寻找
            break;
    }
    
    // 设置电机速度
    Motor_SetSpeed(base_speed - adjustment, base_speed + adjustment);
}

// 修复：添加积分限幅的PID计算
int16_t PID_Calculate(int16_t error) {
    // 积分项计算，带限幅
    integral += error;
    if(integral > INTEGRAL_LIMIT) integral = INTEGRAL_LIMIT;
    if(integral < -INTEGRAL_LIMIT) integral = -INTEGRAL_LIMIT;
    
    int16_t derivative = error - last_error;
    
    int16_t output = (int16_t)(KP * error + KI * integral + KD * derivative);
    
    last_error = error;
    
    // 限制输出范围
    if(output > 400) output = 400;
    if(output < -400) output = -400;
    
    return output;
}

PathType GetCurrentPathType(void) {
    return current_path;
}

uint8_t GetCrossCount(void) {
    return cross_count;
}

void ResetCrossCount(void) {
    cross_count = 0;
    last_cross_time = 0;
}

void SetPIDParams(float kp, float ki, float kd) {
    KP = kp;
    KI = ki;
    KD = kd;
    integral = 0; // 重置积分项
    last_error = 0;
}