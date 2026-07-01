
#include "BSP_LED.h"

#include "BSP.h"

static BSP_GPIO_INFO_typedefstruct const GPIO_LED_TX1 =
{
    .mode = BSP_GPIO_MODE_OUTPUT_PP,
    .option = 0,
    .num = GPIO_PIN_3,
    .port = GPIOC,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;

static BSP_GPIO_INFO_typedefstruct const GPIO_LED_TX2 =
{
    .mode = BSP_GPIO_MODE_OUTPUT_PP,
    .option = 0,
    .num = GPIO_PIN_1,
    .port = GPIOA,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;

static BSP_GPIO_INFO_typedefstruct const GPIO_LED_TX3 =
{
    .mode = BSP_GPIO_MODE_OUTPUT_PP,
    .option = 0,
    .num = GPIO_PIN_2,
    .port = GPIOA,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;

static BSP_GPIO_INFO_typedefstruct const GPIO_LED_TX4 =
{
    .mode = BSP_GPIO_MODE_OUTPUT_PP,
    .option = 0,
    .num = GPIO_PIN_4,
    .port = GPIOB,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;

static BSP_GPIO_INFO_typedefstruct const GPIO_LED_RX =
{
    .mode = BSP_GPIO_MODE_OUTPUT_PP,
    .option = 0,
    .num = GPIO_PIN_0,
    .port = GPIOB,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;


static void FUN_LED_Init(void)
{
    BSP_FUN_GPIO.Set(&GPIO_LED_TX1) ;
    BSP_FUN_GPIO.Set(&GPIO_LED_TX2) ;
    BSP_FUN_GPIO.Set(&GPIO_LED_TX3) ;
    BSP_FUN_GPIO.Set(&GPIO_LED_TX4) ;
    BSP_FUN_GPIO.Set(&GPIO_LED_RX) ;
    
    BSP_FUN_GPIO.Init(&GPIO_LED_TX1) ;
    BSP_FUN_GPIO.Init(&GPIO_LED_TX2) ;
    BSP_FUN_GPIO.Init(&GPIO_LED_TX3) ;
    BSP_FUN_GPIO.Init(&GPIO_LED_TX4) ;
    BSP_FUN_GPIO.Init(&GPIO_LED_RX) ;
    
    BSP_FUN_GPIO.Set(&GPIO_LED_TX1) ;
    BSP_FUN_GPIO.Set(&GPIO_LED_TX2) ;
    BSP_FUN_GPIO.Set(&GPIO_LED_TX3) ;
    BSP_FUN_GPIO.Set(&GPIO_LED_TX4) ;
    BSP_FUN_GPIO.Set(&GPIO_LED_RX) ;
}

//static void FUN_LED_Read(uint8_t * state)
//{
//    *state=0;
//    *state |= (BSP_FUN_GPIO.Read(&GPIO_LED_RX)?BSP_LED_NUM_RX:0x00);
//    *state |= (BSP_FUN_GPIO.Read(&GPIO_LED_TX1)?BSP_LED_NUM_TX1:0x00);
//    *state |= (BSP_FUN_GPIO.Read(&GPIO_LED_TX2)?BSP_LED_NUM_TX2:0x00);
//    *state |= (BSP_FUN_GPIO.Read(&GPIO_LED_TX3)?BSP_LED_NUM_TX3:0x00);
//    *state |= (BSP_FUN_GPIO.Read(&GPIO_LED_TX4)?BSP_LED_NUM_TX4:0x00);
//}

//static void FUN_LED_Write(uint8_t state)
//{
//    state&BSP_LED_NUM_RX?BSP_FUN_GPIO.Reset(&GPIO_LED_RX):BSP_FUN_GPIO.Set(&GPIO_LED_RX);
//    state&BSP_LED_NUM_TX1?BSP_FUN_GPIO.Reset(&GPIO_LED_TX1):BSP_FUN_GPIO.Set(&GPIO_LED_TX1);
//    state&BSP_LED_NUM_TX2?BSP_FUN_GPIO.Reset(&GPIO_LED_TX2):BSP_FUN_GPIO.Set(&GPIO_LED_TX2);
//    state&BSP_LED_NUM_TX3?BSP_FUN_GPIO.Reset(&GPIO_LED_TX3):BSP_FUN_GPIO.Set(&GPIO_LED_TX3);
//    state&BSP_LED_NUM_TX4?BSP_FUN_GPIO.Reset(&GPIO_LED_TX4):BSP_FUN_GPIO.Set(&GPIO_LED_TX4);
//}

//static void FUN_LED_On(uint8_t state)
//{
//    state&BSP_LED_NUM_RX?BSP_FUN_GPIO.Reset(&GPIO_LED_RX):(void)(0);
//    state&BSP_LED_NUM_TX1?BSP_FUN_GPIO.Reset(&GPIO_LED_TX1):(void)(0);
//    state&BSP_LED_NUM_TX2?BSP_FUN_GPIO.Reset(&GPIO_LED_TX2):(void)(0);
//    state&BSP_LED_NUM_TX3?BSP_FUN_GPIO.Reset(&GPIO_LED_TX3):(void)(0);
//    state&BSP_LED_NUM_TX4?BSP_FUN_GPIO.Reset(&GPIO_LED_TX4):(void)(0);
//}

//static void FUN_LED_Off(uint8_t state)
//{
//    state&BSP_LED_NUM_RX?BSP_FUN_GPIO.Set(&GPIO_LED_RX):(void)(0);
//    state&BSP_LED_NUM_TX1?BSP_FUN_GPIO.Set(&GPIO_LED_TX1):(void)(0);
//    state&BSP_LED_NUM_TX2?BSP_FUN_GPIO.Set(&GPIO_LED_TX2):(void)(0);
//    state&BSP_LED_NUM_TX3?BSP_FUN_GPIO.Set(&GPIO_LED_TX3):(void)(0);
//    state&BSP_LED_NUM_TX4?BSP_FUN_GPIO.Set(&GPIO_LED_TX4):(void)(0);
//}

//static void FUN_LED_Toggle(uint8_t state)
//{
//    state&BSP_LED_NUM_RX?BSP_FUN_GPIO.Toggle(&GPIO_LED_RX):(void)(0);
//    state&BSP_LED_NUM_TX1?BSP_FUN_GPIO.Toggle(&GPIO_LED_TX1):(void)(0);
//    state&BSP_LED_NUM_TX2?BSP_FUN_GPIO.Toggle(&GPIO_LED_TX2):(void)(0);
//    state&BSP_LED_NUM_TX3?BSP_FUN_GPIO.Toggle(&GPIO_LED_TX3):(void)(0);
//    state&BSP_LED_NUM_TX4?BSP_FUN_GPIO.Toggle(&GPIO_LED_TX4):(void)(0);
//}
//  1号指示灯点亮
static void led_1_on(void)
{
    BSP_FUN_GPIO.Init(&GPIO_LED_TX1) ;  
    BSP_FUN_GPIO.Reset(&GPIO_LED_TX1); //  低电平 点亮led6，熄灭led8
 
}
//  1号指示灯熄灭
static void led_1_off(void)
{
   BSP_FUN_GPIO.DeInit(&GPIO_LED_TX1);  //  输入模式，两颗led都会熄灭
}
//  2号指示灯点亮
static void led_2_on(void)
{
    
    BSP_FUN_GPIO.Init(&GPIO_LED_TX1) ;//  输出模式，必定点亮1颗led
    BSP_FUN_GPIO.Set(&GPIO_LED_TX1);//  高电平 点亮led2，熄灭led1
    

}
//  2号指示灯熄灭
static void led_2_off(void)
{
   BSP_FUN_GPIO.DeInit(&GPIO_LED_TX1);   //  输入模式，两颗led都会熄灭
}
//  3号指示灯点亮
static void led_3_on(void)
{
    BSP_FUN_GPIO.Init(&GPIO_LED_TX4) ;
    BSP_FUN_GPIO.Set(&GPIO_LED_TX4);//  高电平 点亮led9，熄灭led7

}
//  3号指示灯熄灭
static void led_3_off(void)
{
   BSP_FUN_GPIO.DeInit(&GPIO_LED_TX4);  //  输入模式，两颗led都会熄灭
}
//  4号指示灯点亮
static void led_4_on(void)
{
    BSP_FUN_GPIO.Init(&GPIO_LED_TX4) ;
    BSP_FUN_GPIO.Reset(&GPIO_LED_TX4);//  低电平 点亮led7，熄灭led9
} 

//  4号指示灯熄灭
static void led_4_off(void)
{
   BSP_FUN_GPIO.DeInit(&GPIO_LED_TX4);  //  输入模式，两颗led都会熄灭
}

//  指示灯点亮
static void led_rx_on(void)
{
    BSP_FUN_GPIO.Init(&GPIO_LED_RX) ;
    BSP_FUN_GPIO.Reset(&GPIO_LED_RX);//  低电平 点亮led7，熄灭led9
}
//  4号指示灯熄灭
static void led_rx_off(void)
{
   BSP_FUN_GPIO.Set(&GPIO_LED_RX);  //  输入模式，两颗led都会熄灭
}
//  指示灯动作分组
static void (*led_state[][2])(void)={
    {led_1_off, led_1_on},
    {led_2_off, led_2_on},
    {led_3_off, led_3_on},
    {led_4_off, led_4_on},
    

};
//  指示灯状态
static uint8_t state[sizeof(led_state)/sizeof(led_state[0])];
//  刷新指示灯状态
static void led_io_refresh(void)
{
    static uint8_t set_point=0; //  组别，

    //  指示灯编号分组
    static const uint8_t point[][2]={
        {1-1,   2-1},
        {3-1,   4-1},
//        {5-1,   6-1},
//        {7-1,   8-1},
    };
    
    set_point=(set_point+1)%(sizeof(point[0])/sizeof(point[0][0]));  //  同一组内的编号切换
    
    led_state[point[0][set_point]][state[point[0][set_point]]]();   //  第0组别指示灯的状态
    led_state[point[1][set_point]][state[point[1][set_point]]]();   //  第1组别指示灯的状态
//    if(USB_LED_ENABLE){
//        led_state[point[2][set_point]][state[point[2][set_point]]]();   //  第2组别指示灯的状态
//        led_state[point[3][set_point]][state[point[3][set_point]]]();   //  第3组别指示灯的状态
//    }
}

static void led_io_on(BSP_LED_NUM_Typedefenum num)
{
    state[num]=1;
}

static void led_io_off(BSP_LED_NUM_Typedefenum num)
{
    state[num]=0;
}
static void led_io_toggle(BSP_LED_NUM_Typedefenum num)
{
    state[num]=!state[num];
}

BSP_LED_FUN_typestruct BSP_LED_FUN =
{
    .Init = FUN_LED_Init,
    .refresh=   led_io_refresh,
    .on     =   led_io_on,
    .off    =   led_io_off,
    .toggle =   led_io_toggle,
    .ledrxon=led_rx_on,
    .ledrxoff=led_rx_off,
//    .Read = FUN_LED_Read,
//    .Write = FUN_LED_Write,
//    .On = FUN_LED_On,
//    .Off = FUN_LED_Off,
//    .Toggle = FUN_LED_Toggle,
} ;
