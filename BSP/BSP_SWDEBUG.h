
#ifndef __BSP_SWDEBUG_H__
#define __BSP_SWDEBUG_H__

struct _BSP_SWDEBUG_FUN_struct{
    void ( * Init ) ( void ) ;
} ;
typedef const struct _BSP_SWDEBUG_FUN_struct BSP_SWDEBUG_FUN_typestruct;
extern BSP_SWDEBUG_FUN_typestruct BSP_SWDEBUG_FUN;  // SWDEBUG 相关的函数指针结构体

#endif
