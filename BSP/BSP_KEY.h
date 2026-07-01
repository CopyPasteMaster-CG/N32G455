
#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

#include "stdint.h"


#define BSP_KEY_NUM_1   (0x01)
#define BSP_KEY_NUM_2   (0x02)
#define BSP_KEY_NUM_3   (0x04)
#define BSP_KEY_NUM_4   (0x08)

struct _BSP_KEY_FUN_struct{
    void ( * Init ) ( void ) ;
    void ( * Read ) ( uint8_t * state ) ;
} ;
typedef const struct _BSP_KEY_FUN_struct BSP_KEY_FUN_typestruct;
extern BSP_KEY_FUN_typestruct BSP_KEY_FUN;  // KEY 相关的函数指针结构体

#endif
