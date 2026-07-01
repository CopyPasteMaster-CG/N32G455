
#ifndef __APP_H__
#define __APP_H__

#include "stdbool.h"
#include "string.h"
#include "stdint.h"


struct _APP_HANDLE_struct{
    int (*Create)(void);
    int (*Read)(void *);
    void (*Event)(int events);
};
typedef struct _APP_HANDLE_struct APP_HANDLE_typedefstruct;

#define __MAKE_STR(x) #x

//  任务创建时的状态
#define APP_CREATE_OK           (0)
#define APP_CREATE_ERR_EVENT    (1)
#define APP_CREATE_ERR_MUTEX    (2)
#define APP_CREATE_ERR_HANDLE   (3)

//  任务运行过程中的状态
#define APP_EVENT_START     (0x01<<(0))

#include "APP_GSV2011.h"
#include "APP_USART.h"
#include "APP_NEC.h"
#include "APP_FLASH.h"
#include "APP_FPGA.h"
#include "APP_KEY.h"
#include "APP_LED.h"
#include "APP_SOFT_USART.h"

#endif
