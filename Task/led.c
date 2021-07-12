#include "led.h"
#include "task.h"

void led_task(void *pvParameters){
    //BaseType_t err;
    for(;;){
        // err=xTaskNotifyWait((uint32_t)0x00,
        //                     (uint32_t)ULONG_MAX)
        if(LoseLoraSignalFlag==0){
            HAL_GPIO_TogglePin(LEDB_GPIO_Port,LEDB_Pin);
            HAL_GPIO_WritePin(LEDR_GPIO_Port,LEDR_Pin,GPIO_PIN_SET);
            HAL_GPIO_WritePin(LEDG_GPIO_Port,LEDG_Pin,GPIO_PIN_SET);
            HAL_GPIO_WritePin(LEDY_GPIO_Port,LEDY_Pin,GPIO_PIN_SET);
        }
        if(LoseLoraSignalFlag==1){
            HAL_GPIO_WritePin(LEDR_GPIO_Port,LEDR_Pin,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LEDG_GPIO_Port,LEDG_Pin,GPIO_PIN_SET);
            HAL_GPIO_WritePin(LEDB_GPIO_Port,LEDB_Pin,GPIO_PIN_SET);
            HAL_GPIO_WritePin(LEDY_GPIO_Port,LEDY_Pin,GPIO_PIN_SET);
        }
        vTaskDelay(500);

    }
}                                                                                                            
void print_task(void * pvParameters){
    
}