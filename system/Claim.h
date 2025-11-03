#include <stdint.h>
#ifndef __CLAIM_H
#define __CLAIM_H

enum TEST{
    TEST_1, TEST_2
};

extern enum TEST test;

extern volatile int16_t Encoder_Count;

extern volatile int g_oled_clear_request;
extern volatile int g_encoder_init;
extern volatile int g_encoder_deinit;
extern volatile int g_oled_show_oneplus_request;

enum Motor_Mode{
    Motor_Mode_break,
    Motor_Mode_frd_rotation,
    Motor_Mode_rvs_rotation,
    Motor_Mode_stop
};

extern enum Motor_Mode Motor1_Mode;
extern enum Motor_Mode Motor2_Mode;

extern volatile int16_t Motor1_Speed;
extern volatile int16_t Motor2_Speed;

extern volatile int32_t Motor1_Position;
extern volatile int32_t Motor2_Position;

extern volatile float pid_subject;
extern volatile float Target, Actual, Out;
extern volatile float Kp, Ki, Kd;
extern volatile float Error0, Error1, Error2;

extern volatile float OuterTarget, OuterActual, OuterOut;
extern volatile float OuterKp, OuterKi, OuterKd;
extern volatile float OuterError0, OuterError1, OuterError2;

extern volatile float Motor2_Target, Motor2_Actual, Motor2_Out;
extern volatile float Motor2_Kp, Motor2_Ki, Motor2_Kd;
extern volatile float Motor2_Error0, Motor2_Error1, Motor2_Error2;

#endif