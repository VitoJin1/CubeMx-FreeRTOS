#include "pump.h"
void pump_task(void *pvParameters){
    for(;;){
        if(RemoteCommand.mode_switch==MANUAL_MODE||RemoteCommand.mode_switch==AUTO_MODE){
            if(RemoteCommand.pump_switch==1)__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,18250);
            else if(RemoteCommand.pump_switch==0)__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,19000);
        }
        else if(RemoteCommand.mode_switch==CALI_MODE)__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,19000);
        vTaskDelay(500);
    }
}