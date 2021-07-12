#include "putter.h"
void putter_task(void *pvParamters){
    for(;;){
        if(RemoteCommand.mode_switch==MANUAL_MODE||RemoteCommand.mode_switch==AUTO_MODE){
            if(RemoteCommand.screw_switch==2){
                HAL_GPIO_WritePin(PutterIn1_GPIO_Port,PutterIn1_Pin,GPIO_PIN_SET);
                vTaskDelay(5);
                HAL_GPIO_WritePin(PutterIn2_GPIO_Port,PutterIn2_Pin,GPIO_PIN_RESET);
            }
            else if(RemoteCommand.screw_switch==1){
                HAL_GPIO_WritePin(PutterIn1_GPIO_Port,PutterIn1_Pin,GPIO_PIN_RESET);
                vTaskDelay(5);
                HAL_GPIO_WritePin(PutterIn2_GPIO_Port,PutterIn2_Pin,GPIO_PIN_SET);
            }
            else{
                HAL_GPIO_WritePin(PutterIn1_GPIO_Port,PutterIn1_Pin,GPIO_PIN_RESET);
                vTaskDelay(5);
                HAL_GPIO_WritePin(PutterIn2_GPIO_Port,PutterIn2_Pin,GPIO_PIN_RESET);
            }
        }
        else if(RemoteCommand.mode_switch==CALI_MODE){
            HAL_GPIO_WritePin(PutterIn1_GPIO_Port,PutterIn1_Pin,GPIO_PIN_RESET);
            vTaskDelay(5);
            HAL_GPIO_WritePin(PutterIn2_GPIO_Port,PutterIn2_Pin,GPIO_PIN_RESET);
        }
        vTaskDelay(500);
    }
}