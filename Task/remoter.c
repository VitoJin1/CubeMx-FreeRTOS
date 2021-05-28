#include "remoter.h"
// uint8_t Uart3_Rx_Len;
// uint8_t Uart3_Rx_End_Flag;
// uint8_t LORA_DATA_RECEIVE[UART3_DMA_RECEIVE_BUFFER];
void lora_receive_task(void * pvParameters){
    // static uint16_t receive_cnt=0;
    // __HAL_UART_ENABLE_IT(&huart3,UART_IT_IDLE);
    // HAL_UART_Receive_DMA(&huart3,LORA_DATA_RECEIVE,UART3_DMA_RECEIVE_BUFFER);
    // for(;;){
    //     if(Uart3_Rx_End_Flag==1){
    //         memset(LORA_DATA_RECEIVE,0,UART3_DMA_RECEIVE_BUFFER);
    //         Uart3_Rx_Len=0;
    //         Uart3_Rx_End_Flag=0;
    //         HAL_UART_Receive_DMA(&huart3,LORA_DATA_RECEIVE,UART3_DMA_RECEIVE_BUFFER);
    //         receive_cnt++;
    //     }
    //     vTaskDelay(1);
    // }
}