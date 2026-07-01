
#include "BSP_NEC.h"

#include "BSP.h"

#include "string.h"

static BSP_GPIO_INFO_typedefstruct const GPIO_RX =
{
    .mode = BSP_GPIO_MODE_INPUT,
    .option = 0,
    .num = GPIO_PIN_3,
    .port = GPIOA,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;

static TIM_Module * const module = TIM2;



static BSP_NEC_RECEIVE_IRQ_Callback NEC_CALLBACK = NULL;

static void FUN_NEC_Init(BSP_NEC_RECEIVE_IRQ_Callback rx_callback)
{
    TIM_ICInitType Structure;
    TIM_TimeBaseInitType base;
    NVIC_InitType NVIC_InitStructure={0};
    
    BSP_FUN_GPIO.Init(&GPIO_RX);
    GPIO_ConfigPinRemap(GPIO_PartialRemap1_TIM2, DISABLE);
    
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TIM2, ENABLE);
    
    TIM_InitTimBaseStruct(&base);
    base.ClkDiv     =   TIM_CLK_DIV1;       //  tDTS = tCK_INT
    base.CntMode    =   TIM_CNT_MODE_UP;    //  向上计数
    base.Period     =   65535;              //  自动重装载值，设置为最大
    base.Prescaler  =   72-1;               //  主时钟频率是72M，需要进行72分频
    TIM_InitTimeBase(module, &base);
    
    TIM_InitIcStruct(&Structure);
    Structure.Channel       =   TIM_CH_3;                       //  捕获通道3，内部通道
    Structure.IcFilter      =   0;                              //  无滤波器，以fDTS采样
    Structure.IcPolarity    =   TIM_IC_POLARITY_FALLING;        //  下降沿
    Structure.IcPrescaler   =   TIM_IC_PSC_DIV1;                //  无预分频器，捕获输入口上检测到的每一个边沿都触发一次捕获；
    Structure.IcSelection   =   TIM_IC_SELECTION_INDIRECTTI;    //  交叉连接，既捕获通道3与定时器的物理接口4交叉连接
    TIM_ICInit(module, &Structure);
    
    TIM_InitIcStruct(&Structure);
    Structure.Channel       =   TIM_CH_4;                       //  捕获通道4，内部通道
    Structure.IcFilter      =   0;                              //  无滤波器，以fDTS采样
    Structure.IcPolarity    =   TIM_IC_POLARITY_RISING;         //  上升沿
    Structure.IcPrescaler   =   TIM_IC_PSC_DIV1;                //  无预分频器，捕获输入口上检测到的每一个边沿都触发一次捕获；
    Structure.IcSelection   =   TIM_IC_SELECTION_DIRECTTI;      //  直接连接，既捕获通道4与定时器的物理接口4直接连接
    TIM_ICInit(module, &Structure);
    
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_Enable(module, ENABLE);
    NEC_CALLBACK=rx_callback;
    TIM_ConfigInt(module, TIM_INT_CC4, ENABLE);   //  允许捕获/比较4中断，因为空闲电平为高，所以传输的结束总是下降沿。又因为捕获通道4对应上升沿，所以只要开启IC4的中断即可一次中断读取到两个数据
}

void TIM2_IRQ_CC4_Callback(void)
{
    if(NEC_CALLBACK){
        NEC_CALLBACK(module->CCDAT3, module->CCDAT4);
    }
}


BSP_NEC_FUN_typestruct BSP_NEC_FUN =
{
    .Init = FUN_NEC_Init,
} ;
