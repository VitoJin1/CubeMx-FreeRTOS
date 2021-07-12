#include "balance.h"
LPF_1orderRC_F Prop_PID_Output_Filter;//not inited
PID_Regulator PROP_OUTER_PID=PROP_ANGLE_LOOP_PID;
PID_Regulator PROP_INNER_PID=PROP_GYRO_LOOP_PID;
float Z_Euler_Reference=0.0;
static float Z_Outer_Zero_Cnt=0;
float Z_Outer_Last_data=0;
static float IMU_Shift=0.0;
uint8_t Balance_Stable_Flag=0;



/*
* ManualBalancePressed was created to give outer loop refference when 
* siwthing acro from enable to disable
*/
void inner_loop(void *pvParameters){
    TickType_t PreviousWakeTime;
    PreviousWakeTime =xTaskGetTickCount();
    const TickType_t TimeIncrement=pdMS_TO_TICKS(50);
    static uint8_t ManualBalancePressed=0;
    for(;;){
        if(Balance_Stable_Flag==1){
            if(RemoteCommand.acro_switch==0){
                if(ManualBalancePressed==1){
                    Z_Euler_Reference=Z_Euler_Angle;
                    PROP_OUTER_PID.output=0.0;
                    ManualBalancePressed=0;
                }
                PROP_INNER_PID.ref=PROP_OUTER_PID.output;
            }
            else if(RemoteCommand.acro_switch==1){
                PROP_INNER_PID.ref=RemoteCommand.Left_X_Speed*(-0.5f);
                if(ManualBalancePressed==0)ManualBalancePressed=1;
            }
            PROP_INNER_PID.fdb=Z_Gyro;
            PROP_INNER_PID.Calc(&PROP_INNER_PID);
            
        }
    vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);
    }
    
}

void outer_loop(void *pvParameters){
    TickType_t PreviousWakeTime;
    PreviousWakeTime=xTaskGetTickCount();
    const TickType_t TimeIncrement=pdMS_TO_TICKS(50);
    for(;;){
        if(Balance_Stable_Flag==1){
            if(Z_Euler_Angle-Z_Outer_Last_data< -PI){
                Z_Outer_Zero_Cnt++;
            }
            else if(Z_Euler_Angle-Z_Outer_Last_data > PI){
                Z_Outer_Zero_Cnt--;
            }
            PROP_OUTER_PID.fdb=2*PI*(float)Z_Outer_Zero_Cnt+Z_Euler_Angle;
            Z_Outer_Last_data=Z_Euler_Angle;
            PROP_OUTER_PID.ref=Z_Euler_Angle-IMU_Shift;
            PROP_OUTER_PID.Calc(&PROP_OUTER_PID);
        }
    vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);
    }
}

void manual_cali(void *pvParameters){
    TickType_t PreviousWakeTime;
    PreviousWakeTime =xTaskGetTickCount();
    const TickType_t TimeIncrement=pdMS_TO_TICKS(100);
    for(;;){
        if(Balance_Stable_Flag==1){
            if(RemoteCommand.Left_X_Speed!=0){
                if(RemoteCommand.mode_switch==AUTO_MODE)
                IMU_Shift=IMU_Shift+0.02f*RemoteCommand.Left_X_Speed;
                else if(RemoteCommand.mode_switch==CALI_MODE||RemoteCommand.mode_switch==MANUAL_MODE)
                IMU_Shift=0.0;
            }
        }
        else if (Balance_Stable_Flag==0)IMU_Shift=0.0;
        vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);
    }
}

//modify 0/1 semaphore to imrove 
void balance_check(void * pvParameters){
    uint8_t BalanceFlag=0;
    uint8_t BalanceStableCnt=0;
    float Z_Eulter_Last_data=0.0;
    TickType_t PreviousWakeTime;
    PreviousWakeTime =xTaskGetTickCount();
    const TickType_t TimeIncrement=pdMS_TO_TICKS(100);
    for(;;){
        BalanceFlag=RemoteCommand.balance_switch;
       if(Balance_Stable_Flag==0){
           if(BalanceFlag==1){
               Z_Euler_Reference=Z_Euler_Angle;
               if(ABS(Z_Euler_Reference-Z_Eulter_Last_data)<0.1)
                    BalanceStableCnt++;
                else BalanceStableCnt = 0;
                if(BalanceStableCnt>=20)Balance_Stable_Flag=1;
                Z_Eulter_Last_data=Z_Euler_Reference;
                LPF_1orderRC_F_init(&Prop_PID_Output_Filter);
           }
           else if(BalanceFlag==0){
               //PIDOuterClear(&PROP_OUTER_PID);
               PROP_OUTER_PID.Clear(&PROP_OUTER_PID);
               PROP_INNER_PID.output=0;
               PROP_OUTER_PID.output=0;
               BalanceStableCnt=0;
               IMU_Shift=0.0;
           }
       }
       else if(Balance_Stable_Flag==1){
           if(BalanceFlag==0){
               //PIDOuterClear(&PROP_OUTER_PID);
               PROP_OUTER_PID.Clear(&PROP_OUTER_PID);
               Balance_Stable_Flag=0;
           }
       }
    vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);
    }
    
}

