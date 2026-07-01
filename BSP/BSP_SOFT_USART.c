#include "BSP_SOFT_USART.h"

#include "stdio.h"
#include "string.h"
#include "n32g45x_exti.h"
#include "BSP_USART.h"

#define SOFT_UART_HALF_BIT_US 103
#define SOFT_UART_BIT_US      207
#define SOFT_RX_BUF_SIZE      64

typedef enum {
    RX_IDLE,
    RX_START_CHECK,
    RX_DATA,
    RX_STOP
} rx_state_t;

typedef enum {
    TX_IDLE,
    TX_START,
    TX_DATA,
    TX_STOP
} tx_state_t;

typedef enum {
    TIM3_OWNER_NONE,
    TIM3_OWNER_RX,
    TIM3_OWNER_TX
} tim3_owner_t;

static uint8_t rx_byte;
static uint8_t rx_bit_index;
static volatile rx_state_t rx_state = RX_IDLE;
static volatile uint8_t soft_rx_buf[SOFT_RX_BUF_SIZE];
static volatile uint8_t soft_rx_head;
static volatile uint8_t soft_rx_tail;
static volatile uint8_t soft_rx_overflow;

static BSP_SOFT_USART_RX_NOTIFY soft_rx_notify;

static uint8_t *tx_buf;
static volatile uint16_t tx_len;
static volatile uint16_t tx_index;
static volatile uint8_t tx_byte;
static volatile uint8_t tx_bit_index;
static volatile tx_state_t tx_state = TX_IDLE;
static volatile tim3_owner_t tim3_owner = TIM3_OWNER_NONE;

static void UART_RX_EXTI_Enable(FunctionalState cmd);
static void UART_TX_Sample(void);
static void UART_RX_Sample(void);
static void UART_RX_PutByte(uint8_t data);





void UART_TX_SendArray(uint8_t *buf, uint16_t len)
{
    if(len == 0) return;

    while((tx_state != TX_IDLE) || (rx_state != RX_IDLE))
    {
        rt_thread_mdelay(1);;
    }

    tim3_owner = TIM3_OWNER_TX;
    UART_RX_EXTI_Enable(DISABLE);

    tx_buf = buf;
    tx_len = len;
    tx_index = 0;
    tx_byte = tx_buf[tx_index++];
    tx_bit_index = 0;
    tx_state = TX_DATA;

    GPIO_ResetBits(GPIOA, GPIO_PIN_1);

    TIM_Enable(TIM3, DISABLE);
    TIM_SetAutoReload(TIM3, SOFT_UART_BIT_US);
    TIM_SetCnt(TIM3, 0);
    TIM_ClrIntPendingBit(TIM3, TIM_INT_UPDATE);
    TIM_Enable(TIM3, ENABLE);

    while(tx_state != TX_IDLE)
    {
        rt_thread_mdelay(1);;
    }
}


static void UART_RX_PutByte(uint8_t data)
{
    uint8_t was_empty;
    uint8_t next;
    BSP_SOFT_USART_RX_NOTIFY notify = RT_NULL;

    next = (uint8_t)(soft_rx_head + 1);
    if(next >= SOFT_RX_BUF_SIZE)
    {
        next = 0;
    }

    if(next == soft_rx_tail)
    {
        soft_rx_overflow = 1;
        return;
    }

    was_empty = (soft_rx_head == soft_rx_tail);//为空

    soft_rx_buf[soft_rx_head] = data;
    soft_rx_head = next;

    if(was_empty)
    {
        notify = soft_rx_notify;
    }

    if(notify != RT_NULL)
    {
        notify();
    }
}
static void TIM3_Init(void)
{
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TIM3, ENABLE);

    TIM_TimeBaseInitType TIM_InitStructure;

    TIM_InitStructure.Prescaler = 71;     // 1MHz = 1us
    TIM_InitStructure.Period    = SOFT_UART_BIT_US;    // 104us
    TIM_InitStructure.CntMode      = TIM_CNT_MODE_UP;
    TIM_InitStructure.ClkDiv    = TIM_CLK_DIV1;

    TIM_InitTimeBase(TIM3, &TIM_InitStructure);

    NVIC_InitType NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    /* 更新中断 */

    TIM_ConfigInt(TIM3, TIM_INT_UPDATE, ENABLE);

    TIM_ClrIntPendingBit(TIM3, TIM_INT_UPDATE); 

    NVIC_EnableIRQ(TIM3_IRQn);
}

