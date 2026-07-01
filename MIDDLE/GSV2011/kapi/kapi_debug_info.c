#include "kernel_fsm.h"
#include "kapi.h"
#include "uapi.h"

void PrintHdcpFsm(GSV_CONFIG *gsv_config,AvPort *Port, uint8 OldState)
{
    if(*Port->content.is_HdcpFsm != OldState)
    {
        switch(*Port->content.is_HdcpFsm)
        {
           case AvFsmHdcpDefault:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: HdcpDefault",Port->index);
            break;

           case AvFsmHdcpDisable:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: HdcpDisable",Port->index);
            break;

           case AvFsmHdcpFail:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: HdcpFail",Port->index);
            break;

           case AvFsmHdcp2p2State:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: Hdcp2p2State",Port->index);
            break;

           case AvFsmHdcpReAuthentication:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: HdcpReAuthentication",Port->index);
            break;

           case AvFsmHdcpReadBksv:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: HdcpReadBksv",Port->index);
            break;

           case AvFsmHdcpReceiverMode:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: HdcpReceiverMode",Port->index);
            break;

           case AvFsmHdcpRepeaterMode:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: HdcpRepeaterMode",Port->index);
            break;

           case AvFsmHdcpRepeaterReset:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: HdcpRepeaterReset",Port->index);
            break;

           case AvFsmHdcpRequestSinkHdcp:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: HdcpRequestSinkHdcp",Port->index);
            break;

           case AvFsmHdcpStartAuthentication:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: HdcpStartAuthentication",Port->index);
            break;

           case AvFsmHdcpUpstreamConnected:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: HdcpUpstreamConnected",Port->index);
            break;
        }
    }
}

void PrintPlugTxFsm(GSV_CONFIG *gsv_config,AvPort *Port, uint8 OldState)
{
    if(*Port->content.is_PlugTxFsm != OldState)
    {
        switch (*Port->content.is_PlugTxFsm)
        {
          case AvFsmPlugTxEdidError:
              AvUapiOutputDebugFsm(gsv_config,"Port%d: PlugTxEdidError", Port->index);
            break;
          case AvFsmPlugTxDefault:
              AvUapiOutputDebugFsm(gsv_config,"Port%d: PlugTxDefault", Port->index);
            break;
          case AvFsmPlugTxDefaultEdid:
              AvUapiOutputDebugFsm(gsv_config,"Port%d: PlugTxDefaultEdid", Port->index);
            break;
          case AvFsmPlugTxEdid:
              AvUapiOutputDebugFsm(gsv_config,"Port%d: PlugTxEdid", Port->index);
            break;
          case AvFsmPlugTxEdidManage:
              AvUapiOutputDebugFsm(gsv_config,"Port%d: PlugTxEdidManage", Port->index);
            break;
          case AvFsmPlugTxEnableTxCore:
              AvUapiOutputDebugFsm(gsv_config,"Port%d: PlugTxEnableTxCore", Port->index);
            break;
          case AvFsmPlugTxHdcp:
              AvUapiOutputDebugFsm(gsv_config,"Port%d: PlugTxHdcp", Port->index);
            break;
          case AvFsmPlugTxHpdAntiDither:
              AvUapiOutputDebugFsm(gsv_config,"Port%d: PlugTxHpdAntiDither", Port->index);
            break;
          case AvFsmPlugTxReset:
              AvUapiOutputDebugFsm(gsv_config,"Port%d: PlugTxReset", Port->index);
          gsv_config->read.connect=0;
            break;
          case AvFsmPlugTxStable:
              AvUapiOutputDebugFsm(gsv_config,"Port%d: PlugTxStable", Port->index);
            gsv_config->read.connect=1;
            break;
          case AvFsmPlugTxTransmitVideo:
              AvUapiOutputDebugFsm(gsv_config,"Port%d: PlugTxTransmitVideo", Port->index);
            break;
          case AvFsmPlugTxVideoUnlocked:
              AvUapiOutputDebugFsm(gsv_config,"Port%d: PlugTxVideoUnlocked", Port->index);
            break;
        }
    }
}

