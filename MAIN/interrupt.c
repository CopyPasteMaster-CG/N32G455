
#include "N32G45x.h"

void SysTick_Handler(void)
{
    extern void rt_os_tick_callback(void);
    rt_os_tick_callback();
}


static uint8_t rx_mode = 0; // 0=align 1=sample



__weak void SPI1_IRQ_TE_Callback(void)
{
    (void)(0);
}
__weak void SPI1_IRQ_RNE_Callback(void)
{
    (void)(0);
}
void SPI1_IRQHandler(void)
{
    if (SPI_I2S_GetIntStatus(SPI1, SPI_I2S_INT_TE) != RESET){
//        SPI_I2S_EnableInt(SPI1, SPI_I2S_INT_TE, DISABLE);
        SPI1_IRQ_TE_Callback();
    }
    if (SPI_I2S_GetIntStatus(SPI1, SPI_I2S_INT_RNE) != RESET){
//        SPI_I2S_EnableInt(SPI1, SPI_I2S_INT_RNE, DISABLE);
        SPI1_IRQ_RNE_Callback();
    }
    //  不响应错误中断
    SPI1->CTRL2&=(~0x02);
}

__weak void DMA1_CH3_IRQ_HT_Callback(void)
{
}
__weak void DMA1_CH3_IRQ_TC_Callback(void)
{
}
void DMA1_Channel3_IRQHandler(void)
{
    //  传输一半
    if(DMA_GetFlagStatus(DMA1_FLAG_HT3, DMA1)){
        DMA_ClearFlag(DMA1_FLAG_HT3, DMA1);
        DMA1_CH3_IRQ_HT_Callback();
    }
    //  传输完成
    if(DMA_GetFlagStatus(DMA1_FLAG_TC3, DMA1)){
        DMA_ClearFlag(DMA1_FLAG_TC3, DMA1);
        DMA1_CH3_IRQ_TC_Callback();
    }
    DMA_ClearFlag(DMA1_FLAG_GL3|DMA1_FLAG_TE3, DMA1);
}

__weak void USART1_IRQ_RXD_Callback(uint8_t da)
{
}
void USART1_IRQHandler(void)
{
    //  接收缓冲区非空
    if(USART_GetIntStatus(USART1, USART_INT_RXDNE)){
        USART_ClrIntPendingBit(USART1,USART_INT_RXDNE);
        USART1_IRQ_RXD_Callback(USART1->DAT);
    }
    if(USART_GetIntStatus(USART1, USART_INT_OREF)){
        (void)USART1->STS;
        (void)USART1->DAT;
    }
    
}





__weak void TIM2_IRQ_CC4_Callback(void)
{
}
void TIM2_IRQHandler(void)
{
    if(TIM_GetIntStatus(TIM2, TIM_INT_CC4)){
        TIM_ClrIntPendingBit(TIM2, TIM_INT_CC2);
        TIM2_IRQ_CC4_Callback();
    }
}


__weak void EXTI15_10_IRQHandler(void)
{
}


__weak void TIM3_IRQHandler(void)
{

}