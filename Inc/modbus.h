#ifndef __MODBUS_H
#define __MODBUS_H
#include "main.h"
#include "remoter.h"
#include "math.h"
#include "usart.h"
#include "tim.h"
#include "timers.h"
#include "led.h"//delete later
#define MODBUS_DMA_RECEIVE_BUFFER 20
#define MODBUS_DMA_TRANSMIT_BUFFER 10
#define MODBUSMAXLOAD    65
#define MODBUSMAXTEMP     80
extern uint8_t RS485_RX_BUFF[MODBUS_DMA_RECEIVE_BUFFER];
extern uint8_t RS485SafeDetectFlag;//
extern uint8_t OverLoadWarnning;
uint16_t CRC_Compute(uint8_t *puchMsg, uint16_t usDataLen);
void RS485_SendData(uint8_t *buff,uint8_t len);
void Master_03_Slove( uint8_t board_adr,uint16_t start_address,uint16_t lenth );
void Master_06_Slove(uint8_t board_adr,uint16_t start_address, int16_t value);
void CableMotorSpeedControl(int16_t LVel,int16_t RVel);
void RightPulseGenerate(uint16_t pre);
void LeftPulseGenerate(uint16_t pre);
float fPower_decode(uint8_t *pData);
uint16_t iTempeature_decode(uint8_t *pData);
void MODBUSDataProcess(uint8_t *pData);
void FiveSecModbusProtectCallback(TimerHandle_t xTimer);
#endif // !__MODBUS_H
