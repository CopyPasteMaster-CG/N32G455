/**
 * @file kapi.c
 *
 * @brief kernel api functions of this audio/video software package \n
 *        this file will also be only header file of this software package when user do porting
 */
#include "kapi.h"
#include "uapi.h"
#include "kernel_status_update.h"

//static AvPort *FirstPort = NULL;
//static AvPort *PreviousPort = NULL;
//AvFpKeyCommand   AvHookKeyCmd;
//AvFpUartCommand  AvHookUartCmd;
//AvFpIrdaCommand  AvHookIrdaCmd;
void ClearVideoFromPort(pin AvPort *port);
void ClearAudioFromPort(pin AvPort *port);

/**
 * @brief  hookup user layer access functions
 * @return AvOk - success
 */
kapi AvRet AvKapiHookUserFunctions(GSV_CONFIG *gsv_config,pin AvFpKeyCommand keyCmd, pin AvFpUartCommand uartCmd,pin AvFpIrdaCommand IrdaCmd)
{
    AvRet ret = AvOk;
    gsv_config->AvHookKeyCmd = keyCmd;
    gsv_config->AvHookUartCmd = uartCmd;
    gsv_config->AvHookIrdaCmd = IrdaCmd;
    return ret;
}

/**
 * @brief  init software
 * @return none
 * @note
 */
kapi AvRet AvApiInit( GSV_CONFIG *gsv_config)
{
    AvRet ret = AvOk;
    ret = AvUapiInit(gsv_config);
    return ret;
}

/**
 * @brief  add device
 * @return none
 * @note customer is allowed to add customized device to the loop
 * during initialization, the device's detail should be fed
 */
kapi AvRet AvApiAddDevice(AvDevice *device, AvDeviceType type, uint8 index,
                           void *specific, void *port,  void *extension)
{
    device->type      = type;
    device->index     = index;
    device->specific  = specific;
    device->port      = port;
    device->extension = extension;

    return AvOk;

}

/**
 * @brief  add audio/video port
 * @return none
 * @note customer is allowed to add customized port to the loop
 * during initialzation, the port's AvPort and AvPortType
 * must be set, a dedicated ID will be given to this port
 */
kapi AvRet AvApiAddPort(GSV_CONFIG *gsv_config, AvDevice *device, pin AvPort *port, uint16 index, AvPortType type)
{
    if(!(gsv_config->FirstPort))
        gsv_config->FirstPort = port;

    port->device = device;
    port->index  = index;
    port->type   = type;
    AvApiPortMemAllocate(gsv_config,port);

    if(gsv_config->PreviousPort)
        gsv_config->PreviousPort->next = (struct AvPort*)port;
		
    gsv_config->PreviousPort = port;
    port->next = NULL;

    return AvOk;

}

/**
 * @brief  init device
 * @return none
 * @note device initialization must be done after AddDevice and AddPort
 * Or else, the port structure will crash.
 */
kapi AvRet AvApiInitDevice(GSV_CONFIG *gsv_config,AvDevice *device)
{
    AvUapiInitDevice(gsv_config, device);
    return AvOk;
}

/**
 * @brief  init fsm
 * @return none
 * @note device initialization must be done after initdevice
 * Or else, the port structure will crash.
 */
kapi AvRet AvApiInitFsm(pin AvPort *port)
{
    switch(port->type)
    {
        case HdmiRx:
        {
            AvKapiFsmEnterFunPlugRxFsm(port);
            AvKapiFsmEnterFunHdcpFsm(port);
            AvKapiFsmEnterFunReceiverFsm(port);
            break;
        }
        case DviRx:
        {
            AvKapiFsmEnterFunPlugRxFsm(port);
            AvKapiFsmEnterFunReceiverFsm(port);
            break;
        }
        case AnalogRx:
        {
            AvKapiFsmEnterFunReceiverFsm(port);
            break;
        }
        case LogicVideoRx:
        {
            AvKapiFsmEnterFunReceiverFsm(port);
            break;
        }
        case LogicAudioRx:
        {
            break;
        }
        case HdmiTx:
        {
            AvKapiFsmEnterFunPlugTxFsm(port);
#if AvEnableCecFeature /* CEC Related */
            AvKapiFsmEnterFunCecFsm(port);
#endif
            break;
        }
        case DviTx:
        {
            AvKapiFsmEnterFunPlugTxFsm(port);
            break;
        }
        default:
            break;
    }
    return AvOk;
}

