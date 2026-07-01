/**
 * @file kernel_fsm_update.h
 *
 * @brief functions for update FSMs in the kernel layer
 */

#include "kernel_status_update.h"
#include "kapi.h"
#include "uapi_function_mapper.h"
#include "uapi.h"


/* PART 1 */
/* Receiver FSM */

/* check RX FSM status */
void KfunCheckRxState(pin AvPort *port)
{
    /* if no new information, return state */
    switch (*port->content.is_ReceiverFsm)
    {
        case AvFsmRxDefault:
            break;
        case AvFsmRxDetect:
            {
                KfunPollingRxStatus(port);
                break;
            }
        case AvFsmRxFreerun:
            {
                KfunPollingRxStatus(port);
                break;
            }
        case AvFsmRxReceiving:
            {
                KfunPollingRxStatus(port);
                break;
            }
        case AvFsmRxReset:
            {
                KfunPollingRxStatus(port);
                break;
            }
        default:
            break;
    }

    return;
}

/* PART 2 */
/* HDCP FSM */
#if AvEnableSimplifyHdcp
void KfunSimpleHdcpManage(GSV_CONFIG *gsv_config,AvPort *port)
{
    /* 1. Firstly Find whether source is encrypted or not */
    KfunHdcpDetectSource(gsv_config,port);
    /* 2. No matter source is encrypted or not, sink should also get encrypt info */
    KfunHdcpDetectSink(gsv_config,port);
    /* 3. Sync them */
    KfunSimpleHdcpSync(gsv_config,port);
    return;
}

#else
void KfunCheckRxHdcpState( AvPort *port)
{
    /* check BKSV Ready */
    /* Int.BksvReady */
    /* HDCP Authenticated */
    /* Int.HdcpAuth */
    /* HDCP Error */
    /* Int.HdcpError */

    switch (*port->content.is_HdcpFsm)
    {
        case AvFsmHdcpDefault:
            break;
        case AvFsmHdcpDisable:
        {
            break;
        }
        case AvFsmHdcpFail:
        {
            /* if pr->hpd, go to HDCP_DISABLE*/
            break;
        }
        case AvFsmHdcpUpstreamConnected:
        {
            KfunHdcpDetectSource(port);
            break;
        }
        case AvFsmHdcpStartAuthentication:
        {
            KfunHdcpDetectSource(port);
            break;
        }
        case AvFsmHdcpReadBksv:
        {
            /* allocate a ptr for sink list*/
            /* then use a chain */
            /* check BKSV update */
            KfunHdcpDetectSink(port);
            break;
        }
        case AvFsmHdcpRequestSinkHdcp:
        {
            KfunHdcpDetectSource(port);
            KfunHdcpDetectSink(port);
            break;
        }
        case AvFsmHdcpReceiverMode:
        {
            KfunHdcpDetectSource(port);
            KfunHdcpDetectSink(port);
            break;
        }
        case AvFsmHdcpRepeaterMode:
        {
            KfunHdcpDetectSource(port);
            KfunHdcpDetectSink(port);
            break;
        }
        case AvFsmHdcpReAuthentication:
        {
            KfunHdcpDetectSource(port);
            break;
        }
        case AvFsmHdcpRepeaterReset:
        {
            KfunHdcpDetectSink(port);
            break;
        }
        default:
            break;
    }

    return;
}
#endif

/* PART 3 */
/* Rx Routing FSM */

/* PART 4 */
/* Tx Routing FSM */

/* PART 5 */
/* Plug RX FSM */

void KfunCheckPrState(GSV_CONFIG *gsv_config,AvPort *port)
{
    /* State detection based on FSM state */
    /* TMDS clock detection, not interrupt */
    /* HdmiInts.TmdsClkChg */
    /* cable detection, not interrupt */
    /* HdmiInts.CableDetect */
    /* Internal Mute detection*/
    /* HdmiInts.InternMute */
    /* Video PLL Lock */
    /* HdmiInts.VidPllChg */
    /* Check DE regen lock */
    /* HdmiInts.DeRegenLck */
    /* Vsync lock */
    /* HdmiInts.VsyncLck */
    /* Encryption state */
    /* HdmiInts.Encrypted */

    /* TMDS Frequency Change int*/
    /* HdmiInts.NewTmdsFreq */

    /* check routing, if not sink is connected, reset this port */
    if(port->core.HdmiCore != -1)
        port->content.rx->Reset = 0;
    else
        port->content.rx->Reset = 1;

    switch (*port->content.is_PlugRxFsm)
    {
        case AvFsmPlugRxDefault:
            break;
        case AvFsmPlugRxDetect:
        {
            AvUapiRxGetStatus(gsv_config,port);
            break;
        }
        case AvFsmPlugRxInfoUpdate:
            break;
        case AvFsmPlugRxInputLock:
        {
            AvUapiRxGetStatus(gsv_config,port);
            break;
        }
        case AvFsmPlugRxPlugged:
        {
            AvUapiRxGetStatus(gsv_config,port);
            break;
        }
        case AvFsmPlugRxPullDownHpd:
        {
            AvUapiRxGetStatus(gsv_config,port);
            break;
        }
        case AvFsmPlugRxReadTiming:
            break;
        case AvFsmPlugRxReset:
        {
            AvUapiRxGetStatus(gsv_config,port);
         
            break;
        }
        case AvFsmPlugRxStable:
        {
            AvUapiRxGetStatus(gsv_config,port);
         
            break;
        }
        default:
            break;
    }
}