void propeller_control(void * pvParameters){
    static int16_t prop_speed_left =0;
    static int16_t prop_speed_right=0;
    static int16_t prop_pid_left   =0;
    static int16_t prop_pid_right  =0;
    for(;;){
        if(Balance_Stable_Flag==1){   
            prop_pid_left=200-PROP_INNER_PID.output;
            LIMIT_MIN_MAX(prop_pid_left,0,400);
            prop_pid_right=200+PROP_INNER_PID.output;
            LIMIT_MIN_MAX(prop_pid_right,0,400);
        }
        else{
            prop_pid_right =0;
            prop_pid_left  =0;
        }
        if(RemoteCommand.mode_switch==MANUAL_MODE||RemoteCommand.mode_switch==AUTO_MODE){
            if( Balance_Stable_Flag==0 ){
                prop_speed_left  = RemoteCommand.Left_Prop_Speed;
                prop_speed_right = RemoteCommand.Right_Prop_Speed;
            }
            else if( Balance_Stable_Flag==1 ){
                RemoteCommand.Right_Prop_Speed=RemoteCommand.Right_Prop_Speed-200;
                RemoteCommand.Left_Prop_Speed =RemoteCommand.Left_Prop_Speed -200;
                prop_speed_left  = LIMIT_MIN_MAX ( RemoteCommand.Left_Prop_Speed  , 0 , 550 );
                prop_speed_right = LIMIT_MIN_MAX ( RemoteCommand.Right_Prop_Speed , 0 , 550 );
                prop_speed_left  = prop_speed_left + prop_pid_left;
                prop_speed_right = prop_speed_left + prop_pid_right;
            }
            __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,19000-prop_speed_left);
            __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,19000-prop_speed_right);    
        }
        else if(RemoteCommand.mode_switch==CALI_MODE){
            PIDOuterClear(&PROP_OUTER_PID);
            __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,19000);
            __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,19000);
        }
    }
}

void PIDInnerCalc(pPID_Regulator pid){
    pid->err[1]               = pid->ref-pid->fdb;
    pid->componentP           = pid->kp*pid->err[1];
    pid->componentI           = pid->componentI+pid->err[1];
    pid->componentD           = pid->kd*(pid->err[1]-pid->err[0]);
    Prop_PID_Output_Filter.Vi = pid->componentP+pid->componentI+pid->componentD+pid->ref*PROP_GYRO_FEEDFORD_PARAMETER;
    pid->output               = LPF_1orderRC_F_FUNC(&Prop_PID_Output_Filter);
    pid->outputLast           = pid->output;
    pid->err[0]               = pid->err[1];
    
}

void PIDOuterCalc(pPID_Regulator pid){
    pid->err[1]               = pid->ref-pid->fdb;
    if(ABS(pid->err[1])<0.01f){
    pid->err[0]               = pid->err[1];
        return;
    }
    pid->componentP           = pid->kp*pid->err[1];
    pid->componentI           = pid->ki*pid->err[1];
    pid->componentD           = pid->kd*(pid->err[1]-pid->err[0]);
    pid->output               = pid->componentP+pid->componentI+pid->componentD;
    LIMIT_MIN_MAX(pid->output, -pid->outputMax, pid->outputMax);
    pid->outputLast           = pid->output;
    pid->err[0]               = pid->err[1];
}
void PIDInnerClear(pPID_Regulator pid){
    pid->fdb=0.0f;
}

void PIDOuterClear(pPID_Regulator pid){
    pid->fdb=0.0f;
    Z_Outer_Last_data=0;
    Z_Outer_Zero_Cnt=0;
}

void LPF_1orderRC_F_init(LPF_1orderRC_F *v)
{
    v->Vi=0.0f;
    v->Vo_last=0.0f;
    v->Vo=0.0f;
    v->Fcutoff=30;
    v->Fs=100;
}

float LPF_1orderRC_F_FUNC(LPF_1orderRC_F *v)
{
    float RC, Cof1,Cof2;
    RC=(float)1.0f/2.0f/PI/v->Fcutoff;
    Cof1=1/(1+RC*v->Fs);
    Cof2=RC*v->Fs/(1+RC*v->Fs);
    v->Vo=Cof1*v->Vi+Cof2*v->Vo_last;
    v->Vo_last=v->Vo;
    return v->Vo;
}

