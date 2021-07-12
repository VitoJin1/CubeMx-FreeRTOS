#include "bsp.h"
void esc_init_task(void *pvParameters){
    HAL_GPIO_WritePin(RollerSwitch_GPIO_Port,RollerSwitch_Pin,GPIO_PIN_SET);
    vTaskDelay(1000);
    HAL_GPIO_WritePin(PropLSwitch_GPIO_Port,PropLSwitch_Pin,GPIO_PIN_SET);
    vTaskDelay(1000);
    HAL_GPIO_WritePin(PropRSwitch_GPIO_Port,PropRSwitch_Pin,GPIO_PIN_SET);
    vTaskDelay(1000);
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,18500);
    vTaskDelay(1500);
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,18000);
    vTaskDelay(1500);
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,19000);
    vTaskDelay(1500);
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,18500);
    
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,18000);
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,18000);
    __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,18000);
    vTaskDelay(1500);
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,19000);
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,19000);
    __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,19000);
    vTaskDelay(1500);
    vTaskDelete(ESC_TASK_Handler);
}