/* PART 6 */
/* Plug TX FSM */

void KfunCheckPtState(GSV_CONFIG *gsv_config, AvPort *port)
{
    /* HPD Interrupt */
    /* Int.Hpd */
    /* MSEN state */
    /* Int.MonSen */
    /* EDID Ready Interrupt */
    /* Int.EdidReady*/
    /* Vsync Interrupt */
    /* Int.Vsync */

    switch (*port->content.is_PlugTxFsm)
    {
        case AvFsmPlugTxEdidError:
        {
            AvUapiTxGetStatus(gsv_config,port);
            break;
        }
        case AvFsmPlugTxDefault:
            break;
        case AvFsmPlugTxDefaultEdid:
            break;
        case AvFsmPlugTxEdid:
        {
            AvUapiTxGetStatus(gsv_config,port);
            /* AvUapiTxEdidStat(port); */
            break;
        }
        case AvFsmPlugTxEdidManage:
        {
            AvUapiTxGetStatus(gsv_config,port);
            /* AvUapiTxEdidStat(port); */
            break;
        }
        case AvFsmPlugTxHdcp:
        {
            AvUapiTxGetStatus(gsv_config,port);
            AvUapiTxGetHdcpStatus(gsv_config,port);
            break;
        }
        case AvFsmPlugTxHpdAntiDither:
        {
            AvUapiTxGetStatus(gsv_config,port);
            break;
        }
        case AvFsmPlugTxReset:
        {
            AvUapiTxGetStatus(gsv_config,port);
            break;
        }
        case AvFsmPlugTxStable:
        {
            AvUapiTxGetStatus(gsv_config,port);
            AvUapiTxGetHdcpStatus(gsv_config,port);
            break;
        }
        case AvFsmPlugTxTransmitVideo:
        {
            if(port->content.tx->InfoReady == 0)
                AvHandleEvent(gsv_config,port, AvEventPortDownStreamSending, NULL, NULL);
            AvUapiTxGetStatus(gsv_config,port);
            AvUapiTxGetHdcpStatus(gsv_config,port);
            break;
        }
        case AvFsmPlugTxEnableTxCore:
        {
            AvHandleEvent(gsv_config,port, AvEventPortDownStreamConnected, NULL, NULL);
            AvUapiTxGetStatus(gsv_config,port);
            break;
        }
        case AvFsmPlugTxVideoUnlocked:
        {
            AvUapiTxGetStatus(gsv_config,port);
            break;
        }
        default:
            break;
    }
    return;
}

/* PART 7 */
/* CEC FSM state */
void KfunCheckCecState(AvPort *port)
{
#if AvEnableCecFeature /* CEC Related */
    switch (*port->content.is_CecFsm)
    {
        case AvFsmCecDefault:
            break;
        case AvFsmCecNotConnected:
            break;
        case AvFsmCecReset:
            break;
        default:
            AvUapiCecRxGetStatus(port);
            AvUapiCecTxGetStatus(port);
            if(port->content.cec->RxGetFlag)
                KfunCecRxMsgProcess(port);
            break;
    }
#endif
    return;
}

/* Part 8 Video Input and Output */
void KfunCheckLogicVideoTx(GSV_CONFIG *gsv_config,AvPort *port)
{
    if(port->content.RouteVideoFromPort != NULL)
        AvUapiCheckLogicVideoTx(gsv_config,port);
}

void KfunCheckLogicVideoRx(GSV_CONFIG *gsv_config,AvPort *port)
{
    if(port->content.RouteVideoToPort != NULL)
        AvUapiCheckLogicVideoRx(gsv_config,port);
}

/* Part 9 Audio Input and Output */
void KfunCheckLogicAudioTx(GSV_CONFIG *gsv_config,AvPort *port)
{
    AvUapiCheckLogicAudioTx(gsv_config,port);
}

