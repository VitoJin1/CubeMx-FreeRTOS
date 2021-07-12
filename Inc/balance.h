#ifndef __BALANCE_H
#define __BALANCE_H
#include "main.h"
#include "common.h"
#include "remoter.h"
#include "imu.h"
#include "math.h"
#include "tim.h"
void inner_loop(void *pvParameters);
void outer_loop(void *pvParameters);
void manual_cali(void *pvParameters);
void balance_check(void * pvParameters);
void propeller_control(void * pvParameters);
typedef struct PID_Regulator
{
    float ref;
    float fdb;
    float err[2];
    float kp;
    float ki;
    float kd;
    float componentP;
    float componentI;
    float componentD;
    float componentPMax;
    float componentIMax;
    float componentDMax;
    float output;
    float outputMax;
    float outputLast;
    // void (*Calc)( struct PID_Regulator * pid);
    // void (*Clear)( struct PID_Reglator  * pid);
    void (*Calc)(struct PID_Regulator * pid);
    void (*Clear)(struct PID_Regulator * pid);
}PID_Regulator, * pPID_Regulator;
typedef struct{
    float Vi;
    float Vo_last;
    float Vo;
    float Fcutoff;
    float Fs;
}LPF_1orderRC_F;
void PIDInnerCalc(pPID_Regulator pid );
void PIDOuterCalc(pPID_Regulator pid );
void PIDInnerClear(pPID_Regulator pid);
void PIDOuterClear(pPID_Regulator pid);
#define PROP_ANGLE_OUTPUT_MAX 3
#define PROP_GYRO_OUTPUT_MAX 200
#define PROP_GYRO_FEEDFORD_PARAMETER 200

//Inner loop
#define PROP_GYRO_LOOP_PID \
{\
    0,0,{0,0},\
    400,0,200,\
    0,0,0,\
    200,0,200,\
    0,PROP_GYRO_OUTPUT_MAX,0,&PIDInnerCalc,&PIDInnerClear}

//Outer loop
#define PROP_ANGLE_LOOP_PID \
{\
    0,0,{0,0},\
    1.0,0,0.0,\
    0,0,0,\
    0.7,0,0.7,\
    0,PROP_ANGLE_OUTPUT_MAX,0,&PIDOuterCalc,&PIDOuterClear}

void  LPF_1orderRC_F_init(LPF_1orderRC_F *v);
float LPF_1orderRC_F_FUNC(LPF_1orderRC_F *v);





#endif // !__BALANCE_H
