/**
 * @file av_main.c
 *
 * @brief sample main entry for audio/video based software
 */
#include "av_main.h"
#include "global_var.h"

#include "BSP_GSV2011.h"
#include "rtthread.h"
#include "APP_GSV2011.h"
#include "uapi.h"
#include "kernel_fsm.h"
#include "BSP_USART.h"
#include "stdio.h"
//#include "APP_GSV2011.h"
AvRet BspGetMilliSecond(uint32 *ms)
{
    *ms = rt_tick_get();
    return AvOk;
}

//extern uint8 EdidHdmi2p0;
//extern uint8 LogicOutputSel;

void AvFpKeyCommand_NULL (AvPort *port)
{
}


/**
 * @brief  sample main entry for audio/video based software
 * @return never return
 */
#ifdef GSV_BASE_PROJECT
  int main(void)
#else
  int GsvMain( GSV_CONFIG *gsv_config)
#endif

{
	uint32 tick;
	
	  /* 1. Low Level Hardware Level Initialization */
    /* 1.1 init bsp support (user speficic) */
	
    gsv_config->bsp->Init();
    rt_thread_delay(100);
    gsv_config->bsp->RST_Disable();
    rt_thread_delay(100);
  

    /* 1.2 init software package and hookup user's bsp functions */
    AvApiInit( gsv_config );

   
				
    AvApiHookBspFunctions( gsv_config,
                gsv_config->bsp->I2cRead,
                gsv_config->bsp->I2cWrite,
                gsv_config->bsp->UartSendByte,
                gsv_config->bsp->UartGetByte, 
                &BspGetMilliSecond, 
                gsv_config->bsp->GetKey, 
                gsv_config->bsp->IrdaGetByte );			
		
		    
		
//		if(gsv_config->index == 0 ){
		
//			AvApiHookUserFunctions(&ListenToKeyCommand, &ListenToUartCommand, &ListenToIrdaCommand);
			AvApiHookUserFunctions(gsv_config, AvFpKeyCommand_NULL, AvFpKeyCommand_NULL, AvFpKeyCommand_NULL);
//		kapi AvRet AvKapiHookUserFunctions(GSV_CONFIG *gsv_config , pin AvFpKeyCommand keyCmd , pin AvFpUartCommand uartCmd,pin AvFpIrdaCommand IrdaCmd)

//		}

    /* 2. Device Level Declaration */
    /* 2.1 total devices */
    /* it must be declared in AvDevice */
    AvDevice devices[1];

    /* 2.2 specific devices and ports */
    /* they must be able to be linked to the device in 1. */
    Gsv2k11Device gsv2k11_0;
    AvPort gsv2k11Ports[9];


    rt_memset(devices,0,sizeof(devices));
    rt_memset(&gsv2k11_0,0,sizeof(gsv2k11_0));
    rt_memset(gsv2k11Ports,0,sizeof(gsv2k11Ports));
        /* 2.3 init device address in 2.2 */
    gsv2k11_0.DeviceAddress = AvGenerateDeviceAddress(0x00,0x01,0xB0,0x00);

    /* 2.4 connect devices to device declaration */
    AvApiAddDevice(&devices[0], Gsv2k11, 0, (void *)&gsv2k11_0, (void *)&gsv2k11Ports[0],  NULL);

    /* 3. Port Level Declaration */
    /* 3.1 init devices and port structure, must declare in number order */
    /* 0-3 HdmiRx, 4-7 HdmiTx, 8-9 TTLTx, 10-11 TTLRx,
       20-23 Scaler, 24-27 Color, 28 VideoGen, 30 VideoIn, 32 VideoOut,
       34 AudioGen, 36 ClockGen */
    AvApiAddPort(gsv_config, &devices[0],&gsv2k11Ports[0] ,0 ,HdmiRx);
    AvApiAddPort(gsv_config, &devices[0],&gsv2k11Ports[1] ,5 ,HdmiTx);
    AvApiAddPort(gsv_config, &devices[0],&gsv2k11Ports[2] ,32,LogicVideoTx);
    AvApiAddPort(gsv_config, &devices[0],&gsv2k11Ports[3] ,8 ,LogicAudioTx);
    AvApiAddPort(gsv_config, &devices[0],&gsv2k11Ports[4] ,20,VideoScaler);
    AvApiAddPort(gsv_config, &devices[0],&gsv2k11Ports[5] ,24,VideoColor);
    AvApiAddPort(gsv_config, &devices[0],&gsv2k11Ports[6] ,28,VideoGen);
    AvApiAddPort(gsv_config, &devices[0],&gsv2k11Ports[7] ,30,LogicVideoRx);
    AvApiAddPort(gsv_config, &devices[0],&gsv2k11Ports[8] ,10,LogicAudioRx);

    /* 3.2 initialize port content */
#if AvEnableCecFeature
    gsv2k11Ports[1].content.cec->CecEnable = 1;
    if(AudioStatus == 0)
        gsv2k11Ports[1].content.cec->EnableAudioAmplifier = AV_CEC_AMP_TO_DISABLE;
    else
    {
        gsv2k11Ports[1].content.cec->EnableAudioAmplifier = AV_CEC_AMP_TO_ENABLE;
        gsv2k11Ports[1].content.cec->EnableARC = AV_CEC_ARC_TO_INITIATE;
    }
    Cec_Tx_Audio_Status.Volume = 30;
    Cec_Tx_Audio_Status.Mute   = 0;    /*  */
    Cec_Tx_Audio_Status.AudioMode = 1; /* Audio Mode is ON to meet ARC */
    Cec_Tx_Audio_Status.AudioRate = 1; /* 100% rate */
    Cec_Tx_Audio_Status.AudioFormatCode = AV_AUD_FORMAT_LINEAR_PCM; /* Follow Spec */
    Cec_Tx_Audio_Status.MaxNumberOfChannels = 2; /* Max Channels */
    Cec_Tx_Audio_Status.AudioSampleRate = 0x07; /* 32KHz/44.1KHz/48KHz */
    Cec_Tx_Audio_Status.AudioBitLen = 0x01;  /* 16-bit only */
    Cec_Tx_Audio_Status.MaxBitRate  = 0;  /* default */
    Cec_Tx_Audio_Status.ActiveSource = 0; /* default */
#endif

    /* 3.3 init fsms */
    AvApiInitDevice( gsv_config, &devices[0]);
		

    AvApiPortStart(gsv_config);
		



    /* 3.4 routing */
    /* connect the port by video using AvConnectVideo */
    /* connect the port by audio using AvConnectAudio */
    /* connect the port by video and audio using AvConnectAV */

    /* 3.4.1 video routing */
    /* CHIP1 setting */
    /* case 1: default routing RxA->TxB */
//		LogicOutputSel = 0;
    if(gsv_config->LogicOutputSel == 1)
    {
        AvApiConnectPort(gsv_config,&gsv2k11Ports[0], &gsv2k11Ports[1], AvConnectAV);
        AvApiConnectPort(gsv_config,&gsv2k11Ports[0], &gsv2k11Ports[2], AvConnectVideo);
        AvApiConnectPort(gsv_config,&gsv2k11Ports[0], &gsv2k11Ports[3], AvConnectAudio);
    }
    /* case 2: routing of LogicTx/Rx->TxB */
    else
    {
        AvApiConnectPort(gsv_config,&gsv2k11Ports[7], &gsv2k11Ports[1], AvConnectVideo);
        AvApiConnectPort(gsv_config,&gsv2k11Ports[8], &gsv2k11Ports[1], AvConnectAudio);
    }
		
		
		
    LogicLedOut(gsv_config->LogicOutputSel );

    /* 3.4.2 ARC Connection, set after rx port connection to avoid conflict */
#if AvEnableCecFeature
    if(AudioStatus == 1)
    {
        AvApiConnectPort(gsv_config,&gsv2k11Ports[0], &gsv2k11Ports[1], AvConnectAudio);
    }
#endif

    /* 3.4.3 Internal Video Generator*/
#if AvEnableInternalVideoGen
    gsv2k11Ports[6].content.video->timing.Vic = 0x61; /* 1080p60 */ //0x10
    gsv2k11Ports[6].content.video->AvailableVideoPackets = AV_BIT_AV_INFO_FRAME;
    gsv2k11Ports[6].content.video->Cd         = AV_CD_24;
    gsv2k11Ports[6].content.video->Y          = AV_Y2Y1Y0_RGB;
    gsv2k11Ports[6].content.vg->Pattern       = AV_PT_COLOR_BAR;
    AvApiConnectPort(gsv_config,&gsv2k11Ports[6],&gsv2k11Ports[1],AvConnectVideo);//////  GSV intel gen colobar
#endif

    /* 3.4.4 Audio Insertion */
#if AvEnableAudioTTLInput
    gsv2k11Ports[8].content.audio->AudioMute    = 0;
//    gsv2k11Ports[8].content.audio->AudFormat    = AV_AUD_I2S;
    gsv2k11Ports[8].content.audio->AudFormat    = AV_AUD_SPDIF;
    gsv2k11Ports[8].content.audio->AudType      = AV_AUD_TYPE_ASP;
    gsv2k11Ports[8].content.audio->AudCoding    = AV_AUD_FORMAT_LINEAR_PCM;
    gsv2k11Ports[8].content.audio->AudMclkRatio = AV_MCLK_256FS;
    gsv2k11Ports[8].content.audio->Layout       = 0;    /* 2 channel Layout = 0 */
    gsv2k11Ports[8].content.audio->Consumer     = 0;    /* Consumer */
    gsv2k11Ports[8].content.audio->Copyright    = 0;    /* Copyright asserted */
    gsv2k11Ports[8].content.audio->Emphasis     = 0;    /* No Emphasis */
    gsv2k11Ports[8].content.audio->CatCode      = 0;    /* Default */
    gsv2k11Ports[8].content.audio->SrcNum       = 0;    /* Refer to Audio InfoFrame */
    gsv2k11Ports[8].content.audio->ChanNum      = 2;    /* Audio Channel Count */
    gsv2k11Ports[8].content.audio->SampFreq     = AV_AUD_FS_48KHZ; /* Sample Frequency */
    gsv2k11Ports[8].content.audio->ClkAccur     = 0;    /* Level 2 */
    gsv2k11Ports[8].content.audio->WordLen      = 0x0B; /* 24-bit word length */
#endif

    /* 3.4.5 Video Parallel Bus Input */
    /* CommonBusConfig = 0 to disable, CommonBusConfig = 1~64 for feature setting */
    uint8 CommonBusConfig = 11;
    gsv2k11Ports[2].content.lvtx->Config        = CommonBusConfig;
    /* 3.4.5.1 LogicVideoTx Port's Y and InCS
       = AV_Y2Y1Y0_INVALID/AV_CS_AUTO to do no 2011 color processing,
       = Dedicated Color for internal Color/Scaler Processing */
    if((ParallelConfigTable[CommonBusConfig*3 + 1] & 0x04) != 0)
    {
        gsv2k11Ports[2].content.video->Y           = AV_Y2Y1Y0_YCBCR_422;
        gsv2k11Ports[2].content.video->InCs        = AV_CS_LIM_YUV_709;
    }
    else
    {
        gsv2k11Ports[2].content.video->Y           = AV_Y2Y1Y0_INVALID;
        gsv2k11Ports[2].content.video->InCs        = AV_CS_AUTO;
    }
    /* 3.4.5.2 LogicVideoTx Port's Limited Highest Pixel Clock Frequency
      = 600 to output HDMI 2.0 on Parallel bus,
      = 300 to output HDMI 1.4 on Parallel bus,
      = 150 to output 1080p on Parallel bus */
    gsv2k11Ports[2].content.video->info.TmdsFreq   = 600;

    /* 3.4.6 Video Parallel Bus Input */
    gsv2k11Ports[7].content.video->timing.Vic  = 0x61; /* 1080P60 */
    gsv2k11Ports[7].content.video->AvailableVideoPackets = AV_BIT_GC_PACKET | AV_BIT_AV_INFO_FRAME;
    gsv2k11Ports[7].content.video->Cd          = AV_CD_24;
    if((ParallelConfigTable[CommonBusConfig*3 + 1] & 0x04) != 0)
    {
        gsv2k11Ports[7].content.video->Y           = AV_Y2Y1Y0_YCBCR_422;
        gsv2k11Ports[7].content.video->InCs        = AV_CS_LIM_YUV_709;
    }
    else
    {
        gsv2k11Ports[7].content.video->Y           = AV_Y2Y1Y0_RGB;
        gsv2k11Ports[7].content.video->InCs        = AV_CS_RGB;
			

	
    }
    gsv2k11Ports[7].content.lvrx->Config       = 11;

    /* 3.4.7 Video Parallel Bus Config */
    gsv2k11Ports[7].content.rx->VideoEncrypted = 0;
    if(gsv_config->LogicOutputSel  == 1)
    {
        gsv2k11Ports[2].content.lvtx->Update       = 1;
    }
    else
    {
        gsv2k11Ports[7].content.lvrx->Update       = 1;
    }
			
    /* 4. routine */
    uint8 NewVic = 0x61;
    uint16 PixelFreq = 0;
 uint8 CommonBusConfig_new ;
    char str[100];
//		int htotal;
    /* call update api to enter into audio/video software loop */
    while(1)
    {
        AvApiUpdate(gsv_config);

        
        AvPortConnectUpdate(gsv_config,&devices[0]);
			
        /* 4.1 switch Vic based on frequency */
        if((gsv_config->LogicOutputSel  == 0) && (gsv2k11Ports[7].content.lvrx->Lock == 1))
        {
            CommonBusConfig=11;
            gsv2k11Ports[7].content.lvrx->Config       = 11;
            PixelFreq = gsv2k11Ports[7].content.video->info.TmdsFreq;
            if((ParallelConfigTable[CommonBusConfig*3 + 2] & 0x40) == 0x40)
            {
                if((ParallelConfigTable[CommonBusConfig*3 + 2] & 0x01) == 0x00)
                    PixelFreq = PixelFreq / 2;
            }
            else
            {
                if((ParallelConfigTable[CommonBusConfig*3 + 2] & 0x01) == 0x00)
                    PixelFreq = PixelFreq * 2;
            }
            if(PixelFreq > 590)
                NewVic = 0x61;
            else if(PixelFreq > 290)
                NewVic = 0x5F;
            else if(PixelFreq > 145)
                NewVic = 0x10;
            else if(PixelFreq > 70)
                NewVic = 0x04;
            else
                NewVic = 0x02;
            if(NewVic != gsv2k11Ports[7].content.video->timing.Vic)
            {
                gsv2k11Ports[7].content.video->timing.Vic = NewVic;
//                sprintf(str,"\r\n 1输出 CommonBusConfig %d index %d NewVic %d", CommonBusConfig,gsv_config->index,NewVic);
//                BSP_USART_FUN.Write(str, strlen(str));
                gsv2k11Ports[7].content.lvrx->Update      = 1;
            }
        }
        
        ///////////////////////输入///////////////////////////
        #if 1
        if((gsv_config->index!=0)&&(gsv_config->LogicOutputSel  == 1)&&(gsv2k11Ports[2].content.lvtx->Lock)){
     
            CommonBusConfig_new=gsv2k11Ports[2].content.lvtx->Config;
            if((gsv2k11Ports[0].content.video->timing.HActive<=1280)&&(gsv2k11Ports[0].content.video->timing.VActive<=720)){
                CommonBusConfig_new=10;
          
                 gsv2k11Ports[2].content.lvtx->Config        = CommonBusConfig_new;
    
                if((ParallelConfigTable[CommonBusConfig_new*3 + 1] & 0x04) != 0)
                {
                    gsv2k11Ports[2].content.video->Y           = AV_Y2Y1Y0_YCBCR_422;
                    gsv2k11Ports[2].content.video->InCs        = AV_CS_LIM_YUV_709;
                }
                else
                {
                    gsv2k11Ports[2].content.video->Y           = AV_Y2Y1Y0_INVALID;
                    gsv2k11Ports[2].content.video->InCs        = AV_CS_AUTO;
                }
                gsv2k11Ports[2].content.video->info.TmdsFreq   = 150;
//                sprintf(str,"\r\n 720 CommonBusConfig %d index %d", CommonBusConfig_new,gsv_config->index);
//                BSP_USART_FUN.Write(str, strlen(str));
        
        }else {
        
             CommonBusConfig_new=11;
             gsv2k11Ports[2].content.lvtx->Config        = CommonBusConfig_new;
    
                if((ParallelConfigTable[CommonBusConfig_new*3 + 1] & 0x04) != 0)
                {
                    gsv2k11Ports[2].content.video->Y           = AV_Y2Y1Y0_YCBCR_422;
                    gsv2k11Ports[2].content.video->InCs        = AV_CS_LIM_YUV_709;
                }
                else
                {
                    gsv2k11Ports[2].content.video->Y           = AV_Y2Y1Y0_INVALID;
                    gsv2k11Ports[2].content.video->InCs        = AV_CS_AUTO;
                }
                       
                
                gsv2k11Ports[2].content.video->info.TmdsFreq   = 600;
//                sprintf(str,"\r\n 10 CommonBusConfig %d index %d", CommonBusConfig_new,gsv_config->index);
//                BSP_USART_FUN.Write(str, strlen(str));
        }
        #endif
         if(CommonBusConfig_new != CommonBusConfig)
            {
                CommonBusConfig=CommonBusConfig_new;
                //gsv2k11Ports[7].content.video->timing.Vic = NewVic;
                sprintf(str,"\r\n CommonBusConfig %d index %d", CommonBusConfig_new,gsv_config->index);
                BSP_USART_FUN.Write(str, strlen(str));
                gsv2k11Ports[2].content.lvtx->Update      = 1;
            }
    }
        
        if((rt_tick_get()-tick)>10000){
        
            tick= rt_tick_get();
            AvUapiOutputDebugMessage(gsv_config,"\r\n\r\n\r\n");
            if(gsv2k11Ports[0].content.video!=NULL){
                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.video->info.TmdsFreq     %d", gsv2k11Ports[0].content.video->info.TmdsFreq);
                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.video->timing.FrameRate  %d", gsv2k11Ports[0].content.video->timing.FrameRate);
                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.video->timing.HActive    %d", gsv2k11Ports[0].content.video->timing.HActive);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.video->timing.HBack      %d", gsv2k11Ports[0].content.video->timing.HBack);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.video->timing.HPolarity  %d", gsv2k11Ports[0].content.video->timing.HPolarity);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.video->timing.HSync      %d", gsv2k11Ports[0].content.video->timing.HSync);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.video->timing.HTotal     %d", gsv2k11Ports[0].content.video->timing.HTotal);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.video->timing.Interlaced %d", gsv2k11Ports[0].content.video->timing.Interlaced);
                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.video->timing.VActive    %d", gsv2k11Ports[0].content.video->timing.VActive);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.video->timing.VBack      %d", gsv2k11Ports[0].content.video->timing.VBack);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.video->timing.Vic        %d", gsv2k11Ports[0].content.video->timing.Vic);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.video->timing.VPolarity  %d", gsv2k11Ports[0].content.video->timing.VPolarity);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.video->timing.VSync      %d", gsv2k11Ports[0].content.video->timing.VSync);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.video->timing.VTotal     %d", gsv2k11Ports[0].content.video->timing.VTotal);
           
