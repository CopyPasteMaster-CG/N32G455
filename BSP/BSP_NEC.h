
#ifndef __BSP_NEC_H__
#define __BSP_NEC_H__

#include "stdint.h"

typedef void (*BSP_NEC_RECEIVE_IRQ_Callback)(uint16_t cnt_down,uint16_t cnt_up);   //  空闲电平是高电平，所以每接收到一次低电平，就传递低电平进入和出去的时间戳，低电平最大持续时间不超过65535us
struct _BSP_NEC_FUN_struct{
    void ( * Init ) ( BSP_NEC_RECEIVE_IRQ_Callback rx_callback) ;
} ;
typedef const struct _BSP_NEC_FUN_struct BSP_NEC_FUN_typestruct;
extern BSP_NEC_FUN_typestruct BSP_NEC_FUN;  // NEC 相关的函数指针结构体

#endif