void PrintPlugRxFsm(GSV_CONFIG *gsv_config,AvPort *Port, uint8 OldState)
{
    if(*Port->content.is_PlugRxFsm != OldState)
    {
        switch (*Port->content.is_PlugRxFsm)
        {
          case AvFsmPlugRxDefault:
              AvUapiOutputDebugFsm(gsv_config,"Port%d: PlugRxDefault", Port->index);
            break;
          case AvFsmPlugRxDetect:
              AvUapiOutputDebugFsm(gsv_config,"Port%d: PlugRxDetect", Port->index);
            break;
          case AvFsmPlugRxInfoUpdate:
              AvUapiOutputDebugFsm(gsv_config,"Port%d: PlugRxInfoUpdate", Port->index);
            break;
          case AvFsmPlugRxInputLock:
              AvHandleEvent(gsv_config,Port, AvEventPortUpStreamConnected, NULL, NULL);
              AvUapiOutputDebugFsm(gsv_config,"Port%d: PlugRxInputLock", Port->index);
              
            break;
          case AvFsmPlugRxPlugged:
              AvUapiOutputDebugFsm(gsv_config,"Port%d: PlugRxPlugged", Port->index);
            break;
          case AvFsmPlugRxPullDownHpd:
              if(Port->content.rx->Input5V == 0)
                  AvHandleEvent(gsv_config,Port, AvEventPortUpStreamDisconnected, NULL, NULL);
              AvUapiOutputDebugFsm(gsv_config,"Port%d: PlugRxPullDownHpd", Port->index);
              gsv_config->read.connect=0;
              
             
//              Port->content.video->timing.HActive=0;
//              Port->content.video->timing.VActive=0;
                  
            break;
          case AvFsmPlugRxReadTiming:
              AvUapiOutputDebugFsm(gsv_config,"Port%d: PlugRxReadTiming", Port->index);
              gsv_config->read.connect=1;
            break;
          case AvFsmPlugRxReset:
              AvUapiOutputDebugFsm(gsv_config,"Port%d: PlugRxReset", Port->index);
                
            break;
          case AvFsmPlugRxStable:
              AvUapiOutputDebugFsm(gsv_config,"Port%d: PlugRxStable", Port->index);
            
            break;
        }
    }
}

void PrintReceiverFsm(GSV_CONFIG *gsv_config,AvPort *Port, uint8 OldState)
{
    if(*Port->content.is_ReceiverFsm != OldState)
    {
        switch (*Port->content.is_ReceiverFsm)
        {
          case AvFsmRxDefault:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: RxDefault", Port->index);
            break;
          case AvFsmRxDetect:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: RxDetect", Port->index);
            break;
          case AvFsmRxFreerun:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: RxFreerun", Port->index);
            break;
          case AvFsmRxReceiving:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: RxReceiving", Port->index);
            break;
          case AvFsmRxReset:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: RxReset", Port->index);
            break;
        }
    }
}

#if AvEnableCecFeature /* CEC Related */
void PrintCecFsm(AvPort *Port, uint8 OldState)
{
    if(*Port->content.is_CecFsm != OldState)
    {
        switch (*Port->content.is_CecFsm)
        {
          case AvFsmCecDefault:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: CecDefault", Port->index);
            break;
          case AvFsmCecIdle:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: CecIdle", Port->index);
            break;
          case AvFsmCecNotConnected:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: CecNotConnected", Port->index);
            break;
          case AvFsmCecReset:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: CecReset", Port->index);
            break;
          case AvFsmCecTxLogAddr:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: CecTxLogAddr", Port->index);
            break;
          case AvFsmCecAudioControl:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: CecAudioControl", Port->index);
            break;
          case AvFsmCecAudioFormat:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: CecAudioFormat", Port->index);
            break;
          case AvFsmCecAudioManage:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: CecAudioManage", Port->index);
            break;
          case AvFsmCecCmdARC:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: CecCmdARC", Port->index);
            break;
          case AvFsmCecCmdAudioARC:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: CecCmdAudioARC", Port->index);
            break;
          case AvFsmCecCmdSystemAudioModetoAll:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: CecCmdSystemAudioModetoAll", Port->index);
            break;
          case AvFsmCecFunctionalDefault:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: CecFunctionalDefault", Port->index);
            break;
          case AvFsmCecCmdActiveSource:
            AvUapiOutputDebugFsm(gsv_config,"Port%d: CecCmdActiveSource", Port->index);
            break;
        }
    }
}
#endif /* CEC Related */
