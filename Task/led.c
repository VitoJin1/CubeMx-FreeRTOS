#include "led.h"
#include "task.h"

void led_task(void *pvParameters){
    //BaseType_t err;
    while(1){
        // err=xTaskNotifyWait((uint32_t)0x00,
        //                     (uint32_t)ULONG_MAX)
        HAL_GPIO_TogglePin(GPIOC,LEDR_Pin);
        vTaskDelay(500);
        HAL_GPIO_TogglePin(GPIOC,LEDG_Pin);
        vTaskDelay(500);
        HAL_GPIO_TogglePin(GPIOC,LEDB_Pin);
        vTaskDelay(500);
        HAL_GPIO_TogglePin(GPIOC,LEDY_Pin);
        vTaskDelay(500);
        printf("hello\r\n");
    }
}
void print_task(void * pvParameters){
    
}