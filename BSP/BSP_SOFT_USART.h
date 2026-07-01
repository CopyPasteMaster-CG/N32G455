#ifndef __BSP_SOFT_USART_H__
#define __BSP_SOFT_USART_H__

#include "rtthread.h"
#include "stdint.h"
#include "n32g45x.h"

typedef void (*BSP_SOFT_USART_RX_NOTIFY)(void);

struct _BSP_SOFT_USART_FUN_struct{
    void ( * Init ) (void) ;
    void ( * Write ) ( uint8_t *data, uint16_t len ) ;
    void ( * Poll ) ( void ) ;
    int  ( * Read ) ( uint8_t *data ) ;
    void ( * SetRxNotify ) ( BSP_SOFT_USART_RX_NOTIFY notify ) ;
} ;

typedef const struct _BSP_SOFT_USART_FUN_struct BSP_SOFT_USART_FUN_typestruct;

extern BSP_SOFT_USART_FUN_typestruct BSP_SFOT_USART_FUN;  

#endif

