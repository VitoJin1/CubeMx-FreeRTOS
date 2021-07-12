#ifndef __REMOTER_H
#define __REMOTER_H
#include "main.h"
#include "stdint.h"
#include "FreeRTOS.h"
#include "usart.h"
#include "string.h"
#include "task.h"
#include "common.h"
#define UART3_DMA_RECEIVE_BUFFER 44
void lora_receive_task(void *pvParameters);
void LORADataDecode(uint8_t *pdata);
float fRamp_for_LORA(int16_t temp);
short sRamp_for_LORA(int16_t temp);
int iPwm_for_LORA(int16_t temp);
void LOTADataClear(void);
typedef struct RemoteStructure{
    int16_t ch0;
    int16_t ch1;
    int16_t ch2;
    int16_t ch3;

    int16_t max_speed; 
    int16_t roller_speed;
    int16_t prop_speed;

    uint8_t valve_switch;
    uint8_t balance_switch;
    uint8_t acro_switch;
    uint8_t pump_switch;
    uint8_t mode_switch;
    uint8_t roller_switch;
    uint8_t screw_switch;
    uint8_t pose_switch;

    /*joystick control value (4)*/
    float Left_X_Speed;
    float Left_Y_Speed;
    float Right_X_Speed;
    float Right_Y_Speed;
    
    /*pinometer control value, 4 total, 2 listed */
    int Roller_Speed;
    int Left_Prop_Speed;
    int Right_Prop_Speed;
    int Max_Speed;
   /*switch control value(14) */
    
    // short Roller_Switch;
    // short Solenoid_Switch;
    // short Balance_Switch;
    // short Mode;
    // short Arm;
    // short Pose;
    // short ACRO_Switch;
    // short Pump_Switch;
}Remoter;
extern Remoter RemoteCommand;
extern uint8_t Request_lora_flag;
extern uint8_t LoseLoraSignalFlag;
#endif // !__REMOTER_H
