#include "modbus.h"
#define CABLE_MANUAL_WIND_SPEED 2500
#define CABLE_AUTO_WIND_SPEED 1500
#define READ_HOLD_REG  03
#define SET_SIHGLE_REG 06
#define Cable_Motor_Left_Address 0x01
#define Cable_Motor_Right_Address 0x02
#define Cable_Motor_Speed_Address 0x301
#define MODBUS_SPEED_CONTROL 1
#define PN008 100
#define HIGH(n)  ((n)>>8)
#define LOW(n)    ((n)&0xff)
#define Max_Power_Show_Address 0x201b
#define IGBT_Temperature_Address 0x2025
uint8_t RS485_RX_BUFF[MODBUS_DMA_RECEIVE_BUFFER];//receive buffer
uint8_t RS485_TX_BUFF[MODBUS_DMA_TRANSMIT_BUFFER];//transmit buffer
uint8_t RS485SafeDetectFlag=0;
uint8_t RS485ReceiveFailed[4];
float RS485ReceiveResult[4];//[0]--left power [1]-right power [2]-left temperature [3]-right temperature
uint8_t OverLoadWarnning=0;
#if  !MODBUS_SPEED_CONTROL
uint8_t LastLeftDir=2;//init value , later on 0 means LEFT_Dir=0 1 means LEFT_Dir =1
uint8_t LastRightDir=2;//init value , laster on 0 means RIGHT_Dir =0 1 means RIGHT_Dir =1
#endif
volatile int16_t Wind_Left =0;
volatile int16_t Wind_Right=0;

void modbus_move_task(void *pvParameters){
    float pre_Left =0.0;
    float pre_Right=0.0;
    TickType_t PreviousWakeTime;
    PreviousWakeTime=xTaskGetTickCount();
    const TickType_t TimeIncrement=pdMS_TO_TICKS(100);
    #if MODBUS_SPEED_CONTROL
    uint8_t cable_switch=0;
    #endif
    for(;;){
        if(RemoteCommand.mode_switch==MANUAL_MODE||RemoteCommand.mode_switch==CALI_MODE){
            Wind_Left =RemoteCommand.Left_Y_Speed *CABLE_MANUAL_WIND_SPEED;
            Wind_Right=RemoteCommand.Right_Y_Speed*CABLE_MANUAL_WIND_SPEED;
        }
        else if(RemoteCommand.mode_switch==AUTO_MODE){
            pre_Left =(-RemoteCommand.Right_X_Speed*sqrt(2)/2.0+RemoteCommand.Right_Y_Speed*sqrt(2)/2.0);
            pre_Right=(RemoteCommand.Right_X_Speed*sqrt(2)/2.0+RemoteCommand.Right_Y_Speed*sqrt(2)/2.0);//-sqrt(2)
            //left is bigger
            if(ABS(RemoteCommand.Right_X_Speed)-ABS(RemoteCommand.Right_Y_Speed)>=0.001){
                if(ABS(pre_Left)-ABS(pre_Right)>=0.001){
                    Wind_Left=ABS(RemoteCommand.Right_X_Speed)*CABLE_AUTO_WIND_SPEED;
                    Wind_Right=ABS(pre_Right/pre_Left)*Wind_Left;
                }                 
                else{
                    Wind_Right=ABS(RemoteCommand.Right_X_Speed)*CABLE_AUTO_WIND_SPEED;
                    Wind_Left=ABS(pre_Left/pre_Right)*Wind_Right;
                }
            }
            //right is bigger
            else{
                if(ABS(pre_Left)-ABS(pre_Right)>=0.001){
                    Wind_Left=ABS(RemoteCommand.Right_Y_Speed)*CABLE_AUTO_WIND_SPEED;
                    Wind_Right=ABS(pre_Right/pre_Left)*Wind_Left;
                }
                else{
                    Wind_Right=ABS(RemoteCommand.Right_Y_Speed)*CABLE_AUTO_WIND_SPEED;
                    Wind_Left=ABS(pre_Left/pre_Right)*Wind_Right;
                }
            }
            if( (OverLoadWarnning!=0)&&( (pre_Left>0)||(pre_Right>0) ) ){
                Wind_Left=0;
                Wind_Right=0;
            }
            if(pre_Left<0)
                Wind_Left=-(int16_t)Wind_Left;         
            if(pre_Right<0)
                Wind_Right=-(int16_t)Wind_Right;

        }
        #if MODBUS_SPEED_CONTROL
        if(cable_switch==0){
            cable_switch=1;
            Master_06_Slove(Cable_Motor_Left_Address,Cable_Motor_Speed_Address,(-1)*Wind_Left);
        }
        else if(cable_switch==1){
            cable_switch=0;
            Master_06_Slove(Cable_Motor_Right_Address,Cable_Motor_Speed_Address,(1)*Wind_Right);
        }
        vTaskDelay(50);
        #endif
        #if !MODBUS_SPEED_CONTROL //pulse control
            CableMotorSpeedControl(Wind_Left,Wind_Right);
            vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);
        #endif
        
    }
}

