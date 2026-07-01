
#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__

#include "stdint.h"
#include "stdbool.h"

typedef struct{
    char NEC_MODE;
    char NEC_TURN;
    char NEC_OUT;
    char USART_MODE;
    char USART_TURN;
}BSP_FLASH_DATA_Typedefstruct;

typedef void (*BSP_FLASH_RESET_DATA_Callback)(BSP_FLASH_DATA_Typedefstruct*data);
struct _BSP_FLASH_FUN_struct{
    void ( * Init ) ( BSP_FLASH_RESET_DATA_Callback rx_callback) ;
    void ( * Write ) ( BSP_FLASH_DATA_Typedefstruct * data ) ;
    bool ( * Read ) ( BSP_FLASH_DATA_Typedefstruct * data ) ;
} ;
typedef const struct _BSP_FLASH_FUN_struct BSP_FLASH_FUN_typestruct;
extern BSP_FLASH_FUN_typestruct BSP_FLASH_FUN;  // FLASH 相关的函数指针结构体

#endif
