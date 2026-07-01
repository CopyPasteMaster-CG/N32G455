#include "av_key_cmd.h"
#include "av_config.h"
#include "hal.h"
#include "kapi.h"
//#include "main.h"

//extern uint8  EdidHdmi2p0;
//extern uint8  LogicOutputSel;

#if AvEnableKeyInput /* Enable UART */

extern uchar AudioStatus;

void ProcessKey(GSV_CONFIG *gsv_config,AvPort *port);
#define FoundKeyPress(p) uChar&p

#endif /* Enable UART */

void ProcessKey(GSV_CONFIG *gsv_config,AvPort *port)
{
#if AvEnableKeyInput
    uint8 value = 0x00;
    uint8 uChar = 0x00;
    if(AvHalGetKey(gsv_config,&uChar) == AvNotAvailable)
        return;

    /* Logic Video/Audio Direction */
    value = (uChar>>0) & 0x01;
    if(value != 0x00)
    {
        gsv_config->LogicOutputSel  = 1 - gsv_config->LogicOutputSel ;
    }
    /* Logic LED */
    LogicLedOut(gsv_config->LogicOutputSel );
#endif
}

void ListenToKeyCommand(GSV_CONFIG *gsv_config,AvPort *port)
{
#if AvEnableKeyInput
    ProcessKey(gsv_config,port);
#endif
}

void RxInLedOut(uint8 enable)
{
//    if(enable == 1)
//        HAL_GPIO_WritePin(GPIOA, LED0_Pin, GPIO_PIN_RESET);
//    else
//        HAL_GPIO_WritePin(GPIOA, LED0_Pin, GPIO_PIN_SET);
}

void TxOutLedOut(uint8 index, uint8 enable)
{
//    if(enable == 1)
//        HAL_GPIO_WritePin(GPIOA, LED1_Pin, GPIO_PIN_RESET);
//    else
//        HAL_GPIO_WritePin(GPIOA, LED1_Pin, GPIO_PIN_SET);
}

void LogicLedOut(uint8 enable)
{
//    if(enable == 1)
//        HAL_GPIO_WritePin(GPIOA, LED2_Pin, GPIO_PIN_RESET);
//    else
//        HAL_GPIO_WritePin(GPIOA, LED2_Pin, GPIO_PIN_SET);
}
