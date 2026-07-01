#ifndef __av_key_cmd_h
#define __av_key_cmd_h

#include "av_config.h"
#include "gsv2011_config.h"
#ifdef GSV_BASE_PROJECT
#else
//#include "stm32f1xx_hal.h"
#endif

void ListenToKeyCommand(GSV_CONFIG *gsv_config,AvPort *port);
void RxInLedOut(uint8 enable);
void TxOutLedOut(uint8 index, uint8 enable);
void Edid2p0LedOut(uint8 enable);
void LogicLedOut(uint8 enable);

#endif