//sprintf(str, "\r\n +++ gsv2k11Ports %d   %d  %d %d  %d+++",gsv_config->index,gsv2k11Ports[0].content.video->timing.HActive,gsv2k11Ports[0].content.video->timing.VActive,gsv2k11Ports[0].content.video->info.TmdsFreq,gsv2k11Ports[0].content.video->timing.FrameRate);
//    BSP_USART_FUN.Write(str, strlen(str));            
                }
            if(gsv2k11Ports[0].content.audio!=NULL){
// gsv2k11Ports[2].content.lvtx->Update      = 1;
                
                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.audio->AudCoding             %d", gsv2k11Ports[0].content.audio->AudCoding);
                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.audio->AudFormat             %d", gsv2k11Ports[0].content.audio->AudFormat);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.audio->AudioMute             %d", gsv2k11Ports[0].content.audio->AudioMute);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.audio->AudMclkRatio          %d", gsv2k11Ports[0].content.audio->AudMclkRatio);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.audio->AudType               %d", gsv2k11Ports[0].content.audio->AudType);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.audio->AvailableAudioPackets %d", gsv2k11Ports[0].content.audio->AvailableAudioPackets);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.audio->CatCode               %d", gsv2k11Ports[0].content.audio->CatCode);
                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.audio->ChanNum               %d", gsv2k11Ports[0].content.audio->ChanNum);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.audio->ClkAccur              %d", gsv2k11Ports[0].content.audio->ClkAccur);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.audio->Consumer              %d", gsv2k11Ports[0].content.audio->Consumer);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.audio->Copyright             %d", gsv2k11Ports[0].content.audio->Copyright);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.audio->Emphasis              %d", gsv2k11Ports[0].content.audio->Emphasis);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.audio->Layout                %d", gsv2k11Ports[0].content.audio->Layout);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.audio->NValue                %d", gsv2k11Ports[0].content.audio->NValue);
                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.audio->SampFreq              %d", gsv2k11Ports[0].content.audio->SampFreq);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.audio->SrcNum                %d", gsv2k11Ports[0].content.audio->SrcNum);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[0].content.audio->WordLen               %d", gsv2k11Ports[0].content.audio->WordLen);
            }
            if(gsv2k11Ports[1].content.video!=NULL){
                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.video->info.TmdsFreq     %d", gsv2k11Ports[1].content.video->info.TmdsFreq);
                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.video->timing.FrameRate  %d", gsv2k11Ports[1].content.video->timing.FrameRate);
                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.video->timing.HActive    %d", gsv2k11Ports[1].content.video->timing.HActive);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.video->timing.HBack      %d", gsv2k11Ports[1].content.video->timing.HBack);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.video->timing.HPolarity  %d", gsv2k11Ports[1].content.video->timing.HPolarity);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.video->timing.HSync      %d", gsv2k11Ports[1].content.video->timing.HSync);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.video->timing.HTotal     %d", gsv2k11Ports[1].content.video->timing.HTotal);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.video->timing.Interlaced %d", gsv2k11Ports[1].content.video->timing.Interlaced);
                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.video->timing.VActive    %d", gsv2k11Ports[1].content.video->timing.VActive);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.video->timing.VBack      %d", gsv2k11Ports[1].content.video->timing.VBack);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.video->timing.Vic        %d", gsv2k11Ports[1].content.video->timing.Vic);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.video->timing.VPolarity  %d", gsv2k11Ports[1].content.video->timing.VPolarity);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.video->timing.VSync      %d", gsv2k11Ports[1].content.video->timing.VSync);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.video->timing.VTotal     %d", gsv2k11Ports[1].content.video->timing.VTotal);
            }
            if(gsv2k11Ports[1].content.audio!=NULL){
                
                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.audio->AudCoding             %d", gsv2k11Ports[1].content.audio->AudCoding);
                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.audio->AudFormat             %d", gsv2k11Ports[1].content.audio->AudFormat);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.audio->AudioMute             %d", gsv2k11Ports[1].content.audio->AudioMute);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.audio->AudMclkRatio          %d", gsv2k11Ports[1].content.audio->AudMclkRatio);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.audio->AudType               %d", gsv2k11Ports[1].content.audio->AudType);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.audio->AvailableAudioPackets %d", gsv2k11Ports[1].content.audio->AvailableAudioPackets);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.audio->CatCode               %d", gsv2k11Ports[1].content.audio->CatCode);
                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.audio->ChanNum               %d", gsv2k11Ports[1].content.audio->ChanNum);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.audio->ClkAccur              %d", gsv2k11Ports[1].content.audio->ClkAccur);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.audio->Consumer              %d", gsv2k11Ports[1].content.audio->Consumer);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.audio->Copyright             %d", gsv2k11Ports[1].content.audio->Copyright);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.audio->Emphasis              %d", gsv2k11Ports[1].content.audio->Emphasis);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.audio->Layout                %d", gsv2k11Ports[1].content.audio->Layout);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.audio->NValue                %d", gsv2k11Ports[1].content.audio->NValue);
                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.audio->SampFreq              %d", gsv2k11Ports[1].content.audio->SampFreq);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.audio->SrcNum                %d", gsv2k11Ports[1].content.audio->SrcNum);
