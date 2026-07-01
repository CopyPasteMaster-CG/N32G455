/**
 * @file uapi_function_mapper.h
 *
 * @brief low level ports related universal api functions
 */


#ifndef _UAPI_FUNCTION_MAPPER_H
#define _UAPI_FUNCTION_MAPPER_H

#ifdef __cplusplus
    extern "C" {
#endif

typedef enum
{
    AvVideoPacketType,
    AvAudioPacketType
} AvPacketType;

typedef enum
{
    AvRxUpdate,
    AvRxForce,
    AvRxKeep
} AvRxUpdateType;

#include "gsv2011_config.h"

/* PART 1 */
/* Receiver FSM */
void KfunRxVarInit(GSV_CONFIG *gsv_config,pin AvPort *port);
void KfunRxInit(pin AvPort *port);
void KfunRxVideoManage(GSV_CONFIG *gsv_config,pin AvPort *port);
void KfunRxAudioManage(GSV_CONFIG *gsv_config,pin AvPort *port);
void KfunRxEnableFrun(pin AvPort *port);
void KfunRxDisableFrun(pin AvPort *port);
void KfunRxClearFlags(GSV_CONFIG *gsv_config,pin AvPort *port);
void KfunPollingRxStatus(pin AvPort *port);

/* PART 2 */
/* HDCP FSM */
void KfunHdcpVarInit(GSV_CONFIG *gsv_config,pin AvPort *port);
void KfunHdcpDetectSource(GSV_CONFIG *gsv_config,pin AvPort *port);
void KfunHdcpDetectSink(GSV_CONFIG *gsv_config,pin AvPort *port);
void KfunSimpleHdcpSync(GSV_CONFIG *gsv_config,pin AvPort *port);
void KfunReadBksv(pin AvPort *port);
AvPort *KfunLookupHdcpSource(pin AvPort *port);
void KfunUploadSinkInfo(pin AvPort *port);
void KfunCheckEncryption(GSV_CONFIG *gsv_config,pin AvPort *port);
void KfunEncryptSink(pin AvPort *port);
void KfunDecryptSink(pin AvPort *port);
void KfunDecryptNewSink(pin AvPort *port);
void KfunRxConnection(pio AvPort *port);
void KfunTxConnection(GSV_CONFIG *gsv_config,pio AvPort *port);
void KfunHdcpUnMuteAv(pio AvPort *port);
void KfunHdcpMuteAv(GSV_CONFIG *gsv_config,pio AvPort *port);
AvRet KfunCopyBksv(pout AvPort *RxPort, pin AvPort *TxPort);
void KfunHdcpReset(GSV_CONFIG *gsv_config,pio AvPort *port);
void KfunTxHdcpManage(GSV_CONFIG *gsv_config,pio AvPort *port);
void KfunTxSetMuteAv(GSV_CONFIG *gsv_config,pio AvPort *port);
void KfunTxClearMuteAv(GSV_CONFIG *gsv_config,pio AvPort *port);
void KfunHdcp2p2Manage(pio AvPort *port);
void KfunTxUnlockProtection(GSV_CONFIG *gsv_config,pio AvPort *port);

/* PART 3 */
/* Transmitter FSM */
void KfunTxTmdsMute(GSV_CONFIG *gsv_config,pout AvPort *port);
void KfunTxTmdsUnmute(GSV_CONFIG *gsv_config,pout AvPort *port);
AvRet KfunTxVideoManage(GSV_CONFIG *gsv_config,pout AvPort *port);
AvRet KfunTxAudioManage(GSV_CONFIG *gsv_config,pout AvPort *port);
void KfunPollingTxStatus(pout AvPort *port);
AvRet KfunTxUseDefaultEdid(GSV_CONFIG *gsv_config,pin AvPort *port);
AvRet KfunTxEdidError(GSV_CONFIG *gsv_config,pin AvPort *port);
uint8 KfunFindCecSPAFromEdid(GSV_CONFIG *gsv_config,uint8 *currptr, uint8 *spa);
void KfunGenerateSourceSpa(AvPort *port, uint8 *Spa, uint8 DedicateAddr);
AvRet KfunCecUpdateFromEdid(GSV_CONFIG *gsv_config,AvPort *port);
AvRet KfunTxSinkLost(GSV_CONFIG *gsv_config,AvPort *port);
void KfunTxSetColorDepth(AvPort *RxPort, AvPort *TxPort);
void KfunTxSetHdmiModeSupport(GSV_CONFIG *gsv_config,pout AvPort *port);

/* PART 4 */
/* Routing FSM */
void KfunTxRoutingVarInit(pout AvPort *port);
void KfunRxRoutingVarInit(pout AvPort *port);
void KfunRxRoutingClearEndInfo(pin AvPort *port);

/* PART 5 */
/* Plug RX FSM */
void KfunPrVarInit(pin AvPort *port);
void KfunPrResetPort(GSV_CONFIG *gsv_config,pin AvPort *port);
void KfunPrHpdDown(GSV_CONFIG *gsv_config,pin AvPort *port);
void KfunPrHpdUp(GSV_CONFIG *gsv_config,pin AvPort *port);
void KfunManageHpa(GSV_CONFIG *gsv_config,pin AvPort *port);
void KfunPrepareEdid(GSV_CONFIG *gsv_config,pin AvPort *port);
void KfunStdiReadStatus(pin AvPort *port);
void KfunRxReadInfo(GSV_CONFIG *gsv_config,pin AvPort *port);
void KfunRxWriteEdid(GSV_CONFIG *gsv_config,pio AvPort *port, uint8 *MergedEdid, uint8 SpaLocation, uint8 *SpaValue);
void KfunTxReadEdid(GSV_CONFIG *gsv_config,AvPort *port, uint8 *ReadData);

/* PART 6 */
/* Plug TX FSM */
void KfunPtVarInit(GSV_CONFIG *gsv_config,pout AvPort *port);
void KfunPtPortInit(pout AvPort *port);
void KfunPtResetPort(GSV_CONFIG *gsv_config,pout AvPort *port);
void KfunPtEnableTxCore(GSV_CONFIG *gsv_config,pout AvPort *port);
void KfunPtAntiDither(pout AvPort *port);
void KfunTransmitVideo(pout AvPort *port);
AvPort* KfunLookupFromPort(AvPort *port);

#if AvEnableCecFeature /* CEC Related */
/* PART 7 */
AvRet ValidMsgSize (CecMessage Opcode, uchar MsgSize);
void SendSetSystemAudioMode (AvPort* port, uint8 status);
AvRet KfunCecInit(AvPort *port);
AvRet KfunCecRxMsgProcess(AvPort *port);
AvRet KfunPrepareLogAddr(AvPort *port);
AvRet KfunCecSendMessage(AvPort *port);
AvRet KfunCecSetPhysicalAddr(AvPort *port);
AvRet KfunCecSetLogicalAddr(AvPort *port);
AvRet KfunCecManage(AvPort *port);
#endif

/* Look Up Port */
AvRet KfunUpdatePortFromPacket(GSV_CONFIG *gsv_config,pin AvPort *port, PacketType pkt, uint8 *Content);
AvRet KfunCheckVspPortContent(GSV_CONFIG *gsv_config,pin AvPort *port, PacketType Pkt, uint8 *Content);
AvRet KfunFindAudioNextTxEnd(AvPort* CurrentNode, AvPort* *CompareNode, AvPort* *FoundNode);
AvRet KfunFindAudioNextNode(AvPort* CurrentNode, AvPort* *CompareNode, AvPort* *FoundNode);
AvRet KfunFindAudioRxFront(AvPort* CurrentNode, AvPort** FoundNode);
AvRet KfunFindVideoNextTxEnd(AvPort* CurrentNode, AvPort* *CompareNode, AvPort* *FoundNode);
AvRet KfunFindVideoNextNode(AvPort* CurrentNode, AvPort* *CompareNode, AvPort* *FoundNode);
AvRet KfunFindVideoRxFront(AvPort* CurrentNode, AvPort** FoundNode);

#ifdef __cplusplus
    }
#endif

#endif
