
#ifndef __BSP_H__
#define __BSP_H__

#include "stdint.h"
#include "n32g45x.h"

//  端口模式
enum _BSP_GPIO_MODE_enum{
    BSP_GPIO_MODE_INPUT,               //  输入模式
    BSP_GPIO_MODE_OUTPUT_PP,           //  推挽输出模式
    BSP_GPIO_MODE_OUTPUT_OD,           //  开漏输出模式
    BSP_GPIO_MODE_AF_PP,               //  端口复用，推挽输出
    BSP_GPIO_MODE_AF_OD,               //  端口复用，开漏输出
    BSP_GPIO_MODE_AF_INPUT,            //  端口复用，输入
    BSP_GPIO_MODE_ANALOG,              //  模拟模式
    BSP_GPIO_MODE_IT_RISING,           //  上升沿触发中断模式
    BSP_GPIO_MODE_IT_FALLING,          //  下降沿触发中断模式
    BSP_GPIO_MODE_IT_RISING_FALLING,   //  上升或者下降沿都能触发中断模式
    BSP_GPIO_MODE_EVT_RISING,          //  上升沿触发事件模式
    BSP_GPIO_MODE_EVT_FALLING,         //  下降沿触发事件模式
    BSP_GPIO_MODE_EVT_RISING_FALLING,  //  上升或者下降沿都能触发事件模式
};
typedef enum _BSP_GPIO_MODE_enum BSP_GPIO_MODE_typedefenum;

//  端口上下拉状态
enum _BSP_GPIO_PULL_enum{
    BSP_GPIO_PULL_UP,      //  上拉
    BSP_GPIO_PULL_DOWN,    //  下拉
    BSP_GPIO_PULL_NO,      //  浮空
};
typedef enum _BSP_GPIO_PULL_enum BSP_GPIO_PULL_typedefenum;

enum _BSP_GPIO_SPEED_enum{
    BSP_GPIO_SPEED_HIGH,   //  高速
    BSP_GPIO_SPEED_MID,    //  中速
    BSP_GPIO_SPEED_LOW,    //  低速
};
typedef enum _BSP_GPIO_SPEED_enum BSP_GPIO_SPEED_typedefenum;

typedef void ( * BSP_GPIO_IRQ_Callback_typedef ) ( void ) ;

struct _BSP_GPIO_INFO_struct{
    uint32_t num;
    GPIO_Module * port;
    BSP_GPIO_MODE_typedefenum mode;
    BSP_GPIO_PULL_typedefenum pull;
    BSP_GPIO_SPEED_typedefenum speed;
    BSP_GPIO_IRQ_Callback_typedef irq_callback;
    uint32_t option;
};
typedef struct _BSP_GPIO_INFO_struct BSP_GPIO_INFO_typedefstruct;
static BSP_GPIO_INFO_typedefstruct const GPIO_RST ={
    .mode = BSP_GPIO_MODE_OUTPUT_OD,
    .option = 0,
    .num = GPIO_PIN_0,
    .port = GPIOA,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;

struct _BSP_FUN_GPIO_struct{
    void ( * Init ) ( BSP_GPIO_INFO_typedefstruct const * const info ) ;
    void ( * DeInit ) ( BSP_GPIO_INFO_typedefstruct const * const info ) ;
    void ( * Set ) ( BSP_GPIO_INFO_typedefstruct const * const info ) ;
    void ( * Reset ) ( BSP_GPIO_INFO_typedefstruct const * const info ) ;
    void ( * Toggle ) ( BSP_GPIO_INFO_typedefstruct const * const info ) ;
    char ( * Read ) ( BSP_GPIO_INFO_typedefstruct const * const info ) ;
} ;
typedef const struct _BSP_FUN_GPIO_struct BSP_FUN_GPIO_typestruct;
extern BSP_FUN_GPIO_typestruct BSP_FUN_GPIO;  //  GPIO 相关的函数指针结构体


#endif
