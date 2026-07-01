
#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include "stdint.h"


//#define BSP_LED_NUM_TX1   (0x01)
//#define BSP_LED_NUM_TX2   (0x02)
//#define BSP_LED_NUM_TX3   (0x04)
//#define BSP_LED_NUM_TX4   (0x08)
//#define BSP_LED_NUM_RX    (0x10)

typedef enum{
    BSP_LED_NUM_TX1,
    BSP_LED_NUM_TX2,
    BSP_LED_NUM_TX3,
    BSP_LED_NUM_TX4,
    BSP_LED_NUM_RX,

}BSP_LED_NUM_Typedefenum;

struct _BSP_LED_FUN_struct{
    void ( * Init ) ( void ) ;
     void (*refresh)(void);                      //  刷新指示灯的状态，需要定时调用，建议5ms以内
    void (*on)(BSP_LED_NUM_Typedefenum);        //  将一颗指示灯的状态，标记为点亮（不会立即生效，需要调用刷新函数后才会改变状态）
    void (*off)(BSP_LED_NUM_Typedefenum);       //  将一颗指示灯的状态，标记为熄灭（不会立即生效，需要调用刷新函数后才会改变状态）
    void (*toggle)(BSP_LED_NUM_Typedefenum);    //  将一颗指示灯的状态，标记为翻转（不会立即生效，需要调用刷新函数后才会改变状态）
    void (*ledrxon)( void ) ;
    void (*ledrxoff)( void ) ;
//    void ( * Read ) ( uint8_t * state ) ;
//    void ( * Write ) ( uint8_t state ) ;
//    void ( * On ) ( uint8_t state ) ;
//    void ( * Off ) ( uint8_t state ) ;
//    void ( * Toggle ) ( uint8_t state ) ;
} ;
typedef const struct _BSP_LED_FUN_struct BSP_LED_FUN_typestruct;
extern BSP_LED_FUN_typestruct BSP_LED_FUN;  // LED 相关的函数指针结构体

#endif
