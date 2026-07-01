
#ifndef __BSP_SYSTEMCLOCK_H__
#define __BSP_SYSTEMCLOCK_H__

#include "stdint.h"

struct _BSP_SYSTEMCLOCK_FUN_struct{
    void ( * Init ) ( void ) ;
    void ( * Delay_us )(uint32_t us);
} ;
typedef const struct _BSP_SYSTEMCLOCK_FUN_struct BSP_SYSTEMCLOCK_FUN_typestruct;
extern BSP_SYSTEMCLOCK_FUN_typestruct BSP_SYSTEMCLOCK_FUN;  // SYSTEMCLOCK 相关的函数指针结构体

#endif