/**
 * @brief  update all ports status
 * @return none
 * @note
 */
kapi AvRet AvApiUpdate(GSV_CONFIG *gsv_config)
{
    AvPort* TempPort = gsv_config->FirstPort;
    uint8 OldState;

    while(TempPort)
    {
        /* Make Key Response and Uart Response faster */
        gsv_config->AvHookUartCmd(gsv_config->FirstPort);
        gsv_config->AvHookKeyCmd(gsv_config->FirstPort);
        gsv_config->AvHookIrdaCmd(gsv_config->FirstPort);
		
        switch(TempPort->type)
        {
            case HdmiRx:
            case DviRx:
                OldState = *TempPort->content.is_PlugRxFsm;
                KfunCheckPrState(gsv_config,TempPort);
					
                AvKapiFsmFunPlugRxFsm(gsv_config,TempPort);
					
                ReportPlugRxFsm(gsv_config,TempPort, OldState);
                OldState = *TempPort->content.is_ReceiverFsm;
                KfunCheckRxState(TempPort);
                AvKapiFsmFunReceiverFsm(gsv_config,TempPort);
                ReportReceiverFsm(gsv_config,TempPort, OldState);
                OldState = *TempPort->content.is_HdcpFsm;
                KfunCheckHdcpState(gsv_config,TempPort);
                AvKapiFsmFunHdcpFsm(TempPort);
                ReportHdcpFsm(gsv_config,TempPort, OldState);
			
                break;
            case AnalogRx:
            case LogicVideoRx:
                KfunCheckLogicVideoRx(gsv_config,TempPort);
                KfunSimpleHdcpSync(gsv_config,TempPort);
                break;
            case LogicAudioRx:
#if AvEnableAudioTTLInput
                KfunCheckLogicAudioRx(gsv_config,TempPort);
#endif
                break;
            case HdmiTx:
                OldState = *TempPort->content.is_PlugTxFsm;
                KfunCheckPtState(gsv_config,TempPort);
                AvKapiFsmFunPlugTxFsm(gsv_config,TempPort);
                ReportPlugTxFsm(gsv_config,TempPort, OldState);
#if AvEnableCecFeature /* CEC Related */
                OldState = *TempPort->content.is_CecFsm;
                KfunCheckCecState(TempPort);
                AvKapiFsmFunCecFsm(gsv_config,TempPort);
                ReportCecFsm(TempPort, OldState);
#endif
                break;
            case DviTx:
                KfunCheckPtState(gsv_config,TempPort);
                AvKapiFsmFunPlugTxFsm(gsv_config,TempPort);
                break;
            case AnalogTx:
            case LogicVideoTx:
                KfunCheckLogicVideoTx(gsv_config,TempPort);
                break;
            case LogicAudioTx:
                KfunCheckLogicAudioTx(gsv_config,TempPort);
                break;
            case VideoScaler:
                KfunCheckVideoScaler(gsv_config,TempPort);
                break;
            case VideoColor:
                KfunCheckVideoColor(gsv_config,TempPort);
                break;
#if AvEnableInternalVideoGen
            case VideoGen:
                KfunCheckVideoGen(gsv_config,TempPort);
                break;
#endif
#if AvEnableInternalAudioGen
            case AudioGen:
                KfunCheckAudioGen(TempPort);
                break;
#endif
#if AvEnableInternalClockGen
            case ClockGen:
                KfunCheckClockGen(TempPort);
                break;
#endif
            }
				
        if(!TempPort->next)
            break;
        else
            TempPort = (AvPort*)TempPort->next;
    }

    return AvOk;
}

/**
 * @brief  Change the routing of ports
 * @return none
 * @note
 */
