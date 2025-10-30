#include "pid.h"
#include "encoder.h"
#include "motor.h"
#include "system.h"

PID_TypeDef speed_pid[2];
PID_TypeDef position_pid[2];
static float target_speed[2] = {0, 0};
static float target_position[2] = {0, 0};

void PID_Init(void)
{
    for (int i = 0; i < 2; i++) {
        // 速度环PID参数
        speed_pid[i].Kp = 2.0f;
        speed_pid[i].Ki = 0.5f;
        speed_pid[i].Kd = 0.1f;
        speed_pid[i].integral = 0;
        speed_pid[i].output_max = 1000;
        speed_pid[i].integral_max = 1000;
        
        // 位置环PID参数
        position_pid[i].Kp = 1.0f;
        position_pid[i].Ki = 0.1f;
        position_pid[i].Kd = 0.05f;
        position_pid[i].integral = 0;
        position_pid[i].output_max = 800;
        position_pid[i].integral_max = 500;
    }
}

void PID_Reset(void)
{
    for (int i = 0; i < 2; i++) {
        speed_pid[i].integral = 0;
        speed_pid[i].err_last = 0;
        speed_pid[i].err_prev = 0;
        
        position_pid[i].integral = 0;
        position_pid[i].err_last = 0;
        position_pid[i].err_prev = 0;
    }
}

float PID_Calculate(PID_TypeDef *pid, float target, float actual)
{
    pid->target_val = target;
    pid->actual_val = actual;
    pid->err = target - actual;
    
    // 积分项
    pid->integral += pid->err;
    
    // 积分限幅
    if (pid->integral > pid->integral_max) {
        pid->integral = pid->integral_max;
    } else if (pid->integral < -pid->integral_max) {
        pid->integral = -pid->integral_max;
    }
    
    // 增量式PID计算
    float increment = pid->Kp * (pid->err - pid->err_last) +
                     pid->Ki * pid->err +
                     pid->Kd * (pid->err - 2 * pid->err_last + pid->err_prev);
    
    pid->output += increment;
    
    // 输出限幅
    if (pid->output > pid->output_max) {
        pid->output = pid->output_max;
    } else if (pid->output < -pid->output_max) {
        pid->output = -pid->output_max;
    }
    
    // 更新误差
    pid->err_prev = pid->err_last;
    pid->err_last = pid->err;
    
    return pid->output;
}

void PID_SetParams(uint8_t motor_id, float kp, float ki, float kd)
{
    if (System_GetCurrentMode() == MODE_SPEED_CONTROL) {
        speed_pid[motor_id].Kp = kp;
        speed_pid[motor_id].Ki = ki;
        speed_pid[motor_id].Kd = kd;
    } else {
        position_pid[motor_id].Kp = kp;
        position_pid[motor_id].Ki = ki;
        position_pid[motor_id].Kd = kd;
    }
}

// 在10ms中断中调用
void PID_Update(void)
{
    SystemMode_t mode = System_GetCurrentMode();
    
    if (mode == MODE_SPEED_CONTROL) {
        // 速度环控制
        for (int i = 0; i < 2; i++) {
            float actual_speed = (float)Encoder_GetSpeed(i);
            float output = PID_Calculate(&speed_pid[i], target_speed[i], actual_speed);
            Motor_SetSpeed(i, (int16_t)output);
        }
    } else {
        // 位置环控制 - 电机2跟随电机1
        float motor1_position = (float)Encoder_GetTotalCount(MOTOR1);
        float motor2_position = (float)Encoder_GetTotalCount(MOTOR2);
        
        // 电机2跟随电机1的位置
        float output = PID_Calculate(&position_pid[MOTOR2], motor1_position, motor2_position);
        Motor_SetSpeed(MOTOR2, (int16_t)output);
        
        // 电机1保持自由（或轻微阻尼）
        Motor_SetSpeed(MOTOR1, 0);
    }
}

// 设置目标速度
void PID_SetTargetSpeed(uint8_t motor_id, float speed)
{
    target_speed[motor_id] = speed;
}

// 获取目标速度
float PID_GetTargetSpeed(uint8_t motor_id)
{
    return target_speed[motor_id];
}