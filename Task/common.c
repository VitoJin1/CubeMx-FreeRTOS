#include "common.h"
//can't use hear in stm32
float byte2float(uint8_t * pt){
    float * result;
   result =(float *)pt;
    return (* result);
}
