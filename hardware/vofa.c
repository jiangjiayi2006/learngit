#include "vofa.h"
#include "encoder.h"
#include "pid.h"
#include "serial.h"
#include "system.h"

static uint32_t last_send_time = 0;

void VOFA_Init(void)
{
    last_send_time = system_tick;
}

void VOFA_SendData(void)
{
    // 每10ms发送一次数据
    if (system_tick - last_send_time >= 10) {
        last_send_time = system_tick;
        
        float data[4];
        
        if (System_GetCurrentMode() == MODE_SPEED_CONTROL) {
            data[0] = PID_GetTargetSpeed(MOTOR1);     // 目标速度
            data[1] = (float)Encoder_GetSpeed(MOTOR1); // 实际速度1
            data[2] = (float)Encoder_GetSpeed(MOTOR2); // 实际速度2
            data[3] = speed_pid[MOTOR1].output;       // PID输出
        } else {
            data[0] = (float)Encoder_GetTotalCount(MOTOR1); // 电机1位置
            data[1] = (float)Encoder_GetTotalCount(MOTOR2); // 电机2位置
            data[2] = position_pid[MOTOR2].output;          // 位置环输出
            data[3] = 0;
        }
        
        Serial_SendData(data, 4);
    }
}