void KfunCheckLogicAudioRx(GSV_CONFIG *gsv_config,AvPort *port)
{
#if AvEnableAudioTTLInput
    AvUapiCheckLogicAudioRx(gsv_config,port);
#endif
}

/* Part 10 Scaler */
void KfunCheckVideoScaler(GSV_CONFIG *gsv_config,AvPort *port)
{
    AvPort *SourcePort = (AvPort*)port->content.RouteVideoFromPort;
    if(SourcePort == NULL)
        return;
    else if(port->content.RouteVideoToPort == NULL)
    {
        port->content.scaler->ScalerInVic = 0;
        port->content.scaler->ColorSpace  = AV_Y2Y1Y0_INVALID;
    }
    else if((SourcePort->type == HdmiRx) || (SourcePort->type == VideoGen))
    {
        /* Update Vic */
        if(port->content.scaler->ScalerInVic != SourcePort->content.video->timing.Vic)
        {
            port->content.scaler->Update = 0x03;
            port->content.scaler->ScalerInVic = SourcePort->content.video->timing.Vic;
        }
        if(port->content.scaler->ColorSpace != SourcePort->content.video->Y)
        {
            port->content.scaler->Update = 0x03;
            port->content.scaler->ColorSpace = SourcePort->content.video->Y;
        }
    }
    /* Call Uapi Function */
    AvUapiCheckVideoScaler(gsv_config,port);
}

