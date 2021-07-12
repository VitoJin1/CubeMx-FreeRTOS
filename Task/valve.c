#include "valve.h"

void valve_task(void *pvParameters){
    TickType_t PreviousWakeTime;
    PreviousWakeTime=xTaskGetTickCount();
    const TickType_t TimeIncreasemet=pdMS_TO_TICKS( 500 );
    for(;;){

        if(RemoteCommand.mode_switch==MANUAL_MODE||RemoteCommand.mode_switch==AUTO_MODE){
            if(RemoteCommand.valve_switch==1){
                HAL_GPIO_WritePin(ValveSwitch_GPIO_Port,ValveSwitch_Pin,GPIO_PIN_SET);
            }
            else if( RemoteCommand.mode_switch==0){
                HAL_GPIO_WritePin(ValveSwitch_GPIO_Port,ValveSwitch_Pin,GPIO_PIN_RESET);
            }
        }
        else if( RemoteCommand.mode_switch==CALI_MODE ){
            HAL_GPIO_WritePin(ValveSwitch_GPIO_Port,ValveSwitch_Pin,GPIO_PIN_RESET);
        }
        vTaskDelayUntil(&PreviousWakeTime,TimeIncreasemet);
    }
}
