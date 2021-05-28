/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "semphr.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern SemaphoreHandle_t IMUReadyToRECEIVE_Semaphore;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ValveSwitch_Pin GPIO_PIN_3
#define ValveSwitch_GPIO_Port GPIOE
#define LEDR_Pin GPIO_PIN_0
#define LEDR_GPIO_Port GPIOC
#define LEDG_Pin GPIO_PIN_1
#define LEDG_GPIO_Port GPIOC
#define LEDB_Pin GPIO_PIN_2
#define LEDB_GPIO_Port GPIOC
#define LEDY_Pin GPIO_PIN_3
#define LEDY_GPIO_Port GPIOC
#define PutterIn1_Pin GPIO_PIN_0
#define PutterIn1_GPIO_Port GPIOA
#define PutterIn2_Pin GPIO_PIN_1
#define PutterIn2_GPIO_Port GPIOA
#define RollerSwitch_Pin GPIO_PIN_4
#define RollerSwitch_GPIO_Port GPIOC
#define PropLSwitch_Pin GPIO_PIN_5
#define PropLSwitch_GPIO_Port GPIOC
#define PropRSwitch_Pin GPIO_PIN_6
#define PropRSwitch_GPIO_Port GPIOC
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