kapi AvRet AvApiConnectPort(GSV_CONFIG *gsv_config,pin AvPort *FromPort, pout AvPort *ToPort, AvConnectType type)
{
    AvPort *TempPort = NULL;
    /* Step 1. Assign Video Relation */
    if((type == AvConnectVideo) || (type == AvConnectAV))
    {
        switch(FromPort->type)
        {
            case HdmiRx:
            case AnalogRx:
            case HdmiTx:
            case AnalogTx:
            case VideoScaler:
            case VideoColor:
            case VideoGen:
            case DviRx:
            case LogicVideoRx:
            case DviTx:
            case LogicVideoTx:
                ClearVideoFromPort(ToPort);
						
						//AvUapiOutputDebugMessage("AvApiConnectPort +1");
                /* To Port has already been assigned, the new added one is placed to be front */
                if((FromPort->content.RouteVideoToPort) &&
                   (FromPort->content.RouteVideoToPort != (struct AvPort*)ToPort))
                    ToPort->content.RouteVideoNextSameLevelPort = FromPort->content.RouteVideoToPort;
									 
									// AvUapiOutputDebugMessage("AvApiConnectPort +2");
                FromPort->content.RouteVideoToPort = (struct AvPort*)(ToPort);
                ToPort->content.RouteVideoFromPort = (struct AvPort*)FromPort;
                if(ToPort->type == HdmiTx)
                {
									
									//AvUapiOutputDebugMessage("AvApiConnectPort +3");
                    ToPort->content.video->AvailableVideoPackets = 0;
                    /* ToPort->content.tx->Hpd = AV_HPD_FORCE_LOW; */
                    AvHandleEvent(gsv_config,FromPort, AvEventUpStreamConnectNewDownStream, NULL, NULL);
									
									
                }
                break;
            default:
                break;
        }
    }
    /* Step 2. Assign Audio Relation */
    if((type == AvConnectAudio) || (type == AvConnectAV))
    {
        switch(FromPort->type)
        {
            case HdmiRx:
            case HdmiTx:
            case LogicAudioRx:
            case LogicAudioTx:
            case AnalogTx:
            case AnalogRx:
#if AvEnableInternalAudioGen
            case AudioGen:
#endif
                ClearAudioFromPort(ToPort);
                /* To Port has already been assigned, the new added one is placed to be front */
                if((FromPort->content.RouteAudioToPort) &&
                   (FromPort->content.RouteAudioToPort != (struct AvPort*)ToPort))
                    ToPort->content.RouteAudioNextSameLevelPort = FromPort->content.RouteAudioToPort;
                FromPort->content.RouteAudioToPort = (struct AvPort*)ToPort;
                ToPort->content.RouteAudioFromPort = (struct AvPort*)FromPort;
                if(ToPort->type == HdmiTx)
                {
                    ToPort->content.audio->AvailableAudioPackets = 0;
                }
                break;
            default:
                break;
        }
    }
    /* Step 3. Hdcp Update Chain */
    if((type == AvConnectVideo) || (type == AvConnectAV))
    {
        switch(FromPort->type)
        {
            case HdmiRx:
            case HdmiTx:
            case AnalogTx:
            case AnalogRx:
            case VideoScaler:
            case VideoColor:
            case VideoGen:
            case DviTx:
            case LogicVideoTx:
            case DviRx:
            case LogicVideoRx:
                /* by default, the source port should be connected, right after the port  */
                /* is connected, its HdcpSource will be assigned. But if the sink port    */
                /* is placed in advance, its HdcpSource is null, just look into the chain */
                /* to find it. */
                if(FromPort->content.HdcpSource)
                {
                    TempPort = (AvPort*)(FromPort->content.HdcpSource);
                    while(TempPort->content.HdcpSource)
                    {
                        TempPort = (AvPort*)(TempPort->content.HdcpSource);
                    }
                    ToPort->content.HdcpSource = (struct AvPort*)TempPort;
                }
                else
                   ToPort->content.HdcpSource = FromPort->content.HdcpSource;
                break;
            case LogicAudioRx:
            case LogicAudioTx:
                FromPort->content.HdcpSource = NULL;
                break;
        }
    }

    /* Step 4. Uapi Connect */
    AvUapiConnectPort(gsv_config,FromPort, ToPort, type);

    return AvOk;
}