void modbus_safecheck_task(void *pvParameters){
    TickType_t PreviousWakeTime;
    PreviousWakeTime=xTaskGetTickCount();
    const TickType_t TimeIncrement=pdMS_TO_TICKS(75);
    BaseType_t err=pdFALSE;
    __HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);
    for(;;){
        //Master_06_Slove(Cable_Motor_Right_Address,Cable_Motor_Speed_Address,0);
        if(RS485SafeDetectFlag==0)
        Master_03_Slove(Cable_Motor_Left_Address,Max_Power_Show_Address,1);
        else if(RS485SafeDetectFlag==1)
        Master_03_Slove(Cable_Motor_Right_Address,IGBT_Temperature_Address,1);
        else if(RS485SafeDetectFlag==2)
        Master_03_Slove(Cable_Motor_Left_Address,Max_Power_Show_Address,1);
        else if(RS485SafeDetectFlag==3)
        Master_03_Slove(Cable_Motor_Right_Address,IGBT_Temperature_Address,1);
        HAL_UART_Receive_DMA(&huart2,RS485_RX_BUFF,MODBUS_DMA_RECEIVE_BUFFER);
        err=xSemaphoreTake(MODBUSReadyToRECEIVE_Semaphore,20);
        if(err==pdTRUE){
            MODBUSDataProcess(RS485_RX_BUFF);
        }
        else if(err==pdFALSE){
            RS485ReceiveFailed[RS485SafeDetectFlag]++;
            RS485SafeDetectFlag=(RS485SafeDetectFlag+1)%4;
        }
        vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);
    }
}
void MODBUSDataProcess(uint8_t *pData){
    
    if(pData==NULL){
        return;
    }
    if(((0x01==pData[0])||(0x02==pData[0]))&&0x03==pData[1]&&0x02==pData[2])//confirm it is echo signal 
    {
        if(0==RS485SafeDetectFlag){
            RS485ReceiveResult[0]=fPower_decode(&(pData[3]));
        }
        else if(1==RS485SafeDetectFlag){
            RS485ReceiveResult[1]=fPower_decode(&(pData[3]));
        }
        else if(2==RS485SafeDetectFlag){ 
            RS485ReceiveResult[2]=(float)iTempeature_decode(&(pData[3]));
        }
        else if(3==RS485SafeDetectFlag){
            RS485ReceiveResult[3]=(float)iTempeature_decode(&(pData[3]));
        }
        if((RS485ReceiveResult[0]>=MODBUSMAXLOAD)||(RS485ReceiveResult[1]>=MODBUSMAXLOAD)||(RS485ReceiveResult[2]>=MODBUSMAXTEMP)||(RS485ReceiveResult[3]>=MODBUSMAXTEMP)){
            OverLoadWarnning=1;
            xTimerStart(FiveSecModbusProtectTimer,0);
        }
    }
    RS485SafeDetectFlag=(RS485SafeDetectFlag+1)%4;
}

