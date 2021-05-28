#include "imu.h"
uint8_t IMU_DATA_RECEIVE[UART4_DMA_RECEIVE_BUFFER];
uint8_t Uart4_Rx_Len=0;
uint8_t Uart4_Rx_End_Flag=0;
uint8_t Request_imu_data_flag=0;
uint8_t test_array[]={0x01,0x3d,0xa6,0x31,0x3a};
float Z_Gyro=0.0;
float Z_Euler_Angle=0.0;
char Z_Gryo_print[20];
char Z_Euler_Angle_print[20];
Resolve_Typedef IMUUnresolveData;
void imu_receive_task(void *pvParameters)
{
  TickType_t PreviousWakeTime;
  const TickType_t TimeIncrement=pdMS_TO_TICKS(500);
  static uint16_t receive_cnt=0;
  static uint8_t  receiveData[3];
  BaseType_t err=pdFALSE;
  __HAL_UART_ENABLE_IT(&huart4,UART_IT_IDLE);
  for(;;){
    HAL_UART_Receive_DMA(&huart4,IMU_DATA_RECEIVE,UART4_DMA_RECEIVE_BUFFER); 
    Request_imu_data_flag=1;
    err=xSemaphoreTake(IMUReadyToRECEIVE_Semaphore,100);
    if(err==pdTRUE){
      //printf("%x,%x,%x,%x,%x\r\n",IMU_DATA_RECEIVE[0],IMU_DATA_RECEIVE[1],IMU_DATA_RECEIVE[2],IMU_DATA_RECEIVE[3],IMU_DATA_RECEIVE[4]);
      Request_imu_data_flag=0;
      IMUDataDecode(IMU_DATA_RECEIVE);
    }
    else if(err==pdFALSE){
      Request_imu_data_flag=0;
      printf("error\r\n");
    }
    vTaskDelay(500);
    //vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);
  }
}

void IMUDataDecode(uint8_t *pdata){
    uint8_t i=0;
    for(i=0;i<91;i++){
      if((uint8_t)(*(pdata+i+0))==0x3a&&(uint8_t)(*(pdata+i+89))==0x0d&&(uint8_t)(*(pdata+i+90))==0x0a)
      {
        IMUUnresolveData.char_data[0]=*(pdata+i+19);
        IMUUnresolveData.char_data[1]=*(pdata+i+20);
        IMUUnresolveData.char_data[2]=*(pdata+i+21);
        IMUUnresolveData.char_data[3]=*(pdata+i+22);
        Z_Gyro=IMUUnresolveData.float_data;
        IMUUnresolveData.char_data[0]=*(pdata+i+71);
        IMUUnresolveData.char_data[1]=*(pdata+i+72);
        IMUUnresolveData.char_data[2]=*(pdata+i+73);
        IMUUnresolveData.char_data[3]=*(pdata+i+74);
        Z_Euler_Angle=IMUUnresolveData.float_data;
        //printf("%f %f\r\n ",Z_Gyro,Z_Euler_Angle);
        return;
      }
    }
    printf("failed\r\n");
    return;
}