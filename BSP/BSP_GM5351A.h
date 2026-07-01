
#ifndef __BSP_GM5351A_H__
#define __BSP_GM5351A_H__

#include "stdint.h"


struct _BSP_GM5351A_FUN_struct{
    void ( * Init ) ( void ) ;
    char ( * Set ) ( double out_fre1 , double out_fre2 ,  double out_fre3  ) ;
} ;
typedef const struct _BSP_GM5351A_FUN_struct BSP_GM5351A_FUN_typestruct;
extern BSP_GM5351A_FUN_typestruct BSP_GM5351A_FUN;  // GM5351A 相关的函数指针结构体

#endif
