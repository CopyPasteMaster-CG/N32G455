
#ifndef __BSP_GSV2011_H__
#define __BSP_GSV2011_H__

#include "av_config.h"

struct _BSP_GSV2011_FUN_struct{
    AvRet (*Init)(void);
    AvRet (*I2cRead)(uint32 devAddress, uint32 regAddress, uint8 *data, uint16 count);
    AvRet (*I2cWrite)(uint32 devAddress, uint32 regAddress, uint8 *data, uint16 count);
    AvRet (*UartSendByte)(uint8 *data, uint16 size);
    AvRet (*UartGetByte)(uint8 *data);
    AvRet (*GetKey)(uint8 *data);
    AvRet (*IrdaGetByte)(uint8 *data);
    void (*RST_Enable)(void);
    void (*RST_Disable)(void);
} ;
typedef const struct _BSP_GSV2011_FUN_struct BSP_GSV2011_FUN_typestruct;
extern BSP_GSV2011_FUN_typestruct BSP_GSV2011_FUN0;  // GSV2011 相关的函数指针结构体
extern BSP_GSV2011_FUN_typestruct BSP_GSV2011_FUN1; 
extern BSP_GSV2011_FUN_typestruct BSP_GSV2011_FUN2; 
extern BSP_GSV2011_FUN_typestruct BSP_GSV2011_FUN3; 
extern BSP_GSV2011_FUN_typestruct BSP_GSV2011_FUN4; 
#endif
