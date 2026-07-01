
#ifndef __APP_NEC_H__
#define __APP_NEC_H__

#include "APP.h"
#include "rtthread.h"

typedef struct{
    uint8_t mode;   //  0-9 合计10种模式             其他值，不处理，保持上一时刻状态
    uint8_t turn;   //  0,不旋转   1,旋转180度        其他值，不处理，保持上一时刻状态
    uint8_t out;
    bool activate;
}APP_NEC_DATA_Typedefstruct;


extern const APP_HANDLE_typedefstruct APP_NEC_FUN;
void APP_NEC_ClearActivate(APP_NEC_DATA_Typedefstruct *data);



#endif
