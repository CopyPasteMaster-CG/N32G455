
#ifndef __gsv2011_config_h
#define __gsv2011_config_h


//#include "hal.h"
#include "av_config.h"
//#include "av_user_config_input.h" 
typedef struct
{
  uint8 regAddr;   /* Register address */
  uint8 mask;      /* Mask for the bits in RegAddr */
  uint8 lShift;    /* Absolute left shift (in 2's complement) of */
  uint8 Reserved;
}AvI2CFieldInfo;

/* hardware access functions */
typedef AvRet (*AvFpI2cRead)(uint32, uint32, uint8 *, uint16);
typedef AvRet (*AvFpI2cWrite)(uint32, uint32, uint8 *, uint16);
typedef AvRet (*AvFpUartSendByte)(uint8 *, uint16);
typedef AvRet (*AvFpUartGetByte)(uint8 *);
typedef AvRet (*AvFpGetMilliSecond)(uint32 *);
typedef AvRet (*AvFpGetKey)(uint8 *);
typedef AvRet (*AvFpGetIrda)(uint8 *);
#ifdef COMPILER_C51_MODE
#include "bsp.h"
#define AvI2cRead         BspI2cRead
#define AvI2cWrite        BspI2cWrite
#define AvUartTxByte      BspUartSendByte
#define AvUartRxByte      BspUartGetByte
#define AvGetMilliSecond  BspGetMilliSecond
#define AvGetKey          BspGetKey
#define AvIrdaRxByte      BspIrdaGetByte
#else
#define AvI2cRead         AvHookI2cRead
#define AvI2cWrite        AvHookI2cWrite
#define AvUartTxByte      AvHookUartTxByte
#define AvUartRxByte      AvHookUartRxByte
#define AvGetMilliSecond  AvHookGetMilliSecond
#define AvGetKey          AvHookGetKey
#define AvIrdaRxByte      AvHookGetIrda
#endif
typedef void (*AvFpUartCommand) (AvPort *port);

typedef void (*AvFpKeyCommand) (AvPort *port);
//typedef void (*AvFpUartCommand) (AvPort *port);
typedef void (*AvFpIrdaCommand) (AvPort *port);


//#define AvGlobalStaticBufferSize Gsv2kResourceSize
//typedef void (*AvFpKeyCommand) (AvPort *port);
typedef struct
{
    /* The Port CP Core is connected */
    AvPort *Cp1CoreOccupied;
#if AvEdidStoredInRam
    uint8  gsv_config->InternalResource.TxBEdidRam[AvEdidMaxSize];
#endif
}   Resource;


#define Size  (540+(1*144)+(1*40)+(0*24)+(0*24)+(0*72))


typedef struct
{
    uint8  EdidCeaVicCheck [23];
    uint8  EdidCeaAudioCheck [14];
    uint8  EdidCeaAudioSfTable [14];
    uint8  EdidCeaAudioFmtTable [14];
    uint8  EdidCeaAudioByte3 [14];
    uint8  VsdbCheckList [16];
    uint8  MaxFrlRate;
    uint16 MaxTmdsClk;
    uint16 VesaMaxClk;
    uint16 MaxCharRate;
    uint16 HfVrrMax;
    uint8  HfVrrMin;
    uint8  HfDscFrlRate;
    uint8  HfDscMaxSlices;
    uint8  HfDscTotalChunkKBytes;
    uint8  VsdbHfCheck [22];
    uint8  VcdbCheck [7];
    uint8  Y420VdbCheck [4];
    uint8  HdrStCheck [4];
    uint8  CdbCheck [9];
    uint8  DolbyVisionCheck;
} AvEdidReg;


#include "APP_GSV2011.h"
#include "BSP_GSV2011.h"
#include "stdint.h"
typedef struct GSV_CONFIG_C {

    BSP_GSV2011_FUN_typestruct const * bsp;
    
	int index;
	AvFpI2cRead        AvHookI2cRead;
	AvFpI2cWrite       AvHookI2cWrite;


	AvPort *FirstPort;
	AvPort *PreviousPort;
	uint8  LogicOutputSel;

	Resource InternalResource;
	uint8 AvGlobalStaticBuffer[Size];
	uint32 currentPoint ;
    uint32 write_reg_e5;  //TX
    uint32 write_reg_e1;  //TX
    uint32 write_reg_e7; //RX
    uint32 write_reg_c8; //RX
    uint32 write_reg_e2; //RX
    uint32 write_reg_rxe5; //RX
    
    const uint8 * Gsv2k11ParRxTable;
    const uint8 * Gsv2k11ParTxTable;
    
    
	AvEdidReg DevEdidReg;
    
    
    uint8 InEdid[256];
    uint8 SinkEdid[256];
    uint8 OutEdid[256];
    
    AvFpKeyCommand AvHookKeyCmd;
    AvFpUartCommand  AvHookUartCmd;
    AvFpIrdaCommand  AvHookIrdaCmd;
    
    
    AvFpUartSendByte   AvHookUartTxByte;
    AvFpUartGetByte    AvHookUartRxByte;
    AvFpGetMilliSecond AvHookGetMilliSecond;
    AvFpGetKey         AvHookGetKey;
    AvFpGetIrda        AvHookGetIrda;
	
    
    void * mutex;
    APP_GSV2011_DATA_TYPEDEFSTRUCT read, make;
    void (*WRITE_MEMORY)(struct GSV_CONFIG_C * gsv_config);
} GSV_CONFIG;



//GSV_CONFIG *gsv_config1;

//Resource InternalResource;


////#define FindCp1Mode(port)      (GSV_CONFIG_C.InternalResource.Cp1CoreOccupied)
//static inline AvPort* FindCp1Mode(GSV_CONFIG *gsv_config,port){
//	return (gsv_config.InternalResource.Cp1CoreOccupied);
//}

//#if AvEdidStoredInRam
//#define TxBEdidRam(port)       (GSV_CONFIG_C.InternalResource0.TxBEdidRam)

//#endif


#include "uapi.h"
#endif
