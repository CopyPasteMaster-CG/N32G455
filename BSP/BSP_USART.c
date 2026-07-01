
#include "BSP_USART.h"

#include "BSP.h"

#include "string.h"

static BSP_GPIO_INFO_typedefstruct const GPIO_TX =
{
    .mode = BSP_GPIO_MODE_AF_PP,
    .option = 0,
    .num = GPIO_PIN_9,
    .port = GPIOA,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;

static BSP_GPIO_INFO_typedefstruct const GPIO_RX =
{
    .mode = BSP_GPIO_MODE_INPUT,
    .option = 0,
    .num = GPIO_PIN_10,
    .port = GPIOA,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;

static USART_Module * const module = USART1;



static BSP_USART_RECEIVE_IRQ_Callback UART_RECEIVE_DATA_CALLBACK = NULL;

static void FUN_USART_Init(uint32_t baud,BSP_USART_RECEIVE_IRQ_Callback rx_callback)
{
    USART_InitType Structure;
    NVIC_InitType NVIC_InitStructure={0};
    
    BSP_FUN_GPIO.Init(&GPIO_TX);
    BSP_FUN_GPIO.Init(&GPIO_RX);
    GPIO_ConfigPinRemap(GPIO_RMP_USART1, DISABLE);
    
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_USART1, ENABLE);
    
    USART_StructInit(&Structure);
    Structure.BaudRate = baud;
    Structure.HardwareFlowControl = USART_HFCTRL_NONE;
    Structure.Mode = USART_MODE_RX | USART_MODE_TX;
    Structure.Parity = USART_PE_NO;
    Structure.StopBits = USART_STPB_1;
    Structure.WordLength = USART_WL_8B;
    USART_Init(module, &Structure);
    USART_Enable(module, ENABLE);
    UART_RECEIVE_DATA_CALLBACK=rx_callback;
    USART_ConfigInt(module, USART_INT_RXDNE, ENABLE);   //  开启接收缓冲区非空中断
    
    
    NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void USART1_IRQ_RXD_Callback(uint8_t da)
{
    if(UART_RECEIVE_DATA_CALLBACK){
        UART_RECEIVE_DATA_CALLBACK(da);
    }
}

static void FUN_USART_Write( void * data, uint32_t len )
{
    for(uint32_t i=0; i<len; i++){
        while(USART_GetFlagStatus(module, USART_FLAG_TXDE) == RESET){
            ;
        }
        
        USART_SendData(module, ((uint8_t*)data)[i]);
    }
}



BSP_USART_FUN_typestruct BSP_USART_FUN =
{
    .Init = FUN_USART_Init,
    .Write = FUN_USART_Write,
} ;
