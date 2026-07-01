#ifndef __av_uart_cmd_h
#define __av_uart_cmd_h

#include "av_config.h"

#include "gsv2011_config.h"

#if AvEnableIntegrityCheck
#include "eval_integrity_report.h"
#endif

void ListenToUartCommand(GSV_CONFIG *gsv_config,AvPort *port);

#endif
