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
extern float Z_Gyro;
extern float Z_Euler_Angle;
extern uint8_t Request_imu_data_flag;
extern uint8_t loseImuSignalFlag;
#endif // !__IMU_H