kapi AvRet AvApiPortMemAllocate(GSV_CONFIG *gsv_config,pin AvPort *port)
{
    /* Layer 0. Port Structure */
    port->core.DpllCore = -1;
    port->core.HdmiCore = -1;
    port->core.OsdCore  = -1;
    port->core.VspCore  = -1;
    /* Layer 1. AvPort */
    port->content.ID = NULL;
    port->content.is_PlugRxFsm = NULL;
    port->content.is_active_PlugRxFsm = NULL;
    port->content.is_ReceiverFsm = NULL;
    port->content.is_active_ReceiverFsm = NULL;
    port->content.is_HdcpFsm = NULL;
    port->content.is_active_HdcpFsm = NULL;
    port->content.is_PlugTxFsm = NULL;
    port->content.is_active_PlugTxFsm = NULL;
    port->content.is_TxRoutingFsm = NULL;
    port->content.is_active_TxRoutingFsm = NULL;
#if AvEnableCecFeature /* CEC Related */
    port->content.is_CecFsm = NULL;
    port->content.is_active_CecFsm = NULL;
    port->content.cec = NULL;
#endif /* CEC Related */
    port->content.rx = NULL; /* Rx FSM vars */
    port->content.tx = NULL; /* Tx FSM vars */
    port->content.hdcp = NULL; /* Hdcp vars */
    port->content.hdcptx = NULL; /* Hdcp vars */
    port->content.video = NULL;
    port->content.audio = NULL;
    port->content.scaler = NULL;
    port->content.color = NULL;
#if AvEnableInternalVideoGen
    port->content.vg = NULL;
#endif
#if AvEnableInternalAudioGen
    port->content.ag = NULL;
#endif
#if AvEnableInternalClockGen
    port->content.cg = NULL;
#endif
    port->content.HdcpSource = NULL;
    port->content.HdcpNextSinkPort = NULL;
    port->content.RouteVideoFromPort = NULL;
    port->content.RouteVideoNextSameLevelPort = NULL;
    port->content.RouteVideoToPort = NULL;
    port->content.RouteAudioFromPort = NULL;
    port->content.RouteAudioNextSameLevelPort = NULL;
    port->content.RouteAudioToPort = NULL;

    /* Layer 2. AvContent Details */
    switch(port->type)
    {
        case HdmiRx:
        {
            AvUapiAllocateMemory(gsv_config,sizeof(uint16),    (uint32 *)&(port->content.ID));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),     (uint32 *)&(port->content.is_PlugRxFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),     (uint32 *)&(port->content.is_active_PlugRxFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),     (uint32 *)&(port->content.is_ReceiverFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),     (uint32 *)&(port->content.is_active_ReceiverFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),     (uint32 *)&(port->content.is_HdcpFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),     (uint32 *)&(port->content.is_active_HdcpFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(RxVars),    (uint32 *)&(port->content.rx));
            AvUapiAllocateMemory(gsv_config,sizeof(HdcpRx),    (uint32 *)&(port->content.hdcp));
            AvUapiAllocateMemory(gsv_config,sizeof(AvVideo),   (uint32 *)&(port->content.video));
            AvUapiAllocateMemory(gsv_config,sizeof(AvAudio),   (uint32 *)&(port->content.audio));
            break;
        }
        case DviRx:
        {
            AvUapiAllocateMemory(gsv_config,sizeof(uint16),   (uint32 *)&(port->content.ID));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),    (uint32 *)&(port->content.is_PlugRxFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),    (uint32 *)&(port->content.is_active_PlugRxFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),    (uint32 *)&(port->content.is_ReceiverFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),    (uint32 *)&(port->content.is_active_ReceiverFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(RxVars),   (uint32 *)&(port->content.rx));
            AvUapiAllocateMemory(gsv_config,sizeof(AvVideo),  (uint32 *)&(port->content.video));
            break;
        }
        case AnalogRx:
        {
            AvUapiAllocateMemory(gsv_config,sizeof(uint16),   (uint32 *)&(port->content.ID));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),    (uint32 *)&(port->content.is_ReceiverFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),    (uint32 *)&(port->content.is_active_ReceiverFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(RxVars),   (uint32 *)&(port->content.rx));
            AvUapiAllocateMemory(gsv_config,sizeof(AvVideo),  (uint32 *)&(port->content.video));
            AvUapiAllocateMemory(gsv_config,sizeof(AvAudio),  (uint32 *)&(port->content.audio));
            break;
        }