static void UART_RX_EXTI_Enable(FunctionalState cmd)
{
    EXTI_InitType EXTI_InitStructure;

    if(cmd == ENABLE)
    {
        EXTI_ClrITPendBit(EXTI_LINE15);
        NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
    }

    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_LineCmd = cmd;
    EXTI_InitStructure.EXTI_Line    = EXTI_LINE15;
    EXTI_InitPeripheral(&EXTI_InitStructure);

    if(cmd == DISABLE)
    {
        EXTI_ClrITPendBit(EXTI_LINE15);
        NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
    }
}


static void UART_RX_Start(void)
{
    tim3_owner = TIM3_OWNER_RX;
    UART_RX_EXTI_Enable(DISABLE);

    rx_state = RX_START_CHECK;
    rx_bit_index = 0;
    rx_byte = 0;

    TIM_Enable(TIM3, DISABLE);
    TIM_SetAutoReload(TIM3, SOFT_UART_HALF_BIT_US);
    TIM_SetCnt(TIM3, 0);
    TIM_ClrIntPendingBit(TIM3, TIM_INT_UPDATE);
    TIM_Enable(TIM3, ENABLE);
}


void UART_TX_Send(uint8_t data)
{
    static uint8_t tx_single_byte;

    tx_single_byte = data;
    UART_TX_SendArray(&tx_single_byte, 1);
}

static void UART_TX_Sample(void)
{
    switch(tx_state)
    {
        case TX_START:
            GPIO_ResetBits(GPIOA, GPIO_PIN_1); 
            tx_state = TX_DATA;
            tx_bit_index = 0;
            break;
        case TX_DATA:
            {
                if(tx_byte & (1 << tx_bit_index))
                    GPIO_SetBits(GPIOA, GPIO_PIN_1);
                else
                    GPIO_ResetBits(GPIOA, GPIO_PIN_1);

                tx_bit_index++;

                if(tx_bit_index >= 8)
                    tx_state = TX_STOP;
            }
            break;
        case TX_STOP:
            GPIO_SetBits(GPIOA, GPIO_PIN_1); // stop bit
            /* 还有数据 */
            if(tx_index < tx_len)
            {
                tx_byte = tx_buf[tx_index++];
                tx_bit_index = 0;
                tx_state = TX_START;
            }
            else
            {
                /* 发送结束 */
                tx_state = TX_IDLE;
                tim3_owner = TIM3_OWNER_NONE;
                TIM_Enable(TIM3,DISABLE);
                UART_RX_EXTI_Enable(ENABLE);
            }
            break;
                    default:
            tx_state = TX_IDLE;
            TIM_Enable(TIM3,DISABLE);
            tim3_owner = TIM3_OWNER_NONE;
            UART_RX_EXTI_Enable(ENABLE);
            break;
    }
}
static void UART_RX_Sample(void)
{
    if(rx_state == RX_IDLE)
    {
        tim3_owner = TIM3_OWNER_NONE;
        TIM_Enable(TIM3, DISABLE);
        return;
    }

    uint8_t bit = GPIO_ReadInputDataBit(GPIOC, GPIO_PIN_15);
    if(rx_state == RX_START_CHECK)
    {
       if(bit != 0)
       {
           rx_state = RX_IDLE;
           tim3_owner = TIM3_OWNER_NONE;
           TIM_Enable(TIM3,DISABLE);
           UART_RX_EXTI_Enable(ENABLE);
           return;
       }

        /* 进入数据采样 */
        rx_state = RX_DATA;

        rx_bit_index = 0;
        /* 切换104us采样 */
        TIM_SetAutoReload(TIM3, SOFT_UART_BIT_US);
        return;
    }
    if(rx_state == RX_DATA)
    {
        rx_byte |= (bit << rx_bit_index);
        rx_bit_index++;

        if(rx_bit_index >= 8)
        {
            rx_state = RX_STOP;
        }
        return;
    }
   if(rx_state == RX_STOP)
    {

       if(bit != 0U)
        {
            UART_RX_PutByte(rx_byte);
        }

        rx_state = RX_IDLE;
        TIM_Enable(TIM3, DISABLE);
        tim3_owner = TIM3_OWNER_NONE;
        UART_RX_EXTI_Enable(ENABLE);
    }
}

