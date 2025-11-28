#include "trace_algorithm.h"
#include "motor.h"
#include "trace_sensor.h"
#include "SystemTick.h"

// 优化后的PID参数
volatile float KP = 1.8f;
volatile float KI = 0.03f;    
volatile float KD = 0.6f;

// PID状态变量
volatile int16_t last_error = 0;
volatile int16_t integral = 0;
volatile PathType current_path = PATH_STRAIGHT;

void ResetPID(void) {
    last_error = 0;
    integral = 0;
}

void Trace_Algorithm(void) {
    uint8_t sensor_state = TraceSensor_Read();
    int16_t error = 0;
    int16_t base_speed = 450;
    
    // 传感器状态处理
    switch(sensor_state) {
        case 0x00: // 00000 - 全部白线，脱轨
            error = last_error > 0 ? 4 : -4;
            current_path = PATH_LOST;
            base_speed = 250;
            break;
            
        case 0x04: // 00100 - 中间传感器在线上
            error = 0;
            current_path = PATH_STRAIGHT;
            base_speed = 550;
            break;
            
        case 0x02: // 00010 - 偏右
        case 0x06: // 00110
            error = -1;
            current_path = PATH_CURVE;
            break;
            
        case 0x01: // 00001 - 偏右很多
        case 0x03: // 00011
        case 0x07: // 00111
            error = -3;
            current_path = PATH_CURVE;
            break;
            
        case 0x08: // 01000 - 偏左
        case 0x0C: // 01100
            error = 1;
            current_path = PATH_CURVE;
            break;
            
        case 0x10: // 10000 - 偏左很多
        case 0x18: // 11000  
        case 0x1C: // 11100
            error = 3;
            current_path = PATH_CURVE;
            break;
            
        case 0x1F: // 11111 - 全部黑线，十字路口
        case 0x0E: // 01110 - 十字路口
            error = 0;
            current_path = PATH_CROSS;
            base_speed = 500;
            break;
            
        default:
            if(sensor_state & 0x03) error = -2;
            else if(sensor_state & 0x18) error = 2;
            else error = 0;
            current_path = PATH_CURVE;
            break;
    }
    
    // PID计算
    integral += error;
    if(integral > 400) integral = 400;
    if(integral < -400) integral = -400;
    
    int16_t derivative = error - last_error;
    int16_t adjustment = (int16_t)(KP * error + KI * integral + KD * derivative);
    last_error = error;
    
    // 限制调整量
    if(adjustment > 280) adjustment = 280;
    if(adjustment < -280) adjustment = -280;
    
    // 设置电机速度
    int16_t left_speed = base_speed - adjustment;
    int16_t right_speed = base_speed + adjustment;
    
    // 限制速度范围并确保最小速度
    left_speed = (left_speed < 150) ? 150 : (left_speed > 800 ? 800 : left_speed);
    right_speed = (right_speed < 150) ? 150 : (right_speed > 800 ? 800 : right_speed);
    
    Motor_SetSpeed(left_speed, right_speed);
}

int16_t PID_Calculate(int16_t error) {
    return (int16_t)(KP * error);
}

PathType GetCurrentPathType(void) {
    return current_path;
}

uint8_t GetCrossCount(void) {
    return 0;
}

void ResetCrossCount(void) {
}

void SetPIDParams(float kp, float ki, float kd) {
    KP = kp;
    KI = ki;
    KD = kd;
}