#if AvEnableVideoLogicBus
        case LogicVideoRx:
        {
            AvUapiAllocateMemory(gsv_config,sizeof(uint16),   (uint32 *)&(port->content.ID));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),    (uint32 *)&(port->content.is_ReceiverFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),    (uint32 *)&(port->content.is_active_ReceiverFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(RxVars),   (uint32 *)&(port->content.rx));
            AvUapiAllocateMemory(gsv_config,sizeof(AvVideo),  (uint32 *)&(port->content.video));
            AvUapiAllocateMemory(gsv_config,sizeof(AvLogicVideo),(uint32 *)&(port->content.lvrx));
            break;
        }
#endif
        case LogicAudioRx:
        {
            AvUapiAllocateMemory(gsv_config,sizeof(uint16),   (uint32 *)&(port->content.ID));
            AvUapiAllocateMemory(gsv_config,sizeof(AvAudio),  (uint32 *)&(port->content.audio));
            break;
        }
        case HdmiTx:
        {
            AvUapiAllocateMemory(gsv_config,sizeof(uint16),   (uint32 *)&(port->content.ID));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),    (uint32 *)&(port->content.is_PlugTxFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),    (uint32 *)&(port->content.is_active_PlugTxFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),    (uint32 *)&(port->content.is_TxRoutingFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),    (uint32 *)&(port->content.is_active_TxRoutingFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),    (uint32 *)&(port->content.is_HdcpFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),    (uint32 *)&(port->content.is_active_HdcpFsm));
#if AvEnableCecFeature /* CEC Related */
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),    (uint32 *)&(port->content.is_CecFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),    (uint32 *)&(port->content.is_active_CecFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(AvCec),    (uint32 *)&(port->content.cec));
#endif /* CEC Related */
            AvUapiAllocateMemory(gsv_config,sizeof(TxVars),   (uint32 *)&(port->content.tx));
            AvUapiAllocateMemory(gsv_config,sizeof(HdcpTx),   (uint32 *)&(port->content.hdcptx));
            AvUapiAllocateMemory(gsv_config,sizeof(AvVideo),  (uint32 *)&(port->content.video));
            AvUapiAllocateMemory(gsv_config,sizeof(AvAudio),  (uint32 *)&(port->content.audio));
            /* av_platf_enter_internal_HdcpFsm(port); */
            break;
        }
        case DviTx:
        {
            AvUapiAllocateMemory(gsv_config,sizeof(uint16),   (uint32 *)&(port->content.ID));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),    (uint32 *)&(port->content.is_PlugTxFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),    (uint32 *)&(port->content.is_active_PlugTxFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),    (uint32 *)&(port->content.is_TxRoutingFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),    (uint32 *)&(port->content.is_active_TxRoutingFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(TxVars),   (uint32 *)&(port->content.tx));
            AvUapiAllocateMemory(gsv_config,sizeof(AvVideo),  (uint32 *)&(port->content.video));
            AvUapiAllocateMemory(gsv_config,sizeof(AvAudio),  (uint32 *)&(port->content.audio));
            break;
        }
        case AnalogTx:
        {
            AvUapiAllocateMemory(gsv_config,sizeof(uint16),   (uint32 *)&(port->content.ID));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),    (uint32 *)&(port->content.is_TxRoutingFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),    (uint32 *)&(port->content.is_active_TxRoutingFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(TxVars),   (uint32 *)&(port->content.tx));
            AvUapiAllocateMemory(gsv_config,sizeof(AvVideo),  (uint32 *)&(port->content.video));
            AvUapiAllocateMemory(gsv_config,sizeof(AvAudio),  (uint32 *)&(port->content.audio));
            break;
        }
        case VideoScaler:
        {
            AvUapiAllocateMemory(gsv_config,sizeof(uint16),   (uint32 *)&(port->content.ID));
            AvUapiAllocateMemory(gsv_config,sizeof(AvScaler), (uint32 *)&(port->content.scaler));
            break;
        }
        case VideoColor:
        {
            AvUapiAllocateMemory(gsv_config,sizeof(uint16),   (uint32 *)&(port->content.ID));
            AvUapiAllocateMemory(gsv_config,sizeof(AvColor),  (uint32 *)&(port->content.color));
            break;
        }
