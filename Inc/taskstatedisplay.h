#ifndef __TASKSTATEDISPLAY_H
#define __TASKSTATEDISPLAY_H
#include "main.h"
#include "string.h"

//choose one marco to show the corespond message


#define GETSYSTEMSTATE  0 // show all tasks 
#define GETTASKINFO     1 // show  a signal task's infomation
#define GETTASKSTATE    0 // show all task state
#define GETTASKRUNTIME  0
extern volatile unsigned long long FreeRTOSRunTimeTicks;


void task_state_display(void *pvParameters);


#endif