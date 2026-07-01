
#include "BSP.h"

#include "stdbool.h"
#include "string.h"


static inline bool ___FUN_GPIO_CLK_ENABLE(BSP_GPIO_INFO_typedefstruct const * const info)
{
    switch ((uint32_t)(info->port))
    {
#if defined ( GPIOA )
    case ((uint32_t)(GPIOA_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
        break ;
#endif

#if defined ( GPIOB )
    case ((uint32_t)(GPIOB_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
        break ;
#endif

#if defined ( GPIOC )
    case ((uint32_t)(GPIOC_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOC, ENABLE);
        break ;
#endif

#if defined ( GPIOD )
    case ((uint32_t)(GPIOD_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOD, ENABLE);
        break ;
#endif

#if defined ( GPIOE )
    case ((uint32_t)(GPIOE_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOE, ENABLE);
        break ;
#endif

#if defined ( GPIOF )
    case ((uint32_t)(GPIOF_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOF, ENABLE);
        break ;
#endif

#if defined ( GPIOG )
    case ((uint32_t)(GPIOG_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOG, ENABLE);
        break ;
#endif
#if defined ( GPIOH )
    case ((uint32_t)(GPIOH_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOH, ENABLE);
        break ;
#endif

#if defined ( GPIOI )
    case ((uint32_t)(GPIOI_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOI, ENABLE);
        break ;
#endif

#if defined ( GPIOJ )
    case ((uint32_t)(GPIOJ_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOJ, ENABLE);
        break ;
#endif

#if defined ( GPIOK )
    case ((uint32_t)(GPIOK_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOK, ENABLE);
        break ;
#endif

#if defined ( GPIOL )
    case ((uint32_t)(GPIOL_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOL, ENABLE);
        break ;
#endif

#if defined ( GPIOM )
    case ((uint32_t)(GPIOM_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOM, ENABLE);
        break ;
#endif

#if defined ( GPION )
    case ((uint32_t)(GPION_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPION, ENABLE);
        break ;
#endif
#if defined ( GPIOO )
    case ((uint32_t)(GPIOO_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOO, ENABLE);
        break ;
#endif

#if defined ( GPIOP )
    case ((uint32_t)(GPIOP_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOP, ENABLE);
        break ;
#endif

#if defined ( GPIOQ )
    case ((uint32_t)(GPIOQ_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOQ, ENABLE);
        break ;
#endif

#if defined ( GPIOR )
    case ((uint32_t)(GPIOR_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOR, ENABLE);
        break ;
#endif

#if defined ( GPIOS )
    case ((uint32_t)(GPIOS_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOS, ENABLE);
        break ;
#endif

#if defined ( GPIOT )
    case ((uint32_t)(GPIOI_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOT, ENABLE);
        break ;
#endif

#if defined ( GPIOU )
    case ((uint32_t)(GPIOU_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOU, ENABLE);
        break ;
#endif
#if defined ( GPIOV )
    case ((uint32_t)(GPIOV_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOV, ENABLE);
        break ;
#endif

#if defined ( GPIOW )
    case ((uint32_t)(GPIOW_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOW, ENABLE);
        break ;
#endif

#if defined ( GPIOX )
    case ((uint32_t)(GPIOX_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOX, ENABLE);
        break ;
#endif

#if defined ( GPIOY )
    case ((uint32_t)(GPIOY_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOY, ENABLE);
        break ;
#endif

#if defined ( GPIOZ )
    case ((uint32_t)(GPIOZ_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOZ, ENABLE);
        break ;
#endif

#if defined ( GPIO0 )
    case ((uint32_t)(GPIO0_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIO0, ENABLE);
        break ;
#endif

#if defined ( GPIO1 )
    case ((uint32_t)(GPIO1_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIO1, ENABLE);
        break ;
#endif
#if defined ( GPIO2 )
    case ((uint32_t)(GPIO2_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIO2, ENABLE);
        break ;
#endif

#if defined ( GPIO3 )
    case ((uint32_t)(GPIO3_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIO3, ENABLE);
        break ;
#endif

#if defined ( GPIO4 )
    case ((uint32_t)(GPIO4_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIO4, ENABLE);
        break ;
#endif

#if defined ( GPIO5 )
    case ((uint32_t)(GPIO5_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIO5, ENABLE);
        break ;
#endif

#if defined ( GPIO6 )
    case ((uint32_t)(GPIO6_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIO6, ENABLE);
        break ;
#endif

#if defined ( GPIO7 )
    case ((uint32_t)(GPIO7_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIO7, ENABLE);
        break ;
#endif

#if defined ( GPIO8 )
    case ((uint32_t)(GPIO8_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIO8, ENABLE);
        break ;
#endif
#if defined ( GPIO9 )
    case ((uint32_t)(GPIO9_BASE)) :
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIO9, ENABLE);
        break ;
#endif


    default :
        return false ;
    }
    return true ;
}
static BSP_GPIO_IRQ_Callback_typedef gpio_irq_callback [ 16 ] ;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    for (uint8_t i = 0 ; i < (sizeof(gpio_irq_callback) / sizeof(gpio_irq_callback [ 0 ])) ; i++)
    {
        if ((GPIO_Pin & (0x01 << i)) && (gpio_irq_callback [ i ] != NULL))
        {
            gpio_irq_callback [ i ]() ;
        }
    }
}
static void FUN_GPIO_Init(BSP_GPIO_INFO_typedefstruct const * const info)
{
    GPIO_InitType InitStruct = { 0 } ;
    uint8_t irq_flag  = 0 ;

    GPIO_InitStruct(&InitStruct);
    switch (info->mode)
    {
    case BSP_GPIO_MODE_INPUT :
            switch (info->pull)
            {
            case BSP_GPIO_PULL_UP :
                InitStruct.GPIO_Mode = GPIO_Mode_IPU ;
                break ;

            case BSP_GPIO_PULL_DOWN :
                InitStruct.GPIO_Mode = GPIO_Mode_IPD ;
                break ;

            case BSP_GPIO_PULL_NO :
                InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING ;
                break ;
            default :
                return ;
            }
        break ;

    case BSP_GPIO_MODE_OUTPUT_PP :
        InitStruct.GPIO_Mode = GPIO_Mode_Out_PP ;
        break ;

    case BSP_GPIO_MODE_OUTPUT_OD :
        InitStruct.GPIO_Mode = GPIO_Mode_Out_OD ;
        break ;

    case BSP_GPIO_MODE_AF_PP :
        InitStruct.GPIO_Mode = GPIO_Mode_AF_PP ;
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
        break ;

    case BSP_GPIO_MODE_AF_OD :
        InitStruct.GPIO_Mode = GPIO_Mode_AF_OD ;
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
        break ;

//    case BSP_GPIO_MODE_AF_INPUT :
//        InitStruct.GPIO_Mode = GPIO_MODE_AF_INPUT ;
//        break ;

    case BSP_GPIO_MODE_ANALOG :
        InitStruct.GPIO_Mode = GPIO_Mode_AIN ;
        break ;

//    case BSP_GPIO_MODE_IT_RISING :
//        InitStruct.GPIO_Mode = GPIO_MODE_IT_RISING ;
//        irq_flag = 1 ;
//        break ;

   case BSP_GPIO_MODE_IT_FALLING :
       InitStruct.GPIO_Mode = GPIO_Mode_IPU ;
       irq_flag = 1 ;
       break ;

//    case BSP_GPIO_MODE_IT_RISING_FALLING :
//        InitStruct.GPIO_Mode = GPIO_MODE_IT_RISING_FALLING ;
//        irq_flag = 1 ;
//        break ;

//    case BSP_GPIO_MODE_EVT_RISING :
//        InitStruct.GPIO_Mode = GPIO_MODE_EVT_RISING ;
//        break ;

//    case BSP_GPIO_MODE_EVT_FALLING :
//        InitStruct.GPIO_Mode = GPIO_MODE_EVT_FALLING ;
//        break ;

//    case BSP_GPIO_MODE_EVT_RISING_FALLING :
//        InitStruct.GPIO_Mode = GPIO_MODE_EVT_RISING_FALLING ;
//        break ;

    default :
        return ;
    }

//    switch (info->pull)
//    {
//    case BSP_GPIO_PULL_UP :
//        InitStruct.Pull = GPIO_PULLUP ;
//        break ;

//    case BSP_GPIO_PULL_DOWN :
//        InitStruct.Pull = GPIO_PULLDOWN ;
//        break ;

//    case BSP_GPIO_PULL_NO :
//        InitStruct.Pull = GPIO_NOPULL ;
//        break ;
//    default :
//        return ;
//    }

    if( (info->mode == BSP_GPIO_MODE_INPUT) || (info->mode == BSP_GPIO_MODE_ANALOG)){
        InitStruct.GPIO_Speed = GPIO_INPUT ;
    }else{
        switch (info->speed)
        {
        case BSP_GPIO_SPEED_HIGH :
            InitStruct.GPIO_Speed = GPIO_Speed_50MHz ;
            break ;

        case BSP_GPIO_SPEED_MID :
            InitStruct.GPIO_Speed = GPIO_Speed_10MHz ;
            break ;

        case BSP_GPIO_SPEED_LOW :
            InitStruct.GPIO_Speed = GPIO_Speed_2MHz ;
            break ;
        default :
            return ;
        }
    }

    if (___FUN_GPIO_CLK_ENABLE(info) == false)
    {
        return ;
    }
    do
    {
        int i = (sizeof(gpio_irq_callback) / sizeof(gpio_irq_callback [ 0 ])) - 1 ;
        for (; i >= 0 ; i--)
        {
            if (info->num == (0x01 << i))
            {
                break ;
            }
        }
        if (i < 0)
        {
            return ;
        }
        else if (irq_flag != 0)
        {
            if (gpio_irq_callback [ i ] != NULL)
            {
                return ;
            }
            else
            {
                gpio_irq_callback [ i ] = info->irq_callback ;
            }
        }
    }
    while (0) ;
    InitStruct.Pin = info->num ;
    if((info->mode==BSP_GPIO_MODE_OUTPUT_PP) || (info->mode==BSP_GPIO_MODE_OUTPUT_OD)){
        if(info->option){
            BSP_FUN_GPIO.Set(info);
        }else{
            BSP_FUN_GPIO.Reset(info);
        }
    }
    GPIO_InitPeripheral(info->port, &InitStruct) ;

}

static void FUN_GPIO_DeInit(BSP_GPIO_INFO_typedefstruct const * const info)
{
    GPIO_InitType InitStruct = { 0 } ;
    InitStruct.Pin        = info->num;
    InitStruct.GPIO_Speed = GPIO_INPUT;
    InitStruct.GPIO_Mode  = GPIO_Mode_AIN;
    GPIO_InitPeripheral(info->port, &InitStruct) ;
}

static void FUN_GPIO_Set(BSP_GPIO_INFO_typedefstruct const * const info)
{
    GPIO_SetBits(info->port, info->num) ;
}

static void FUN_GPIO_Reset(BSP_GPIO_INFO_typedefstruct const * const info)
{
    GPIO_ResetBits(info->port, info->num) ;
}

static void FUN_GPIO_Toggle(BSP_GPIO_INFO_typedefstruct const * const info)
{
    if(GPIO_ReadOutputDataBit(info->port, info->num)){
        GPIO_ResetBits(info->port, info->num) ;
    }else{
        GPIO_SetBits(info->port, info->num) ;
    }
}

static char FUN_GPIO_Read(BSP_GPIO_INFO_typedefstruct const * const info)
{
    return GPIO_ReadInputDataBit(info->port, info->num) != Bit_RESET ;
}

BSP_FUN_GPIO_typestruct BSP_FUN_GPIO =
{
    .Init = FUN_GPIO_Init,
    .DeInit = FUN_GPIO_DeInit,
    .Set = FUN_GPIO_Set,
    .Reset = FUN_GPIO_Reset,
    .Toggle = FUN_GPIO_Toggle,
    .Read = FUN_GPIO_Read,
} ;