void EXTI15_10_IRQHandler(void)
{
     if(EXTI_GetITStatus(EXTI_LINE15) != RESET)
    {
        EXTI_ClrITPendBit(EXTI_LINE15);
        if(rx_state == RX_IDLE)
        {
            UART_RX_Start();
        }
    }
}


void TIM3_IRQHandler(void)
{
    if(TIM_GetIntStatus(TIM3, TIM_INT_UPDATE) != RESET)
    {
        TIM_ClrIntPendingBit(TIM3, TIM_INT_UPDATE);

        switch(tim3_owner)
        {
            case TIM3_OWNER_TX:
                UART_TX_Sample();
                break;

            case TIM3_OWNER_RX:
                UART_RX_Sample();
                break;

            default:
                TIM_Enable(TIM3, DISABLE);
                break;
        }
    }
}

static void FUN_SOFT_UART_Init(void)
{    
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOC, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
    GPIO_InitType GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.Pin        =GPIO_PIN_1;
    GPIO_InitPeripheral(GPIOA,&GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_PIN_1);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU ;
    GPIO_InitStructure.GPIO_Speed = GPIO_INPUT;
    GPIO_InitStructure.Pin        =GPIO_PIN_15;
    GPIO_InitPeripheral(GPIOC,&GPIO_InitStructure);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);

    GPIO_ConfigEXTILine(GPIOC_PORT_SOURCE, GPIO_PIN_SOURCE15);

    EXTI_InitType EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Falling ;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Line    = EXTI_LINE15;
    EXTI_InitPeripheral(&EXTI_InitStructure);

    NVIC_InitType NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM3_Init();
		TIM_Enable(TIM3,DISABLE);
}

char str[100];
static int FUN_SOFT_UART_Read(uint8_t *data)
{
    if(data == RT_NULL)
    {
        return 0;
    }

    if(soft_rx_head == soft_rx_tail)
    {
        return 0;
    }

    __disable_irq();
    *data = soft_rx_buf[soft_rx_tail];
    soft_rx_tail = (uint8_t)((soft_rx_tail + 1) % SOFT_RX_BUF_SIZE);
    __enable_irq();

    return 1;
}

static void FUN_SOFT_UART_SetRxNotify(BSP_SOFT_USART_RX_NOTIFY notify)
{
    __disable_irq();
    soft_rx_notify = notify;
    __enable_irq();
}
static void FUN_SOFT_UART_Poll(void)
{
    uint8_t data;
    uint8_t overflow;

    while(FUN_SOFT_UART_Read(&data))
    {
        sprintf(str, " %02X", data);
        BSP_USART_FUN.Write(str, strlen(str));
    }

    __disable_irq();
    overflow = soft_rx_overflow;
    soft_rx_overflow = 0;
    __enable_irq();

    if(overflow)
    {
        BSP_USART_FUN.Write("\r\n SOFT RX OVERFLOW\r\n", 22);
    }

}

BSP_SOFT_USART_FUN_typestruct BSP_SFOT_USART_FUN =
{
    .Init = FUN_SOFT_UART_Init,
    .Write = UART_TX_SendArray,
    .Poll = FUN_SOFT_UART_Poll,
    .Read = FUN_SOFT_UART_Read,
    .SetRxNotify = FUN_SOFT_UART_SetRxNotify,
} ;
