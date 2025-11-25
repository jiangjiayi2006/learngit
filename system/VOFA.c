#include "VOFA.h"

void VOFA_SendMotorData(void)
{
    float data[8] = {0};
    
    // 组织数据，并限制数据范围
    data[0] = Constrain_Float(Target, -1000.0f, 1000.0f);              // 目标速度
    data[1] = Constrain_Float((float)Motor1_Speed, -1000.0f, 1000.0f); // 实际速度
    data[2] = Constrain_Float(Out, -100.0f, 100.0f);                   // PID输出
    data[3] = Constrain_Float((float)Motor2_Speed, -1000.0f, 1000.0f); // 电机2速度
    data[4] = Constrain_Float(OuterTarget, -1000.0f, 1000.0f);         // 外环目标
    data[5] = Constrain_Float(OuterActual, -1000.0f, 1000.0f);         // 外环实际
    data[6] = Constrain_Float(OuterOut, -100.0f, 100.0f);              // 外环输出
    data[7] = Constrain_Float((float)Encoder_Get(), -2000.0f, 2000.0f); // 编码器值
    
    Serial_SendJustFloat(data, 8);
}