//                AvUapiOutputDebugMessage(gsv_config,"gsv2k11Ports[1].content.audio->WordLen               %d", gsv2k11Ports[1].content.audio->WordLen);
//                char str[100];
//        sprintf(str, "\r\n %d -%02x  ",gsv_config->index,gsv_config->read.in.rate);
//        BSP_USART_FUN.Write(str, strlen(str));
            }
//            AvUapiOutputDebugMessage(gsv_config,"*gsv2k11Ports[5].content.is_PlugTxFsm %d %s",*gsv2k11Ports[5].content.is_PlugTxFsm,(gsv_config->read.connect)?"connect":"disconnect");
            
//    sprintf(str, "\r\n +++ gsv %d   %d  %d ",gsv_config->index,gsv2k11Ports[0].content.video->timing.HActive,gsv2k11Ports[0].content.video->timing.VActive);
//    BSP_USART_FUN.Write(str, strlen(str));
//          AvUapiOutputDebugMessage(gsv_config,"gsv_config->read.connect            %d", gsv_config->read.connect  );
        }
      
        //  向共享内存写入当前的分辨率信息
        gsv_config->read.in.width=gsv2k11Ports[0].content.video->timing.HActive*((gsv2k11Ports[0].content.video->Y==AV_Y2Y1Y0_YCBCR_420)?2:1);
        gsv_config->read.in.height=gsv2k11Ports[0].content.video->timing.VActive;
        gsv_config->read.out.width=gsv2k11Ports[1].content.video->timing.HActive*((gsv2k11Ports[1].content.video->Y==AV_Y2Y1Y0_YCBCR_420)?2:1);
        gsv_config->read.out.height=gsv2k11Ports[1].content.video->timing.VActive;
        gsv_config->read.in.rate=gsv2k11Ports[0].content.video->timing.FrameRate;
//        gsv_config->read.connect = (*gsv2k11Ports[5].content.is_PlugTxFsm == AvFsmPlugTxStable);
//        gsv_config->read.connect = gsv_config->LogicOutputSel ;
        
        gsv_config->WRITE_MEMORY(gsv_config);
        rt_thread_delay(100);
    }
	
}
