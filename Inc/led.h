#ifndef __LED_H
#define __LED_H
#include "main.h"
#include "cmsis_os.h"
#include "usart.h"
#include "remoter.h"
void print_task(void * pvParameters);

void led_task(void *pvParameters);
#endif // !__LED_H