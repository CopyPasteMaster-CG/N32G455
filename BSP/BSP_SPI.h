#ifndef __BSP_SPI_H__
#define __BSP_SPI_H__

#include "stdint.h"

struct _BSP_SPI_FUN_struct{
    void ( * Init ) ( void) ;
    void ( * Write ) ( void * const tx_buf, void * rx_buf, uint8_t len) ;
} ;
typedef const struct _BSP_SPI_FUN_struct BSP_SPI_FUN_typestruct;
extern BSP_SPI_FUN_typestruct BSP_SPI_FUN;  // SPI 相关的函数指针结构体

#endif
