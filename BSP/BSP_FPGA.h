
#ifndef __BSP_FPGA_H__
#define __BSP_FPGA_H__

#include "stdint.h"
#include "stdbool.h"

struct _BSP_FPGA_FUN_struct{
    bool ( * Init ) ( void ) ;      //  初始化设备
//    bool ( * CONFIG ) ( void ) ;    //  配置FPGA
    bool ( * CFG_DONE )( void ) ;   //  FPGA配置完成，true代表完成
//    bool ( * INIT_FLAG )( void );   //  FPGA的配置是否正确，错误就重新配置
} ;
typedef const struct _BSP_FPGA_FUN_struct BSP_FPGA_FUN_typestruct;
extern BSP_FPGA_FUN_typestruct BSP_FPGA_FUN;  // FPGA 相关的函数指针结构体


#endif
