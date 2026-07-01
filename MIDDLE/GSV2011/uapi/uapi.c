/**
 * @file uapi.c
 *
 * @brief low level ports related universal api functions
 */
#include <stdarg.h>
#include <stdio.h>
#include "uapi.h"
#include "gsv2011_config.h"

/* firmware static buffer */
//static uint8 AvGlobalStaticBuffer[AvGlobalStaticBufferSize];

/* hal functions variables */
//AvFpI2cRead        AvHookI2cRead;
//AvFpI2cWrite       AvHookI2cWrite;
//AvFpUartSendByte   AvHookUartTxByte;
//AvFpUartGetByte    AvHookUartRxByte;
//AvFpGetMilliSecond AvHookGetMilliSecond;
//AvFpGetKey         AvHookGetKey;
//AvFpGetIrda        AvHookGetIrda;
/**
 * @brief  univeral layer inialization function
 * @return AvOk - success
 */
uapi AvRet AvUapiInit(GSV_CONFIG *gsv_config)
{
    /* init hardware access function pointer */
    gsv_config->AvHookI2cRead = NULL;
    gsv_config->AvHookI2cWrite = NULL;
    gsv_config->AvHookUartTxByte = NULL;
	
	if(gsv_config->index == 0){ 
	
        
        gsv_config->AvHookUartRxByte = NULL;
        gsv_config->AvHookGetMilliSecond = NULL;
        gsv_config->AvHookGetKey = NULL;
        gsv_config->AvHookGetIrda = NULL;
		
		
	}
//	  AvHookI2cRead = NULL;
//    AvHookI2cWrite = NULL;
//    gsv_config->AvHookUartTxByte = NULL;
//    gsv_config->AvHookUartRxByte = NULL;
//    gsv_config->AvHookGetMilliSecond = NULL;
//    gsv_config->AvHookGetKey = NULL;
//    gsv_config->AvHookGetIrda = NULL;
    return AvOk;
}

/**
 * @brief  hookup user's hardware access functions
 * @return AvOk - success
 */
uapi AvRet AvUapiHookBspFunctions(GSV_CONFIG *gsv_config, pin AvFpI2cRead i2cRd, pin AvFpI2cWrite i2cWr, pin AvFpUartSendByte uartTxB, pin AvFpUartGetByte uartRxB, pin AvFpGetMilliSecond getMs, pin AvFpGetKey getKey,pin AvFpGetIrda getIrda)
{
	
	
    AvRet ret = AvOk;
//    AvHookI2cRead = i2cRd;
//    AvHookI2cWrite = i2cWr;
//    gsv_config->AvHookUartTxByte = uartTxB;
//    gsv_config->AvHookUartRxByte = uartRxB;
//    gsv_config->AvHookGetMilliSecond = getMs;
//    gsv_config->AvHookGetKey = getKey;
//    gsv_config->AvHookGetIrda = getIrda;
//	
    gsv_config->AvHookI2cRead = i2cRd;
    gsv_config->AvHookI2cWrite = i2cWr;
    gsv_config->AvHookUartTxByte = uartTxB;

    if(gsv_config->index == 0){ 
	
          
        gsv_config->AvHookUartRxByte = uartRxB;
        gsv_config->AvHookGetMilliSecond = getMs;
        gsv_config->AvHookGetKey = getKey;
        gsv_config->AvHookGetIrda = getIrda;
			
			
    }
//	 
	
	
//		if(gsv_config->index == 0){
    AvUapiOutputDebugMessage(gsv_config," ");
    AvUapiOutputDebugMessage(gsv_config,"-------------------------------------------------------------------");
    AvUapiOutputDebugMessage(gsv_config,"    |> Audio/Video Software %s", AvVersion);
    AvUapiOutputDebugMessage(gsv_config,"-------------------------------------------------------------------");
			
//		}
    return ret;
}

/**
 * @brief  output debug message
 * @return AvOk - success
 */
uapi AvRet AvUapiOuputDbgMsg(GSV_CONFIG *gsv_config,schar *stringToFormat, ...)
{
    va_list ap;
    uint8 formattedString[128]; /* max 128 chars in a string */
    int32 chars;
    AvRet ret = AvOk;

    va_start(ap, stringToFormat);
    chars = vsprintf((schar*)formattedString,stringToFormat, ap);
    va_end(ap);

    formattedString[chars+0] = 0x0d;
    formattedString[chars+1] = 0x0a;

    gsv_config->AvUartTxByte(formattedString, chars+2);

    /*
    if (AvUartTxByte != NULL)
    {
        for(i = 0; i < chars; i ++)
            AvUartTxByte((uint8)formattedString[i]);
    }
    AvUartTxByte(10);
    AvUartTxByte(13);
    */
    return ret;
}

/**
 * @brief  output debug message
 * @return AvOk - success
 */
uapi AvRet AvUapiUartInput(GSV_CONFIG *gsv_config,pin schar *stringToFormat, ...)
{
#if AvEnableUartInput
    va_list ap;
    uint8 formattedString[128]; /* max 128 chars in a string */
    int32 chars;
    AvRet ret = AvOk;

    va_start(ap, stringToFormat);
    chars = vsprintf((schar*)formattedString,stringToFormat, ap);
    va_end(ap);

    formattedString[chars+0] = 0x0a;
    formattedString[chars+1] = 0x0d;

    gsv_config->AvUartTxByte(formattedString, chars+2);

    /*
    if (AvUartTxByte != NULL)
    {
        for(i = 0; i < chars; i ++)
            AvUartTxByte((uint8)formattedString[i]);
    }
    AvUartTxByte(10);
    AvUartTxByte(13);
    */
    return ret;
#else
    return AvOk;
#endif
}

/**
 * @brief  allocate memory from global static buffer
 * @param  bytes memory size in bytes
 * @return AvOk - success
 */
uapi AvRet AvUapiAllocateMemory(GSV_CONFIG *gsv_config,pin uint32 bytes, pout uint32 *bufferAddress)
{
    AvRet ret = AvOk;
//    static uint32 currentPoint = 0;
    uint32 wordbytes = bytes & 0xfffffffc;
    if ((gsv_config->currentPoint + bytes) < AvGlobalStaticBufferSize)
    {
        *bufferAddress = (uint32)(&gsv_config->AvGlobalStaticBuffer[gsv_config->currentPoint]);
        if(bytes != wordbytes)
            bytes = wordbytes + 4;
        gsv_config->currentPoint = gsv_config->currentPoint + bytes;
    }
    else
    {
        AvUapiOuputDbgMsg(gsv_config,"ERROR: global static buffer exhausted.");
        ret = AvNotAvailable;
        while(1) ;
    }
    return ret;
}

uapi AvRet AvUapiRxReadInfo(pin AvPort *port)
{
    return AvOk;
}

uapi AvRet AvUapiRxReadStdi(pin AvPort *port)
{
    return AvOk;
}

uapi AvRet AvUapiTxDetectMode(pin AvPort *port)
{
    return AvOk;
}
