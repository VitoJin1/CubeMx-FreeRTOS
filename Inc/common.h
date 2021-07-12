#ifndef __COMMON_H
#define __COMMON_H
#include "stdint.h"
#define ABS(x) ((x>0)? (x):(-x))
#define LIMIT_MIN_MAX(x,min,max) (x) = (((x)<=(min))?(min):(((x)>=(max))?(max):(x)))
#define PI (3.1415927f)
float byte2float(uint8_t * pt);
typedef union  Resolve//small end mode for stm32
{
    float float_data;
    char char_data[4];
}Resolve_Typedef;

#endif 