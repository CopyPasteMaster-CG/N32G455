
#include "BSP_SWDEBUG.h"

#include "BSP.h"


static void FUN_SWDEBUG_Init(void)
{
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
    GPIO_ConfigPinRemap(GPIO_RMP_SW_JTAG_SW_ENABLE, ENABLE);    //  只允许使用SW接口调试，并且释放PA15/PB3/PB4,这几个端口可以正常作为GPIO端口使用。但PA13/PA14被锁定为调试专用接口，无法使用
}


BSP_SWDEBUG_FUN_typestruct BSP_SWDEBUG_FUN =
{
    .Init = FUN_SWDEBUG_Init,
} ;