/* Part 11 Color */
void KfunCheckVideoColor(GSV_CONFIG *gsv_config,AvPort *port)
{
    AvPort *SourcePort = (AvPort*)port->content.RouteVideoFromPort;
    AvPort *CurrentPort = NULL;
    AvPort *PrevPort = NULL;
    AvPort *LogicVideoTxPort = NULL;
    uint32 EdidSupportFeature = 0xffffffff;
    uint8  LogicVideoTxFlag = 0;
    AvVideoY NewColorOutSpace;
    AvVideoCs NewColorOutCs;
    if(SourcePort == NULL)
        return;
    else if(port->content.RouteVideoToPort == NULL)
    {
        port->content.color->ColorInVic    = 0;
        port->content.color->ColorInSpace  = AV_Y2Y1Y0_INVALID;
        port->content.color->ColorOutSpace = AV_Y2Y1Y0_INVALID;
        port->content.color->ColorInCs     = AV_CS_AUTO;
        port->content.color->ColorOutCs    = AV_CS_AUTO;
    }
    else if((SourcePort->type == HdmiRx) || (SourcePort->type == VideoGen) ||
            (SourcePort->type == LogicVideoRx))
    {
        /* Check Downstream Full Range Support */
        while(KfunFindVideoNextTxEnd(port, &PrevPort, &CurrentPort) == AvOk)
        {
            if((CurrentPort->type == HdmiTx) &&
               (CurrentPort->content.tx->EdidReadSuccess == AV_EDID_UPDATED))
                EdidSupportFeature = EdidSupportFeature & CurrentPort->content.tx->EdidSupportFeature;
            else if(CurrentPort->type == LogicVideoTx)
            {
                LogicVideoTxFlag = 1;
                LogicVideoTxPort = CurrentPort;
            }
            PrevPort = CurrentPort;
            CurrentPort = NULL;
        }
        /* Input Vic */
        if(port->content.color->ColorInVic != SourcePort->content.video->timing.Vic)
        {
            port->content.color->Update = 0x03;
            port->content.color->ColorInVic = SourcePort->content.video->timing.Vic;
        }
        /* Input ColorSpace */
        if(port->content.color->ColorInSpace != SourcePort->content.video->Y)
        {
            port->content.color->Update = 0x03;
#if AvEnableInternalVideoGen
            if(SourcePort->type == VideoGen)
                port->content.color->ColorInSpace = AV_Y2Y1Y0_RGB;
            else
#endif
            port->content.color->ColorInSpace = SourcePort->content.video->Y;
        }
        if(port->content.color->ColorInCs != SourcePort->content.video->InCs)
        {
            port->content.color->Update = 0x03;
            port->content.color->ColorInCs = SourcePort->content.video->InCs;
        }
        /* Output ColorSpace */
        if(LogicVideoTxFlag == 1)
        {
            /* Parallel bus YCbCr 422 output for YCbCr 444 fixed output*/
            if(((ParallelConfigTable[LogicVideoTxPort->content.lvtx->Config*3 + 1] & 0x04) != 0) ||
               ((ParallelConfigTable[LogicVideoTxPort->content.lvtx->Config*3 + 2] & 0x40) != 0) ||
               (LogicVideoTxPort->content.video->Y == AV_Y2Y1Y0_YCBCR_422))
                NewColorOutSpace = AV_Y2Y1Y0_YCBCR_444;
            else if(LogicVideoTxPort->content.video->Y == AV_Y2Y1Y0_INVALID)
                NewColorOutSpace = SourcePort->content.video->Y;
            else if((LogicVideoTxPort->content.video->Y == AV_Y2Y1Y0_RGB) &&
                    (SourcePort->content.video->Y == AV_Y2Y1Y0_YCBCR_420))
                NewColorOutSpace = AV_Y2Y1Y0_YCBCR_444;
            else
                NewColorOutSpace = LogicVideoTxPort->content.video->Y;
        }
        else if(port->content.color->ColorInSpace != AV_Y2Y1Y0_YCBCR_420)
            NewColorOutSpace = AV_Y2Y1Y0_YCBCR_420;
        else
            NewColorOutSpace = AV_Y2Y1Y0_YCBCR_444;
        if(NewColorOutSpace != port->content.color->ColorOutSpace)
        {
            port->content.color->Update = 0x03;
            port->content.color->ColorOutSpace = NewColorOutSpace;
        }

        if(LogicVideoTxFlag == 1)
        {
            if(((SourcePort->content.video->InCs == AV_CS_LIM_BT2020_YCC) ||
                (SourcePort->content.video->InCs == AV_CS_LIM_BT2020_RGB)) &&
               (NewColorOutSpace == AV_Y2Y1Y0_RGB))
            {
                port->content.color->ColorInCs  = AV_CS_LIM_YUV_601;
            }
            /* No CP CSC for 420 input, bypass CSC */
            if(SourcePort->content.video->Y == AV_Y2Y1Y0_YCBCR_420)
                NewColorOutCs = SourcePort->content.video->InCs;
            else if(LogicVideoTxPort->content.video->InCs == AV_CS_AUTO)
            {
                if((NewColorOutSpace != AV_Y2Y1Y0_RGB) && (SourcePort->content.video->Y == AV_Y2Y1Y0_RGB))
                    NewColorOutCs = AV_CS_LIM_YUV_709;
                else if((NewColorOutSpace == AV_Y2Y1Y0_RGB) && (SourcePort->content.video->Y != AV_Y2Y1Y0_RGB))
                    NewColorOutCs = AV_CS_LIM_RGB;
                else
                    NewColorOutCs = SourcePort->content.video->InCs;
            }
            else
                NewColorOutCs = LogicVideoTxPort->content.video->InCs;
        }
        else if(((SourcePort->content.video->AvailableVideoPackets & AV_BIT_HDR_PACKET) == 0) ||
                (SourcePort->content.video->Y == AV_Y2Y1Y0_RGB))
        {
            if(EdidSupportFeature & AV_BIT_FEAT_YUV_FULL_RANGE)
            {
                if(EdidSupportFeature & AV_BIT_FEAT_YUV_709)
                    NewColorOutCs = AV_CS_YUV_709;
                else
                    NewColorOutCs = AV_CS_YUV_601;
            }
            else
            {
                if(EdidSupportFeature & AV_BIT_FEAT_YUV_709)
                    NewColorOutCs = AV_CS_LIM_YUV_709;
                else
                    NewColorOutCs = AV_CS_LIM_YUV_601;
            }
        }
        else
            /* no CSC for HDR */
            NewColorOutCs = port->content.color->ColorInCs;
        /* Update CS when new State */
        if(port->content.color->ColorOutCs != NewColorOutCs)
        {
            port->content.color->Update = 0x03;
            port->content.color->ColorOutCs = NewColorOutCs;
        }
    }
    /* Call Uapi Function */
    AvUapiCheckVideoColor(gsv_config,port);
}

/* Part 12 VideoGen */
void KfunCheckVideoGen(GSV_CONFIG *gsv_config,AvPort *port)
{
    /* Call Uapi Function */
    if(port->content.RouteVideoToPort != NULL)
        AvUapiCheckVideoGen(gsv_config,port);
}

/* Part 13 AudioGen */
void KfunCheckAudioGen(AvPort *port)
{
    /* Call Uapi Function */
    if(port->content.RouteVideoToPort != NULL)
        AvUapiCheckAudioGen(port);
}

/* Part 14 ClockGen */
void KfunCheckClockGen(GSV_CONFIG *gsv_config,AvPort *port)
{
    /* Call Uapi Function */
    AvUapiCheckClockGen(gsv_config,port);
}
