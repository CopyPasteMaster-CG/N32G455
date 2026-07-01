
#ifndef __APP_GSV2011_H__
#define __APP_GSV2011_H__

#include "APP.h"
#include "rtthread.h"
#include "stdint.h"

extern const APP_HANDLE_typedefstruct APP_GSV2011_FUN0;
extern const APP_HANDLE_typedefstruct APP_GSV2011_FUN1;
extern const APP_HANDLE_typedefstruct APP_GSV2011_FUN2;
extern const APP_HANDLE_typedefstruct APP_GSV2011_FUN3;
extern const APP_HANDLE_typedefstruct APP_GSV2011_FUN4;


typedef struct{
    struct{
        uint16_t width;  //  水平方向像素点个数
        uint16_t height; //  垂直方向像素点个数
        uint16_t rate;   //  刷新率
    }in,out;
    bool connect;
}APP_GSV2011_DATA_TYPEDEFSTRUCT;

#endif
