/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define START_TASK_PIRO 1// smaller the number is lower the piority is
#define START_STACK_SIZE 128
TaskHandle_t Start_Task_Handler;
void start_task(void *pvParameters);

#define TASK_STATE_DISPLAY_PIRO 0
#define TASK_STATE_DISPLAY_STACK_SIZE 256
TaskHandle_t Task_State_Handler;
void task_state_display(void *pvParameters);

#define LED_TASK_PIRO 2
#define LED_STACK_SZIE 64
TaskHandle_t LED_TASK_Handler;
void led_task(void *pvParameters);

#define ESC_INIT_TASK_PIRO 1
#define ESC_INIT_STACK_SIZE 128
TaskHandle_t ESC_TASK_Handler;
void esc_init_task(void *pvParameters);

#define ROBOT_UNLOCK_TASK_PIRO 1
#define ROBOT_UNLOCK_STACK_SIZE 64
TaskHandle_t ROBOT_UNLOCK_Handler;
void unlockRobotDectect(void *pvParameters);

#define VALVE_TASK_PIRO 4
#define VALVE_STACK_SIZE 64
TaskHandle_t VALVE_TASK_Handler;
void valve_task(void *pvParameters );

#define PUTTER_TASK_PIRO 2         
#define PUTTER_STACK_SIZE 64        
TaskHandle_t PUTTER_TASK_Handler;   
void putter_task( void *pvParamters );
                                   
#define PRINT_TASK_PIRO 5          
#define PRINT_STACK_SIZE 64        
TaskHandle_t PRINT_TASK_Handler;
void print_task( void * pvParameters );

#define IMU_RECEIVE_TASK_PRIO 6
#define IMU_RECEIVE_STACK_SIZE 512
TaskHandle_t IMU_RECEIVE_TASK_Handler;
void imu_receive_task(void *pvParameters );

#define LORA_RECEIVE_TASK_PRIO 6
#define LORA_RECEIVE_STACK_SIZE 512
TaskHandle_t LORA_RECEIVE_TASK_Handler;
void lora_receive_task(void *pvParameters);

#define MODBUS_TASK_PIRO 5
#define MODBUS_TASK_STACK_SIZE 256
TaskHandle_t MODBUS_TASK_Handler;
void modbus_move_task(void *pvParameters);

#define PUMP_TASK_PIRO 2
#define PUMP_STACK_SIZE 64
TaskHandle_t PUMP_TASK_Handler;
void pump_task(void *pvParameters);

#define MODBUS_SAFECHECK_TASK_PIRO 5
#define MODBUS_SAFECHECK_STACK_SIZE 128
TaskHandle_t MODBUS_TASK_Handler;
void modbus_safecheck_task(void *pvParameters); 

#define ROLLER_TASK_PIRO 3
#define ROLLER_TASK_STACK_SIZE 64
TaskHandle_t ROLLER_TASK_Handler;
void roller_task(void *pvParameters);

#define INNER_LOOP_TASK_PIRO 4
#define INNER_LOOP_TASK_STACK_SIZE 128
TaskHandle_t INNER_TASK_Handler;
void inner_loop(void *pvParameters);

#define OUTER_LOOP_TASK_PIRO 4
#define OUTER_LOOP_TASK_STACK_SIZE 128
TaskHandle_t OUTER_TASK_Handler;
void outer_loop(void *pvParameters);

#define MANUAL_CALI_TASK_PIRO 5
#define MANUAL_CALI_TASK_STACK_SIZE 128
TaskHandle_t MANUAL_TASK_Handler;
void manual_cali(void * pvParameters);

#define BALANCE_CHECK_TASK_PIRO 3
#define BALANCE_CHEKC_TASK_STACK_SIZE 128
TaskHandle_t BALANCE_TASK_Handler;
void balance_check(void * pvParameters);

