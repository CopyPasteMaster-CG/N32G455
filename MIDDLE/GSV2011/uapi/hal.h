/**
 * @file hal.h
 *
 * @brief hardware abstract layer header file
 */

#ifndef __hal_h
#define __hal_h

#include "av_config.h"




#include "gsv2011_config.h"


/* exported functions */
#ifdef __cplusplus
extern "C" {
#endif

	
AvRet AvHalI2cRead(GSV_CONFIG *gsv_config,pin uint32 devAddress, pin uint32 regAddress, pout uint8 *avdata, pin uint16 count);
AvRet AvHalI2cWrite(GSV_CONFIG *gsv_config,pin uint32 devAddress, pin uint32 regAddress, pin uint8 *avdata, pin uint16 count);
AvRet AvHalUartSendByte(GSV_CONFIG *gsv_config,pin uint8 *avdata, uint16 avsize);
AvRet AvHalUartGetByte(GSV_CONFIG *gsv_config,pout uint8 *avdata);
AvRet AvHalGetMilliSecond(GSV_CONFIG *gsv_config,pout uint32 *ms);
AvRet AvHalGetKey(GSV_CONFIG *gsv_config,uint8 *avdata);
AvRet AvHalGetIrda(GSV_CONFIG *gsv_config,pout uint8 *avdata);
AvRet AvHalGetElapsedMilliSecond(GSV_CONFIG *gsv_config,pin uint32 *oldTime, pout uint32 *elapsedTime);
AvRet AvHalGetTime(GSV_CONFIG *gsv_config,pout uint32 *day, pout uint32 *hour, pout uint32 *min, pout uint32 *sec);

AvRet AvHalI2cRdMultiField(GSV_CONFIG *gsv_config,pin uint32 devAddress, pin uint32 regAddress, pin uint16 number, pout uint8 *avdata);
AvRet AvHalI2cWrMultiField(GSV_CONFIG *gsv_config,pin uint32 devAddress, pin uint32 regAddress, pin uint16 number, pin uint8 *avdata);
AvRet AvHalI2cReadField8(GSV_CONFIG *gsv_config,pin uint32 devAddress, pin uint32 regAddress, pin uint8 mask, pin uint8 bitPos, pout uint8 *avdata);
AvRet AvHalI2cWriteField8(GSV_CONFIG *gsv_config,pin uint32 devAddress, pin uint32 regAddress, pin uint8 mask, pin uint8 bitPos, pin uint8 fieldVal);
AvRet AvHalI2cWriteOField8(pin uint32 devAddress, pin uint32 regAddress, pin uint8 mask, pin uint8 bitPos, pin uint8 *currVal, pin uint8 fieldVal);
AvRet AvHalI2cReadField32(GSV_CONFIG *gsv_config,pin uint32 devAddress, pin uint32 regAddress, pin uint8 msbMask, pin uint8 lsbMask, pin uint8 lsbPos, pin uint8 endian, pin uint8 fldSpan, pout uint32 *avdata);
AvRet AvHalI2cReadFlagField32(pin uint32 devAddress, pin uint32 *addrTable, pin uint8 msbMask, pin uint8 lsbMask, pin uint8 lsbPos, pin uint8 endian, pin uint8 fldSpan, pout uint32 *avdata);
AvRet AvHalI2cWriteField32(GSV_CONFIG *gsv_config,pin uint32 devAddress, pin uint32 regAddress, pin uint8 msbMask, pin uint8 lsbMask, pin uint8 lsbPos, pin uint8 endian, pin uint8 fldSpan, pin uint32 avdata);
AvRet AvHalI2cWriteFragField32(pin uint32 devAddress, pin uint32 *addrTable, pin uint8 msbMask, pin uint8 lsbMask, pin uint8 lsbPos, pin uint8 endian, pin uint8 fldSpan, pin uint32 avdata);
AvRet AvHalI2cReadRandField32(pin uint32 devAddress, pin AvI2CFieldInfo *fldInfo, pin uint8 fldSpan, pout uint32 *avdata);
AvRet AvHalI2cWriteRandField32(pin uint32 devAddress, pin AvI2CFieldInfo *fldInfo, pin uint8 fldSpan, pin uint32 avdata);

uint16 LookupValue8 (uchar *Table, uchar Value, uchar EndVal, uchar Step);
uint16 AvMemcpy(void *dst, void *src, uint32 count);
uint16 AvMemset(void *ptr, uint8 avdata, uint32 count);
uint16 AvMemcmp(const void * cs,const void * ct, unsigned int count);
uint16 AvStrcmp(const void * cs,const void * ct);
uint8 AvCheckSum(uint8 *avdata, uint16 TotalNumber, uint16 CheckSumPosition);
uint8 IsLetter(uint8 uChar);
// returns capital letter if it's a letter,
// or the number if it's a number and 0 if it's neither
bool IsBackSpace(uint8 uChar);
// returns true if it's a backspace
bool IsCR(uint8 uChar);
// returns true if it's a carriage return
bool IsSpace(uint8 uChar);
// returns true if it's a space
uint32 AsciiToNumber(uint8 *pString, uint8 Length);
void AvUselessFunction(AvPort *port, ...);

#define AvHalI2cReadMultiField(gsv_config,devAddress, regAddress, number, avdata)  \
            AvHalI2cRdMultiField(gsv_config,devAddress, (0x080000 | regAddress), number, avdata)
#define AvHalI2cWriteMultiField(gsv_config,devAddress, regAddress, number, avdata) \
            AvHalI2cWrMultiField(gsv_config,devAddress, (0x080000 | regAddress), number, avdata)
#define AvHalI2cReadField32BE(gsv_config,devAddress, regAddress, msbMask, lsbMask, lsbPos, fldSpan, avdata) \
            AvHalI2cReadField32(gsv_config,devAddress,  (0x080000 | regAddress), msbMask, lsbMask, lsbPos, AvBigEndian, fldSpan, avdata)
#define AvHalI2cReadField32LE(gsv_config,devAddress, regAddress, msbMask, lsbMask, lsbPos, fldSpan, avdata) \
            AvHalI2cReadField32(gsv_config,devAddress,  (0x080000 | regAddress), msbMask, lsbMask, lsbPos, AvLittleEndian, fldSpan, avdata)
#define AvHalI2cWriteField32BE(gsv_config,devAddress, regAddress, msbMask, lsbMask, lsbPos, fldSpan, avdata) \
            AvHalI2cWriteField32(gsv_config,devAddress, (0x080000 | regAddress), msbMask, lsbMask, lsbPos, AvBigEndian, fldSpan, avdata)
#define AvHalI2cWriteField32LE(gsv_config,devAddress, regAddress, msbMask, lsbMask, lsbPos, fldSpan, avdata) \
            AvHalI2cWriteField32(gsv_config,devAddress, (0x080000 | regAddress), msbMask, lsbMask, lsbPos, AvLittleEndian, fldSpan, avdata)

#define AvHalI2c16ReadMultiField(gsv_config,devAddress, regAddress, number, avdata) \
            AvHalI2cRdMultiField(gsv_config,devAddress, (0x100000 | regAddress), number, avdata)
#define AvHalI2c16WriteMultiField(gsv_config,devAddress, regAddress, number, avdata) \
            AvHalI2cWrMultiField(gsv_config,devAddress, (0x100000 | regAddress), number, avdata)
#define AvHalI2c16ReadField32BE(devAddress, regAddress, msbMask, lsbMask, lsbPos, fldSpan, avdata) \
            AvHalI2cReadField32(devAddress,  (0x100000 | regAddress), msbMask, lsbMask, lsbPos, AvBigEndian, fldSpan, avdata)
#define AvHalI2c16ReadField32LE(devAddress, regAddress, msbMask, lsbMask, lsbPos, fldSpan, avdata) \
            AvHalI2cReadField32(devAddress,  (0x100000 | regAddress), msbMask, lsbMask, lsbPos, AvLittleEndian, fldSpan, avdata)
#define AvHalI2c16WriteField32BE(gsv_config,devAddress, regAddress, msbMask, lsbMask, lsbPos, fldSpan, avdata) \
            AvHalI2cWriteField32(gsv_config,devAddress, (0x100000 | regAddress), msbMask, lsbMask, lsbPos, AvBigEndian, fldSpan, avdata)
#define AvHalI2c16WriteField32LE(gsv_config,devAddress, regAddress, msbMask, lsbMask, lsbPos, fldSpan, avdata) \
            AvHalI2cWriteField32(gsv_config,devAddress, (0x100000 | regAddress), msbMask, lsbMask, lsbPos, AvLittleEndian, fldSpan, avdata)

#ifdef __cplusplus
}
#endif
#endif
