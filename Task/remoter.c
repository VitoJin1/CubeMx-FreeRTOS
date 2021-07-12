#include "remoter.h"
uint8_t LORA_DATA_RECEIVE[UART3_DMA_RECEIVE_BUFFER];
uint8_t Request_lora_flag=0;//maybe can be optimized, without using flag
Remoter RemoteCommand;
uint8_t LoseLoraSignalFlag=0;
void lora_receive_task(void * pvParameters){
    TickType_t PreviousWakeTime;
    PreviousWakeTime=xTaskGetTickCount();
    const TickType_t TimeIncreasement=pdMS_TO_TICKS(25);
    BaseType_t err=pdFALSE;
    __HAL_UART_ENABLE_IT(&huart3,UART_IT_IDLE);
    for(;;){
        HAL_UART_Receive_DMA(&huart3,LORA_DATA_RECEIVE,UART3_DMA_RECEIVE_BUFFER);
        Request_lora_flag=1;
        err=xSemaphoreTake( LORARedyToRECEIVE_Semaphore,1500 ) ;
        if( err == pdTRUE ){
            LoseLoraSignalFlag=0;//enhance later
            Request_lora_flag=0;
            LORADataDecode(LORA_DATA_RECEIVE);
        }
        else if(err ==pdFALSE){
            LOTADataClear();
            LoseLoraSignalFlag=1;//enhance later
            Request_lora_flag=0;
            
        }
        vTaskDelayUntil(&PreviousWakeTime,TimeIncreasement);
    }
}
void LOTADataClear(){
    RemoteCommand.acro_switch     =0;
    RemoteCommand.roller_switch   =0;
    RemoteCommand.pump_switch     =0;
    RemoteCommand.balance_switch  =0;
    RemoteCommand.valve_switch    =0;

    RemoteCommand.Right_X_Speed =0.0;
    RemoteCommand.Right_Y_Speed =0.0;
    RemoteCommand.Left_X_Speed  =0.0;
    RemoteCommand.Left_Y_Speed  =0.0;

    RemoteCommand.Left_Prop_Speed=0;
    RemoteCommand.Right_Y_Speed=0;
    RemoteCommand.Roller_Speed =0;
    RemoteCommand.Max_Speed=0;
    RemoteCommand.mode_switch=0;
    RemoteCommand.screw_switch=0;
    RemoteCommand.pose_switch=0;
}
void LORADataDecode(uint8_t *pdata){
    uint8_t i=0;
    for(i=0;i<22;i++){
        if((uint8_t)(*(pdata+i+0))==0x0a&&(uint8_t)(*(pdata+i+1))==0x0d&&(uint8_t)(*(pdata+i+20))==0x0d&& (uint8_t)(*(pdata+i+21))==0x0a){
            RemoteCommand.ch2           =pdata[i+2]*256+pdata[i+3]*1;
            RemoteCommand.ch3           =pdata[i+4]*256+pdata[i+5]*1;
            RemoteCommand.ch1           =pdata[i+6]*256+pdata[i+7]*1;
            RemoteCommand.ch0           =pdata[i+8]*256+pdata[i+9]*1; 
            RemoteCommand.roller_speed  =pdata[i+10]*256+pdata[i+11]*1;
            RemoteCommand.max_speed     =pdata[i+12]*256+pdata[i+13]*1;
            RemoteCommand.prop_speed    =pdata[i+16]*256+pdata[i+17]*1;

            if( (((pdata[18]>>7)&0x01)==1) && (((pdata[18]>>6)&0x01)==0) )
            RemoteCommand.pose_switch=0;
            else if( (((pdata[18]>>7)&0x01)==0) && (((pdata[18]>>6)&0x01)==0) )
            RemoteCommand.pose_switch=1;
            else RemoteCommand.pose_switch=2;

            if( (((pdata[18]>>5)&0x01)==0)  && (((pdata[18]>>4)&0x01)==0) )
            RemoteCommand.screw_switch=0;
            else if( (((pdata[18]>>5)&0x01)==1)  && (((pdata[18]>>4)&0x01)==0) )
            RemoteCommand.screw_switch=1;
            else if( (((pdata[18]>>5)&0x01)==0)  && (((pdata[18]>>4)&0x01)==1) )
            RemoteCommand.screw_switch=2;

            if( (((pdata[18]>>3)&0x01)==1)  && (((pdata[18]>>2)&0x01)==0) )
            RemoteCommand.mode_switch=0;
            else if (  (((pdata[18]>>3)&0x01)==0)  && (((pdata[18]>>2)&0x01)==0) )
            RemoteCommand.mode_switch=1;
            else if (  (((pdata[18]>>3)&0x01)==0)  && (((pdata[18]>>2)&0x01)==1) )
            RemoteCommand.mode_switch=2;

            RemoteCommand.acro_switch     =(pdata[19]>>6)&0x01;
            RemoteCommand.roller_switch   =(pdata[19]>>5)&0x01;
            RemoteCommand.pump_switch     =(pdata[19]>>4)&0x01;
            RemoteCommand.balance_switch  =(pdata[19]>>3)&0x01;
            RemoteCommand.valve_switch    =(pdata[19]>>2)&0x01;

            RemoteCommand.Right_X_Speed =fRamp_for_LORA(RemoteCommand.ch0);
            RemoteCommand.Right_Y_Speed =fRamp_for_LORA(RemoteCommand.ch1);
            RemoteCommand.Left_X_Speed =fRamp_for_LORA(RemoteCommand.ch2);
            RemoteCommand.Left_Y_Speed =fRamp_for_LORA(RemoteCommand.ch3);

            RemoteCommand.Left_Prop_Speed=iPwm_for_LORA(RemoteCommand.prop_speed);
            RemoteCommand.Right_Prop_Speed=iPwm_for_LORA(RemoteCommand.prop_speed);
            RemoteCommand.Roller_Speed =iPwm_for_LORA(RemoteCommand.roller_speed);
            RemoteCommand.Max_Speed=iPwm_for_LORA(RemoteCommand.max_speed);
        }
    }
}

float fRamp_for_LORA(int16_t temp)//joystick analog data interprate, 0-26000 is normal, middle area is 13000 +/- 1000, range set to 1000-25000 
{
    float result=0.0;
    if(temp>(13000-1000)&&temp<(13000+1000))
        result=0.0;
    else if(temp<=13000-1000)
        result=((float)temp-12000.0f)/11000.0f;
    else if(temp>13000+1000)
        result=((float)temp-14000.0f)/11000.0f;
    LIMIT_MIN_MAX(result,-1.0f,1.0f);
    return result;
}

short sRamp_for_LORA(int16_t temp)//sbus switch transport
{
    if(temp==1)
        return 1;
    else if(temp==2)
        return 2;
    else 
        return 0;// receive 0 and receive error will generate 0 command.
}

int iPwm_for_LORA(int16_t temp)// joystick value ignore 0-1000 , 25000 up ,   1000- 25000 map to 0-750
{
    int result;
    if(temp<1000)
        result =0;
    else if(temp>=25000)
        result =750;
    else 
        result =(int)((temp-1000)/32);
    return result;
}