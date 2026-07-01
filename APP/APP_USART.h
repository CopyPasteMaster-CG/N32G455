
#ifndef __APP_USART_H__
#define __APP_USART_H__

#include "APP.h"
#include "rtthread.h"

typedef struct{
    uint8_t mode;   //  0-9 合计10种模式             其他值，不处理，保持上一时刻状态
    uint8_t turn;   //  0,不旋转   1,旋转180度        其他值，不处理，保持上一时刻状态
    bool activate;
}APP_USART_DATA_Typedefstruct;

typedef struct{
    uint8_t buf[3];
}APP_USART_QURUR_DATA_Typedefstruct;


extern const APP_HANDLE_typedefstruct APP_USART_FUN;



#endif
