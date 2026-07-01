
#include "BSP_KEY.h"

#include "BSP.h"

static BSP_GPIO_INFO_typedefstruct const GPIO_KEY_1 =
{
    .mode = BSP_GPIO_MODE_INPUT,
    .option = 0,
    .num = GPIO_PIN_8,
    .port = GPIOA,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;

static BSP_GPIO_INFO_typedefstruct const GPIO_KEY_2 =
{
    .mode = BSP_GPIO_MODE_INPUT,
    .option = 0,
    .num = GPIO_PIN_12,
    .port = GPIOC,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;

static BSP_GPIO_INFO_typedefstruct const GPIO_KEY_3 =
{
    .mode = BSP_GPIO_MODE_INPUT,
    .option = 0,
    .num = GPIO_PIN_2,
    .port = GPIOD,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;

static BSP_GPIO_INFO_typedefstruct const GPIO_KEY_4 =
{
    .mode = BSP_GPIO_MODE_INPUT,
    .option = 0,
    .num = GPIO_PIN_15,
    .port = GPIOA,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;

static void FUN_KEY_Init(void)
{    
    BSP_FUN_GPIO.Init(&GPIO_KEY_1) ;
    BSP_FUN_GPIO.Init(&GPIO_KEY_2) ;
    BSP_FUN_GPIO.Init(&GPIO_KEY_3) ;
    BSP_FUN_GPIO.Init(&GPIO_KEY_4) ;
}

static void FUN_KEY_Read(uint8_t * state)
{
    *state=0;
    *state |= (BSP_FUN_GPIO.Read(&GPIO_KEY_1)?0x00:BSP_KEY_NUM_1);
    *state |= (BSP_FUN_GPIO.Read(&GPIO_KEY_2)?0x00:BSP_KEY_NUM_2);
    *state |= (BSP_FUN_GPIO.Read(&GPIO_KEY_3)?0x00:BSP_KEY_NUM_3);
    *state |= (BSP_FUN_GPIO.Read(&GPIO_KEY_4)?0x00:BSP_KEY_NUM_4);
}


BSP_KEY_FUN_typestruct BSP_KEY_FUN =
{
    .Init = FUN_KEY_Init,
    .Read = FUN_KEY_Read,
} ;
