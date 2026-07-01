
#ifndef __APP_FLASH_H__
#define __APP_FLASH_H__

#include "APP.h"
#include "rtthread.h"

typedef struct{
    char NEC_MODE;
    char NEC_TURN;
    char NEC_OUT;
    char USART_MODE;
    char USART_TURN;
}APP_FLASH_DATA_Typedefstruct;

extern const APP_HANDLE_typedefstruct APP_FLASH_FUN;


#endif