#if AvEnableInternalVideoGen
        case VideoGen:
        {
            AvUapiAllocateMemory(gsv_config,sizeof(uint16),   (uint32 *)&(port->content.ID));
            AvUapiAllocateMemory(gsv_config,sizeof(RxVars),   (uint32 *)&(port->content.rx));
            AvUapiAllocateMemory(gsv_config,sizeof(AvVideo),  (uint32 *)&(port->content.video));
            AvUapiAllocateMemory(gsv_config,sizeof(AvVideoGen),(uint32 *)&(port->content.vg));
            break;
        }
#endif
#if AvEnableInternalAudioGen
        case AudioGen:
        {
            AvUapiAllocateMemory(gsv_config,sizeof(uint16),   (uint32 *)&(port->content.ID));
            AvUapiAllocateMemory(gsv_config,sizeof(RxVars),   (uint32 *)&(port->content.rx));
            AvUapiAllocateMemory(gsv_config,sizeof(AvAudio),  (uint32 *)&(port->content.audio));
            AvUapiAllocateMemory(gsv_config,sizeof(AvAudioGen),(uint32 *)&(port->content.ag));
            break;
        }
#endif
#if AvEnableInternalClockGen
        case ClockGen:
        {
            AvUapiAllocateMemory(gsv_config,sizeof(uint16),   (uint32 *)&(port->content.ID));
            AvUapiAllocateMemory(gsv_config,sizeof(AvClockGen),(uint32 *)&(port->content.cg));
            break;
        }
#endif
#if AvEnableVideoLogicBus
        case LogicVideoTx:
        {
            AvUapiAllocateMemory(gsv_config,sizeof(uint16),   (uint32 *)&(port->content.ID));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),    (uint32 *)&(port->content.is_TxRoutingFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(uint8),    (uint32 *)&(port->content.is_active_TxRoutingFsm));
            AvUapiAllocateMemory(gsv_config,sizeof(AvVideo),  (uint32 *)&(port->content.video));
            AvUapiAllocateMemory(gsv_config,sizeof(AvLogicVideo),(uint32 *)&(port->content.lvtx));
            break;
        }
#endif
        case LogicAudioTx:
        {
            AvUapiAllocateMemory(gsv_config,sizeof(uint16),   (uint32 *)&(port->content.ID));
            AvUapiAllocateMemory(gsv_config,sizeof(AvAudio),  (uint32 *)&(port->content.audio));
            break;
        }
    }

    return AvOk;

}

