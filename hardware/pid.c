#include "MyHeader.h"

#define Outer_INTEGRAL_SEPARATION_THRESHOLD 200
#define INTEGRAL_SEPARATION_THRESHOLD 200
#define MOTOR2_INTEGRAL_SEPARATION_THRESHOLD 200

void PIDControl(void)
{
    int s_speed_location = 0;
    
    if(test == TEST_1 && ((pid_subject > 0 ? pid_subject : -pid_subject) <= 0.5))
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

        OuterError2 = OuterError1;
        OuterError1 = OuterError0;
        OuterError0 = OuterTarget - OuterActual;
            
        float OuterdeltaP = OuterKp * (OuterError0 - OuterError1);
        float OuterdeltaD = OuterKd * (OuterError0 - 2 * OuterError1 + OuterError2);
        float OuterdeltaI = 0;
    
        if((OuterError0 >= 0 ? OuterError0 : -OuterError0) <= Outer_INTEGRAL_SEPARATION_THRESHOLD)
        {
            OuterdeltaI = OuterKi * OuterError0;
        }
    
        OuterOut += OuterdeltaP + OuterdeltaD + OuterdeltaI;
            
        if((OuterError0 > 0 ? OuterError0 : -OuterError0) < 7) OuterOut = 0;
    
        if (OuterOut > 100) OuterOut = 100;
        if (OuterOut < -100) OuterOut = -100;
        if(OuterTarget == 0 && OuterActual == 0) OuterOut = 0;

        Target = OuterOut;
    }
    
    Actual = Motor1_Speed;
    Error2 = Error1;
    Error1 = Error0;
    Error0 = Target - Actual;

    if((Error0 > 0 ? Error0 : -Error0) > 1)
    {
        float deltaP = Kp * (Error0 - Error1);
        float deltaD = Kd * (Error0 - 2 * Error1 + Error2);
        float deltaI = 0;
        
        if((Error0 >= 0 ? Error0 : -Error0) <= INTEGRAL_SEPARATION_THRESHOLD)
        {
            deltaI = Ki * Error0;
        }
        
        Out += deltaP + deltaI + deltaD;
            
        if (Out > 100) Out = 100;
        if (Out < -100) Out = -100;
        if(Target == 0 && Actual == 0) Out = 0;
    }
    
    if(test == TEST_2)
    {
        Motor2_Actual = Motor2_Speed;
        Motor2_Target = Motor1_Speed;
        
        Motor2_Error2 = Motor2_Error1;
        Motor2_Error1 = Motor2_Error0;
        Motor2_Error0 = Motor2_Target - Motor2_Actual;

        if((Motor2_Error0 > 0 ? Motor2_Error0 : -Motor2_Error0) > 1)
        {
            float deltaP = Motor2_Kp * (Motor2_Error0 - Motor2_Error1);
            float deltaD = Motor2_Kd * (Motor2_Error0 - 2 * Motor2_Error1 + Motor2_Error2);
            float deltaI = 0;
            
            if((Motor2_Error0 >= 0 ? Motor2_Error0 : -Motor2_Error0) <= MOTOR2_INTEGRAL_SEPARATION_THRESHOLD)
            {
                deltaI = Motor2_Ki * Motor2_Error0;
            }
            
            Motor2_Out += deltaP + deltaI + deltaD;
                
            if (Motor2_Out > 100) Motor2_Out = 100;
            if (Motor2_Out < -100) Motor2_Out = -100;
            if(Motor2_Target == 0 && Motor2_Actual == 0) Motor2_Out = 0;
        }
        
        Motor2_SetPWM(Motor2_Out);
    }
    
    Motor_SetPWM(Out);
}