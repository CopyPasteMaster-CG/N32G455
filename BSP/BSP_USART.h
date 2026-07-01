
#ifndef __BSP_USART_H__
#define __BSP_USART_H__

#include "stdint.h"
typedef void (*BSP_USART_RECEIVE_IRQ_Callback)(uint8_t data);
struct _BSP_USART_FUN_struct{
    void ( * Init ) ( uint32_t baud ,BSP_USART_RECEIVE_IRQ_Callback rx_callback) ;
    void ( * Write ) ( void * data, uint32_t len ) ;
} ;
typedef const struct _BSP_USART_FUN_struct BSP_USART_FUN_typestruct;
extern BSP_USART_FUN_typestruct BSP_USART_FUN;  // USART 相关的函数指针结构体

#endif
