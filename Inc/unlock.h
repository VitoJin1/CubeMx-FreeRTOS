#ifndef __UNLOCK_H
#define __UNLOCK_H
#include "main.h"
#include "remoter.h"

void unlockRobotDectect(void *pvParameters);
uint8_t CheckForUnlock(void); 
extern uint8_t RobotUnlockFlag;
#endif // !__UNLOCK_H
