#ifndef __IMU_H
#define __IMU_H
#include "main.h"
#include "FreeRTOS.h"
#include "stdint.h"
#include "task.h"
#include "common.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"
#include "usart.h"
#include "stm32f4xx_it.h"
#include "semphr.h"

#define UART4_DMA_RECEIVE_BUFFER  182//4*91 
void imu_receive_task(void *pvParameters);
void IMUDataDecode(uint8_t *pdata);
extern uint8_t Uart4_Rx_Len;
extern  uint8_t Uart4_Rx_End_Flag;
extern uint8_t Request_imu_data_flag;
#endif // !__IMU_H
