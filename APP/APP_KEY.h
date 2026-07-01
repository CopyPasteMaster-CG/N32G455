
#ifndef __APP_KEY_H__
#define __APP_KEY_H__

#include "APP.h"
#include "rtthread.h"

typedef struct{
    bool activate;
    uint32_t tick;
    bool value[4];
    uint8_t mode;
}APP_KEY_DATA_Typedefstruct;

extern const APP_HANDLE_typedefstruct APP_KEY_FUN;



#endif