#define PROP_CONTROL_TASK_PIRO 3
#define PROP_CONTROL_TASK_STACK_SIZE 128
TaskHandle_t PROP_TASK_Handler;
void propeller_control(void * pvParameters);
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
SemaphoreHandle_t IMUReadyToRECEIVE_Semaphore;
SemaphoreHandle_t LORARedyToRECEIVE_Semaphore;
SemaphoreHandle_t MODBUSReadyToRECEIVE_Semaphore;
TimerHandle_t FiveSecModbusProtectTimer;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_UART4_Init();
  MX_USART3_UART_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  MX_TIM10_Init();
  MX_TIM11_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
  IMUReadyToRECEIVE_Semaphore=xSemaphoreCreateBinary();
  LORARedyToRECEIVE_Semaphore=xSemaphoreCreateBinary();
  MODBUSReadyToRECEIVE_Semaphore=xSemaphoreCreateBinary();
  xTaskCreate((TaskFunction_t)esc_init_task,
              (const char *)"esc_init_task",
              (uint16_t)ESC_INIT_STACK_SIZE,
              (void *)NULL,
              (UBaseType_t)ESC_INIT_TASK_PIRO,
              (TaskHandle_t *)&ESC_TASK_Handler);


  xTaskCreate((TaskFunction_t)lora_receive_task,
              (const char *)"lora_receive_task",
              (uint16_t)LORA_RECEIVE_STACK_SIZE,
              (void * )NULL,
              (UBaseType_t)LORA_RECEIVE_TASK_PRIO,
              (TaskHandle_t * )&LORA_RECEIVE_TASK_Handler);

  xTaskCreate((TaskFunction_t)start_task,
              (const char *)"start_task",
              (uint16_t)START_STACK_SIZE,
              (void*)NULL,
              (UBaseType_t)START_TASK_PIRO,
              (TaskHandle_t *)&Start_Task_Handler);

  xTaskCreate((TaskFunction_t)unlockRobotDectect,
                (const char *)"robot_unlock_task",
                (uint16_t)ROBOT_UNLOCK_STACK_SIZE,
                (void *)NULL,
                (UBaseType_t)ROBOT_UNLOCK_TASK_PIRO,
                (TaskHandle_t *)&ROBOT_UNLOCK_Handler);


  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();
  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* DMA1_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
  /* UART4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(UART4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(UART4_IRQn);
}

/* USER CODE BEGIN 4 */

