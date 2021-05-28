#ifndef __COMMON_H
#define __COMMON_H
#include "stdint.h"
float byte2float(uint8_t * pt);
typedef union  Resolve//small end mode for stm32
{
    float float_data;
    char char_data[4];
}Resolve_Typedef;

#endif 