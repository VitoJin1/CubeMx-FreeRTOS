#include "roller.h"
//roller task without safety lock
void roller_task(void *pvParameters){
    TickType_t PreviousWakeTime;
    PreviousWakeTime =xTaskGetTickCount();
    const TickType_t TimeIncrement=pdMS_TO_TICKS(200);
    for(;;){
        if(RemoteCommand.mode_switch==MANUAL_MODE||RemoteCommand.mode_switch==AUTO_MODE){
            if(RemoteCommand.roller_switch==0)
            __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,18500+(RemoteCommand.Roller_Speed/2));
            else if(RemoteCommand.roller_switch==1){
                __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,18500-(RemoteCommand.roller_speed/2));
            }
        }
        else if(RemoteCommand.mode_switch==CALI_MODE){
            __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,18500);
        }
        vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);
    }
}