void start_task(void *pvParameters){
  uint32_t NotifyValue=0;
  for(;;){
    NotifyValue=ulTaskNotifyTake(pdTRUE,100);
    if(NotifyValue!=0){
      vTaskDelete(ROBOT_UNLOCK_Handler);
      taskENTER_CRITICAL( );
      xTaskCreate((TaskFunction_t)led_task,
                  (const char *)"led_task",
                  (uint16_t)LED_STACK_SZIE,
                  (void *)NULL,
                  (UBaseType_t)LED_TASK_PIRO,
                  (TaskHandle_t*)&LED_TASK_Handler);

      xTaskCreate((TaskFunction_t)modbus_move_task,
                  (const char *)"modbus_move_task",
                  (uint16_t)MODBUS_TASK_STACK_SIZE,
                  (void *)NULL,
                  (UBaseType_t)MODBUS_TASK_PIRO,
                  (TaskHandle_t *)&MODBUS_TASK_Handler);
                  
                  /*
      xTaskCreate((TaskFunction_t)imu_receive_task,
                  (const char*)"imu_receive_task",
                  (uint16_t)IMU_RECEIVE_STACK_SIZE,
                  (void *)NULL,
                  (UBaseType_t)IMU_RECEIVE_TASK_PRIO,
                  (TaskHandle_t *)&IMU_RECEIVE_TASK_Handler);
                  */
 /*
      xTaskCreate((TaskFunction_t)modbus_safecheck_task,
                  (const char *)"modbus_safecheck_task",
                  (uint16_t)MODBUS_SAFECHECK_STACK_SIZE,
                  (void *)NULL,
                  (UBaseType_t)MODBUS_SAFECHECK_TASK_PIRO,
                  (TaskHandle_t *)&MODBUS_TASK_Handler);
      */
     /*
      FiveSecModbusProtectTimer=xTimerCreate((const char*)"5s timer",
                                            (TickType_t)5000,
                                            (UBaseType_t)pdFALSE,
                                            (void *)0,
                                            (TimerCallbackFunction_t)FiveSecModbusProtectCallback);
    */
    //valve task
    /*
      xTaskCreate((TaskFunction_t)valve_task,
                  (const char *)"valve_control_task",
                  (uint16_t)VALVE_STACK_SIZE,                               
                  (void *)NULL,
                  (UBaseType_t)VALVE_TASK_PIRO,
                  (TaskHandle_t *)&VALVE_TASK_Handler);
    */
    /*
      xTaskCreate((TaskFunction_t)task_state_display,
                  (const char * )"task_state_display",
                  (uint16_t)TASK_STATE_DISPLAY_STACK_SIZE,
                  (void *)NULL,
                  (UBaseType_t)TASK_STATE_DISPLAY_PIRO,
                  (TaskHandle_t * )&Task_State_Handler);

      */

      //putter task
      /*
      xTaskCreate((TaskFunction_t)putter_task,
                  (const char *)"putter_task",
                  (uint16_t)PUTTER_STACK_SIZE,
                  (void *)NULL,
                  (UBaseType_t)PUTTER_TASK_PIRO,
                  (TaskHandle_t *)&PUTTER_TASK_Handler);
                  */
    
      //pump task
      /*
      xTaskCreate((TaskFunction_t)pump_task,
                  (const char *)"pump_task",
                  (uint16_t)PUMP_STACK_SIZE,
                  (void *)NULL,
                  (UBaseType_t)PUMP_TASK_PIRO,
                  (TaskHandle_t *)&PUMP_TASK_Handler);
                  */
      //roller task
      /*
      xTaskCreate((TaskFunction_t)roller_task,
                  (const char *)"roller_task",
                  (uint16_t)ROLLER_TASK_STACK_SIZE,
                  (void *)NULL,
                  (UBaseType_t)ROLLER_TASK_PIRO,
                  (TaskHandle_t *)&ROLLER_TASK_Handler);

      xTaskCreate((TaskFunction_t)balance_check,
                  (const char *)"balance_check",
                  (uint16_t)BALANCE_CHEKC_TASK_STACK_SIZE,
                  (void*)NULL,
                  (UBaseType_t)BALANCE_CHECK_TASK_PIRO,
                  (TaskHandle_t *)&BALANCE_TASK_Handler);

      xTaskCreate((TaskFunction_t)manual_cali,
                  (const char *)"manual_cali",
                  (uint16_t)MANUAL_CALI_TASK_STACK_SIZE,
                  (void *)NULL,
                  (UBaseType_t)MANUAL_CALI_TASK_PIRO,
                  (TaskHandle_t*)&MANUAL_TASK_Handler);

      xTaskCreate((TaskFunction_t)inner_loop,
                  (const char *)"inner_loop",
                  (uint16_t)INNER_LOOP_TASK_STACK_SIZE,
                  (void *)NULL,
                  (UBaseType_t)INNER_LOOP_TASK_PIRO,
                  (TaskHandle_t*)&INNER_TASK_Handler);
      
      xTaskCreate((TaskFunction_t)outer_loop,
                  (const char *)"outer_loop",
                  (uint16_t)OUTER_LOOP_TASK_STACK_SIZE,
                  (void *)NULL,
                  (UBaseType_t)OUTER_LOOP_TASK_PIRO,
                  (TaskHandle_t *)&OUTER_TASK_Handler);
      
      xTaskCreate((TaskFunction_t)propeller_control,
                  (const char *)"propeller_control",
                  (uint16_t)PROP_CONTROL_TASK_STACK_SIZE,
                  (void *)NULL,
                  (UBaseType_t)PROP_CONTROL_TASK_PIRO,
                  (TaskHandle_t* )&PROP_TASK_Handler);
                  */
      vTaskDelete(Start_Task_Handler);
      taskEXIT_CRITICAL();
    }
    else vTaskDelay(1000); 
  }
}
/* USER CODE END 4 */

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
