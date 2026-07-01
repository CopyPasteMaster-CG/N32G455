
#include "n32g45x.h"

#include "BSP_SWDEBUG.h"
#include "BSP_SYSTEMCLOCK.h"

#include "rtthread.h"

#include "SEGGER_RTT.h"
#include "APP.h"

#include "BSP_SPI.h"
#include "BSP_SOFT_USART.h"

int main(void)
{
    extern SEGGER_RTT_CB _SEGGER_RTT;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
        
    BSP_SWDEBUG_FUN.Init();    
    
    	
    
    APP_USART_FUN.Create();
    APP_SOFT_USART_FUN.Create();
    APP_GSV2011_FUN0.Create();
    APP_NEC_FUN.Create();
    APP_FLASH_FUN.Create();
    APP_FPGA_FUN.Create();
    APP_KEY_FUN.Create();
    APP_LED_FUN.Create();
    
    while(1){
        rt_thread_delay(1);
    }
}
