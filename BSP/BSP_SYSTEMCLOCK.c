
#include "BSP_SYSTEMCLOCK.h"

#include "BSP.h"


static void FUN_SYSTEMCLOCK_Init(void)
{
    //  HSE的晶振频率是8MHz，目标系统频率是144MHz
    RCC_ConfigHse(RCC_HSE_ENABLE);          //  启用HSE
    while(RCC_WaitHseStable() != SUCCESS){} //  死等HSE准备好
    FLASH_SetLatency(FLASH_LATENCY_4);      //  FLASH等待周期是4，在144MHz的主频下
    RCC_ConfigHclk(RCC_SYSCLK_DIV1);        //  HCL等于SYSCLK
    RCC_ConfigPclk2(RCC_HCLK_DIV2);         //  APB2的时钟频率最高时72MHz，由HCLK-2分频得来
    RCC_ConfigPclk1(RCC_HCLK_DIV4);         //  APB1的时钟频率最高时36MHz，由HCLK-4分频得来
    RCC_ConfigPll(RCC_PLL_SRC_HSE_DIV1, RCC_PLL_MUL_18);    //  配置PLLCLK的时钟源选择HSE，进行18倍频，得出144MHz（8MHz*18=144MHz）
    RCC_EnablePll(ENABLE);                  //  启用PLL
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRD) == RESET){}    //  死等PLL准备完成
    RCC_ConfigSysclk(RCC_SYSCLK_SRC_PLLCLK);//  SYSCLK的时钟源选择PLLCLK
    while (RCC_GetSysclkSrc() != RCC_CFG_SCLKSTS_PLL){}     //  死等，硬件时钟源转变完成
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock/1000);
}

static void BSP_FUN_Delay_us(uint32_t us)
{
    volatile uint32_t last_val = SysTick->VAL ; // 计数值辅助判断的
    volatile uint32_t zero_cnt ; //  计数器归零次数
    volatile uint32_t end_val ;  //  计数器退出时的数值

    zero_cnt = ((last_val * ((SysTick->LOAD + 1) / 1000) < (us % 1000)) ? 1 : 0) + (us / 1000) ;
    end_val = (((SysTick->LOAD + 1) - (us % 1000) * ((SysTick->LOAD + 1) / 1000)) + last_val) % (SysTick->LOAD + 1) ;

    //  等待计数值归零的次数耗尽
    while (zero_cnt)
    {
        if (SysTick->VAL > last_val)
        {
            zero_cnt -- ;
        }
        last_val = SysTick->VAL ;
    }

    //  等待计数值到达指定数值
    while (1)
    {
        if (end_val > SysTick->VAL)     //  到达延时的极限
        {
            return ;
        }
        if (SysTick->VAL > last_val)     // 防止没有抓到极限值，定时器就复位的情况
        {
            return ;
        }
        last_val = SysTick->VAL ;
    }
}

BSP_SYSTEMCLOCK_FUN_typestruct BSP_SYSTEMCLOCK_FUN =
{
    .Init = FUN_SYSTEMCLOCK_Init,
    .Delay_us = BSP_FUN_Delay_us,
} ;