float fPower_decode(uint8_t *pData){
    if(pData==NULL)return 0.0;
    float result;
    result=0.1f*(pData[0]*256+pData[1]);
    if(result>=200)                 //65535/2/10acctually
        result=result-6553.5f;
    if(result>=MODBUSMAXLOAD)OverLoadWarnning=1;
    return result;
}
uint16_t iTempeature_decode(uint8_t *pData){
    uint16_t result;
    result=pData[0]*256+pData[1];
    return result;
}
void CableMotorSpeedControl(int16_t LVel,int16_t RVel){
    #if !MODBUS_SPEED_CONTROL
    LVel=((850-RemoteCommand.Max_Speed)*LVel)/1000;
    RVel=((850-RemoteCommand.Max_Speed)*RVel)/1000;
    LIMIT_MIN_MAX(LVel,-CABLE_MANUAL_WIND_SPEED,CABLE_MANUAL_WIND_SPEED);
    LIMIT_MIN_MAX(RVel,-CABLE_MANUAL_WIND_SPEED,CABLE_MANUAL_WIND_SPEED);
    //confirm pulse dirction signal 
    if(LVel>=0){
        if(1!=LastLeftDir){
            HAL_GPIO_WritePin(Left_Dir_GPIO_Port,Left_Dir_Pin,GPIO_PIN_SET);
            LastLeftDir=1;
            vTaskDelay(1);
        }
    }
    else if(LVel<0){
        if(0!=LastLeftDir){
            HAL_GPIO_WritePin(Left_Dir_GPIO_Port,Left_Dir_Pin,GPIO_PIN_RESET);
            LastLeftDir=0;
            vTaskDelay(1);
        }
    }
    if(RVel>=0){
        if(0!=LastRightDir){
            HAL_GPIO_WritePin(Right_Dir_GPIO_Port,Right_Dir_Pin,GPIO_PIN_RESET);
            LastRightDir=0;
            vTaskDelay(1);
        }
    }
    else if(RVel<0){
        if(1!=LastRightDir){
            HAL_GPIO_WritePin(Right_Dir_GPIO_Port,Right_Dir_Pin,GPIO_PIN_SET);
            LastRightDir=1;
            vTaskDelay(1);
        }
    }
    if(ABS(LVel)<=1){
        HAL_TIM_PWM_Stop(&htim11,TIM_CHANNEL_1);
    }
    else{
        LeftPulseGenerate(1000000*60/(ABS(LVel)*PN008));
        HAL_TIM_PWM_Start(&htim11,TIM_CHANNEL_1);
    }
    if(ABS(RVel)<=1){
        HAL_TIM_PWM_Stop(&htim10,TIM_CHANNEL_1);
    }
    else{
        RightPulseGenerate(1000000*60/(ABS(RVel)*PN008));
        HAL_TIM_PWM_Start(&htim10,TIM_CHANNEL_1);
    }
    #endif
}
void RightPulseGenerate(uint16_t pre)
{

  /* USER CODE BEGIN TIM10_Init 0 */

  /* USER CODE END TIM10_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM10_Init 1 */

  /* USER CODE END TIM10_Init 1 */
  htim10.Instance = TIM10;
  htim10.Init.Prescaler = pre;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim10.Init.Period = 100;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim10) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 50;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim10, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM10_Init 2 */

  /* USER CODE END TIM10_Init 2 */
  HAL_TIM_MspPostInit(&htim10);
}
void LeftPulseGenerate(uint16_t pre)
{

  /* USER CODE BEGIN TIM11_Init 0 */

  /* USER CODE END TIM11_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM11_Init 1 */

  /* USER CODE END TIM11_Init 1 */
  htim11.Instance = TIM11;
  htim11.Init.Prescaler = pre;
  htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim11.Init.Period = 100;
  htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 50;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim11, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM11_Init 2 */

  /* USER CODE END TIM11_Init 2 */
  HAL_TIM_MspPostInit(&htim11);

}
const uint8_t auchCRCHi[] = { 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40} ; 
const uint8_t auchCRCLo[] = { 
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 
0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 
0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 
0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 
0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 
0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 
0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,0x43, 0x83, 0x41, 0x81, 0x80, 0x40} ;
uint16_t CRC_Compute(uint8_t *puchMsg, uint16_t usDataLen) 
{ 
	uint8_t uchCRCHi = 0xFF ; 
	uint8_t uchCRCLo = 0xFF ; 
	uint32_t uIndex ; 
	while (usDataLen--) 
	{ 
		uIndex = uchCRCHi ^ *puchMsg++ ; 
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ; 
		uchCRCLo = auchCRCLo[uIndex] ; 
	} 
	return ((uchCRCHi<< 8)  | (uchCRCLo)) ; 
}
void RS485_SendData(uint8_t *buff,uint8_t len)
{ 
		
        while(__HAL_UART_GET_FLAG(&huart2,UART_FLAG_TXE)==RESET);
        HAL_UART_Transmit(&huart2,buff,(uint16_t)len,0xffff);
}
void Master_03_Slove( uint8_t board_adr,uint16_t start_address,uint16_t lenth )
{ 
	uint16_t calCRC;
    RS485_TX_BUFF[0] = board_adr;
    RS485_TX_BUFF[1] = READ_HOLD_REG;   
    RS485_TX_BUFF[2] = HIGH(start_address);  
    RS485_TX_BUFF[3] = LOW(start_address); 
    RS485_TX_BUFF[4] = HIGH(lenth);
    RS485_TX_BUFF[5] = LOW(lenth);
    calCRC=CRC_Compute(RS485_TX_BUFF,6);
    RS485_TX_BUFF[6]=(calCRC>>8)&0xFF;
    RS485_TX_BUFF[7]=(calCRC)&0xFF;
	RS485_SendData(RS485_TX_BUFF,8);
}
void Master_06_Slove(uint8_t board_adr,uint16_t start_address, int16_t value)
{
	
    uint16_t calCRC;
	RS485_TX_BUFF[0] = board_adr;
    RS485_TX_BUFF[1] = SET_SIHGLE_REG;   
    RS485_TX_BUFF[2] = HIGH(start_address);  
    RS485_TX_BUFF[3] = LOW(start_address); 
    RS485_TX_BUFF[4] = HIGH(value);
    RS485_TX_BUFF[5] = LOW(value);
    calCRC=CRC_Compute(RS485_TX_BUFF,6);
	RS485_TX_BUFF[6]=(calCRC>>8)&0xFF;
    RS485_TX_BUFF[7]=(calCRC)&0xFF;
	RS485_SendData(RS485_TX_BUFF,8);
}
void FiveSecModbusProtectCallback(TimerHandle_t xTimer){
    OverLoadWarnning=0;
    return;
}