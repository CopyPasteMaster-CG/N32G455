
#ifndef __APP_LED_H__
#define __APP_LED_H__

#include "APP.h"
#include "rtthread.h"

typedef struct{
    bool activate;
    uint32_t tick;
    bool value[5];
}APP_LED_DATA_Typedefstruct;

extern const APP_HANDLE_typedefstruct APP_LED_FUN;



#endif