void ClearVideoFromPort(pin AvPort *port)
{
    /* clarify the routing in AvPort structure and AvDevice structure */
    AvPort *AbandonRxPort = NULL;

    /* 1. Find abandoned rx port and tx port */
    AbandonRxPort = port;
    while(AbandonRxPort->content.RouteVideoFromPort != NULL)
    {
        AbandonRxPort = (AvPort*)AbandonRxPort->content.RouteVideoFromPort;
        /* 1.1 Remove video ToPort in FromPort */
        if(AbandonRxPort->content.RouteVideoToPort == (struct AvPort*)port)
            AbandonRxPort->content.RouteVideoToPort = port->content.RouteVideoNextSameLevelPort;
        if(AbandonRxPort->content.RouteVideoNextSameLevelPort == (struct AvPort*)port)
            AbandonRxPort->content.RouteVideoNextSameLevelPort = port->content.RouteVideoNextSameLevelPort;
    }
    AbandonRxPort = port->device->port; /* Get 1st Port of the device */
    while((AbandonRxPort != NULL) && (AbandonRxPort->device->index == port->device->index))
    {
        /* 1.2 Remove video ToPort in FromPort */
        if(AbandonRxPort->content.RouteVideoToPort == (struct AvPort*)port)
            AbandonRxPort->content.RouteVideoToPort = port->content.RouteVideoNextSameLevelPort;
        if(AbandonRxPort->content.RouteVideoNextSameLevelPort == (struct AvPort*)port)
            AbandonRxPort->content.RouteVideoNextSameLevelPort = port->content.RouteVideoNextSameLevelPort;
        AbandonRxPort = (AvPort*)(AbandonRxPort->next);
    }

    /* 2. clear tx's pointers, in case of already cleared by kapi layer, add protection */
    port->content.RouteVideoFromPort = NULL;
    port->content.RouteVideoNextSameLevelPort = NULL;
}

void ClearAudioFromPort(pin AvPort *port)
{
    /* clarify the routing in AvPort structure and AvDevice structure */
    AvPort *AbandonRxPort = NULL;

    /* 1. Find abandoned rx port and tx port */
    AbandonRxPort = port;
    while(AbandonRxPort->content.RouteAudioFromPort != NULL)
    {
        AbandonRxPort = (AvPort*)AbandonRxPort->content.RouteAudioFromPort;
        /* 1.1 Remove audio ToPort in FromPort */
        if(AbandonRxPort->content.RouteAudioToPort == (struct AvPort*)port)
            AbandonRxPort->content.RouteAudioToPort = port->content.RouteAudioNextSameLevelPort;
        if(AbandonRxPort->content.RouteAudioNextSameLevelPort == (struct AvPort*)port)
            AbandonRxPort->content.RouteAudioNextSameLevelPort = port->content.RouteAudioNextSameLevelPort;
    }
    AbandonRxPort = port->device->port; /* Get 1st Port of the device */
    while((AbandonRxPort != NULL) && (AbandonRxPort->device->index == port->device->index))
    {
        /* 1.2 Remove audio ToPort in FromPort */
        if(AbandonRxPort->content.RouteAudioToPort == (struct AvPort*)port)
            AbandonRxPort->content.RouteAudioToPort = port->content.RouteAudioNextSameLevelPort;
        if(AbandonRxPort->content.RouteAudioNextSameLevelPort == (struct AvPort*)port)
            AbandonRxPort->content.RouteAudioNextSameLevelPort = port->content.RouteAudioNextSameLevelPort;
        AbandonRxPort = (AvPort*)(AbandonRxPort->next);
    }

    /* 2. clear tx's pointers, in case of already cleared by kapi layer, add protection */
    port->content.RouteAudioFromPort = NULL;
    port->content.RouteAudioNextSameLevelPort = NULL;
}

/**
 * @brief  add audio/video port
 * @return none
 * @note this function is used for initializing port FSMs.
 */
kapi AvRet AvApiPortStart(GSV_CONFIG *gsv_config)
{
    AvPort *TempPort = gsv_config->FirstPort;
		
		
		

	
    while(TempPort)
    {
        AvApiInitFsm(TempPort);
        TempPort = (AvPort*)TempPort->next;
    }
    return AvOk;
}

#if AvEnableCecFeature /* CEC Related */
kapi AvRet AvKapiCecSendMessage(AvPort *port)
{
    return KfunCecSendMessage(port);
}

kapi AvRet AvKapiCecSetPhysicalAddr(AvPort *port)
{
    return KfunCecSetPhysicalAddr(port);
}

kapi AvRet AvKapiCecSetLogicalAddr(AvPort *port)
{
    return KfunCecSetLogicalAddr(port);
}

kapi AvRet AvKapiArcEnable(GSV_CONFIG *gsv_config,AvPort *port, uint8 value)
{
    return AvUapiTxArcEnable(gsv_config,port, value);
}

#endif /* CEC Related */
