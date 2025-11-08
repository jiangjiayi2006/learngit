#include "MyHeader.h"
#include <math.h>

#define Outer_INTEGRAL_SEPARATION_THRESHOLD 200
#define INTEGRAL_SEPARATION_THRESHOLD 200
#define MOTOR2_INTEGRAL_SEPARATION_THRESHOLD 200

// 定义PID控制器实例
static PID_Controller motor1_pid = {0};
static PID_Controller outer_pid = {0};
static PID_Controller motor2_pid = {0};

// 参数限制函数
float Constrain_Float(float value, float min_val, float max_val)
{
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}

// PID初始化
void PID_Init(void)
{
    // 电机1 PID参数
    motor1_pid.kp = 0.4f;
    motor1_pid.ki = 0.22f;
    motor1_pid.kd = 0.0f;
    motor1_pid.integral_limit = INTEGRAL_SEPARATION_THRESHOLD;
    motor1_pid.output_limit = 100.0f;
    
    // 外环PID参数
    outer_pid.kp = 0.3f;
    outer_pid.ki = 0.0f;
    outer_pid.kd = 0.12f;
    outer_pid.integral_limit = Outer_INTEGRAL_SEPARATION_THRESHOLD;
    outer_pid.output_limit = 100.0f;
    
    // 电机2 PID参数
    motor2_pid.kp = 0.4f;
    motor2_pid.ki = 0.22f;
    motor2_pid.kd = 0.0f;
    motor2_pid.integral_limit = MOTOR2_INTEGRAL_SEPARATION_THRESHOLD;
    motor2_pid.output_limit = 100.0f;
}

// 通用的PID计算函数
static float PID_Calculate(PID_Controller* pid, float target, float actual)
{
    // 更新误差
    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];
    pid->error[0] = target - actual;
    
    pid->target = target;
    pid->actual = actual;
    
    // 如果误差很小，直接返回0
    if(fabsf(pid->error[0]) <= 1.0f)
    {
        return 0.0f;
    }
    
    float deltaP = pid->kp * (pid->error[0] - pid->error[1]);
    float deltaD = pid->kd * (pid->error[0] - 2 * pid->error[1] + pid->error[2]);
    float deltaI = 0;
    
    // 积分分离
    if(fabsf(pid->error[0]) <= pid->integral_limit)
    {
        deltaI = pid->ki * pid->error[0];
    }
    
    pid->output += deltaP + deltaI + deltaD;
    
    // 输出限幅
    pid->output = Constrain_Float(pid->output, -pid->output_limit, pid->output_limit);
    
    // 如果目标和实际都为0，输出为0
    if(target == 0 && actual == 0)
    {
        pid->output = 0;
    }
    
    return pid->output;
}

void PIDControl(void)
{
    int s_speed_location = 0;
    
    if(test == TEST_1 && fabsf(pid_subject) <= 0.5f)
    {
        s_speed_location = 1;
        pid_subject = 0;
        g_oled_show_oneplus_request = 1;
    }
    else 
    {
        Target = pid_subject;
    }
    
    if (test == TEST_2 || s_speed_location == 1)
    {
        OuterActual += Motor1_Speed;
        OuterTarget = Encoder_Count;

        if(s_speed_location == 1) 
        {
            s_speed_location = 0;
            OuterTarget = 0;
        }

        // 使用结构体PID计算外环
        OuterOut = PID_Calculate(&outer_pid, OuterTarget, OuterActual);
        
        // 外环特殊处理
        if(fabsf(outer_pid.error[0]) < 7.0f) 
            OuterOut = 0;
            
        if(OuterTarget == 0 && OuterActual == 0) 
            OuterOut = 0;

        Target = OuterOut;
    }
    
    // 电机1内环PID
    Actual = Motor1_Speed;
    Out = PID_Calculate(&motor1_pid, Target, Actual);
    
    if(test == TEST_2)
    {
        Motor2_Actual = Motor2_Speed;
        Motor2_Target = Motor1_Speed;
        
        // 电机2 PID计算
        Motor2_Out = PID_Calculate(&motor2_pid, Motor2_Target, Motor2_Actual);
        Motor2_SetPWM(Motor2_Out);
    }
    
    Motor_SetPWM(Out);
    
    // 保持全局变量同步（为了兼容现有代码）
    Kp = motor1_pid.kp;
    Ki = motor1_pid.ki;
    Kd = motor1_pid.kd;
    
    OuterKp = outer_pid.kp;
    OuterKi = outer_pid.ki;
    OuterKd = outer_pid.kd;
    
    Motor2_Kp = motor2_pid.kp;
    Motor2_Ki = motor2_pid.ki;
    Motor2_Kd = motor2_pid.kd;
}