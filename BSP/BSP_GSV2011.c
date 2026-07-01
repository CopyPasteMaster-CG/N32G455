
#include "BSP_GSV2011.h"

#include "BSP.h"
#include "BSP_SYSTEMCLOCK.h"
#include "BSP_USART.H"
#include "stdint.h"

static BSP_GPIO_INFO_typedefstruct const GPIO_SDA =
{
    .mode = BSP_GPIO_MODE_OUTPUT_OD,
    .option = 1,
    .num = GPIO_PIN_1,
    .port = GPIOC,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;

static BSP_GPIO_INFO_typedefstruct const GPIO_SCL =
{
    .mode = BSP_GPIO_MODE_OUTPUT_OD,
    .option = 1,
    .num = GPIO_PIN_0,
    .port = GPIOC,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;


static BSP_GPIO_INFO_typedefstruct const GPIO_RESET =
{
    .mode = BSP_GPIO_MODE_OUTPUT_OD,
    .option = 0,
    .num = GPIO_PIN_1,
    .port = GPIOB,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;

//TX1_SDA
static BSP_GPIO_INFO_typedefstruct const GPIO_SDA1 =
{
    .mode = BSP_GPIO_MODE_OUTPUT_OD,
    .option = 1,
    .num = GPIO_PIN_11,
    .port = GPIOB,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;
//TX1_SCL
static BSP_GPIO_INFO_typedefstruct const GPIO_SCL1 =
{
    .mode = BSP_GPIO_MODE_OUTPUT_OD,
    .option = 1,
    .num = GPIO_PIN_10,
    .port = GPIOB,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;

//TX2_SDA
static BSP_GPIO_INFO_typedefstruct const GPIO_SDA2 =
{
    .mode = BSP_GPIO_MODE_OUTPUT_OD,
    .option = 1,
    .num = GPIO_PIN_7,
    .port = GPIOC,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;
//TX2_SCL
static BSP_GPIO_INFO_typedefstruct const GPIO_SCL2 =
{
    .mode = BSP_GPIO_MODE_OUTPUT_OD,
    .option = 1,
    .num = GPIO_PIN_6,
    .port = GPIOC,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;
//TX3_SDA
static BSP_GPIO_INFO_typedefstruct const GPIO_SDA3 =
{
    .mode = BSP_GPIO_MODE_OUTPUT_OD,
    .option = 1,
    .num = GPIO_PIN_9,
    .port = GPIOC,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;
//TX3_SCL
static BSP_GPIO_INFO_typedefstruct const GPIO_SCL3 =
{
    .mode = BSP_GPIO_MODE_OUTPUT_OD,
    .option = 1,
    .num = GPIO_PIN_8,
    .port = GPIOC,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;
//TX4_SDA
static BSP_GPIO_INFO_typedefstruct const GPIO_SDA4 =
{
    .mode = BSP_GPIO_MODE_OUTPUT_OD,
    .option = 1,
    .num = GPIO_PIN_9,
    .port = GPIOB,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;
//TX4_SCL
static BSP_GPIO_INFO_typedefstruct const GPIO_SCL4 =
{
    .mode = BSP_GPIO_MODE_OUTPUT_OD,
    .option = 1,
    .num = GPIO_PIN_8,
    .port = GPIOB,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;

////  生成起始信号
//static inline void IIC_MAKE_START(void)
//{
//    //  首先两个端口都拉高
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//    BSP_FUN_GPIO.Set(&GPIO_SDA);
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//    BSP_FUN_GPIO.Set(&GPIO_SCL);
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//    
//    //  然后，先拉低SDA，再拉低SCL，发送起始信号
//    BSP_FUN_GPIO.Reset(&GPIO_SDA);
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//    BSP_FUN_GPIO.Reset(&GPIO_SCL);
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//}

////  生成结束信号
//static inline void IIC_MAKE_STOP(void)
//{
//    //  首先两个端口都拉低
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//    BSP_FUN_GPIO.Reset(&GPIO_SDA);
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//    BSP_FUN_GPIO.Reset(&GPIO_SCL);
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//    
//    //  然后，先拉高SCL，再拉高SDA，发送停止信号
//    BSP_FUN_GPIO.Set(&GPIO_SCL);
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//    BSP_FUN_GPIO.Set(&GPIO_SDA);
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//}

////  生成应答信号
//static inline void IIC_MAKE_ACK(void)
//{
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//    BSP_FUN_GPIO.Reset(&GPIO_SCL);
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//    BSP_FUN_GPIO.Reset(&GPIO_SDA);
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//    BSP_FUN_GPIO.Set(&GPIO_SCL);
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//    BSP_FUN_GPIO.Reset(&GPIO_SCL);
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//}

////  生成非应答信号
//static inline void IIC_MAKE_NACK(void)
//{
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//    BSP_FUN_GPIO.Reset(&GPIO_SCL);
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//    BSP_FUN_GPIO.Set(&GPIO_SDA);
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//    BSP_FUN_GPIO.Set(&GPIO_SCL);
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//    BSP_FUN_GPIO.Reset(&GPIO_SCL);
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//}

////  读取应答信号
//static inline char IIC_READ_ACK(void)
//{
//    uint8_t state;
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//    BSP_FUN_GPIO.Reset(&GPIO_SCL);
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//    BSP_FUN_GPIO.Set(&GPIO_SDA);
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//    BSP_FUN_GPIO.Set(&GPIO_SCL);
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//    state = BSP_FUN_GPIO.Read(&GPIO_SDA);
//    BSP_FUN_GPIO.Reset(&GPIO_SCL);
//    BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//    return state==0;
//}

//static inline void IIC_SEND_DATA(uint8_t da)
//{
//    for(int8_t i=7; i>=0; i--){
//        BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//        BSP_FUN_GPIO.Reset(&GPIO_SCL);
//        BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//        if(da&(0x01<<i)){
//            BSP_FUN_GPIO.Set(&GPIO_SDA);
//        }else{
//            BSP_FUN_GPIO.Reset(&GPIO_SDA);
//        }
//        BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//        BSP_FUN_GPIO.Set(&GPIO_SCL);
//        BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//    }
//}
//static inline uint8_t IIC_RECEIVE_DATA(void)
//{
//    uint8_t ref=0;
//    for(int8_t i=7; i>=0; i--){
//        BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//        BSP_FUN_GPIO.Reset(&GPIO_SCL);
//        BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//        BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//        BSP_FUN_GPIO.Set(&GPIO_SCL);
//        BSP_SYSTEMCLOCK_FUN.Delay_us(2);
//        if(BSP_FUN_GPIO.Read(&GPIO_SDA)){
//            ref|=(0x01<<i);
//        }
//    }
//    return ref;
//}

//static inline int IIC_TX_DATA(uint8_t ICaddr, uint8_t REGaddr, uint8_t const *data, uint32_t len)
//{
//    //  起始
//    IIC_MAKE_START();
//    
//    //  设备地址
//    IIC_SEND_DATA(ICaddr<<1);
//    if(IIC_READ_ACK()==0){
//        return -1;
//    }
//    
//    //  寄存器地址
//    IIC_SEND_DATA(REGaddr);
//    if(IIC_READ_ACK()==0){
//        return -2;
//    }
//    
//    //  数据
//    for(uint32_t i=0; i<len; i++){
//        IIC_SEND_DATA(data[i]);
//        if(IIC_READ_ACK()==0){
//            return -3;
//        }
//    }
//    
//    //  结束
//    IIC_MAKE_STOP();
//    
//    return 1;
//}
//static inline int IIC_RX_DATA(uint8_t ICaddr, uint8_t REGaddr, uint8_t *data, uint32_t len)
//{
//    //  起始
//    IIC_MAKE_START();
//    
//    //  设备地址
//    IIC_SEND_DATA((ICaddr<<1));
//    if(IIC_READ_ACK()==0){
//        return -1;
//    }
//    
//    //  寄存器地址
//    IIC_SEND_DATA(REGaddr);
//    if(IIC_READ_ACK()==0){
//        return -2;
//    }
//    
//    //  起始
//    IIC_MAKE_START();
//    
//    //  设备地址
//    IIC_SEND_DATA((ICaddr<<1)+1);
//    if(IIC_READ_ACK()==0){
//        return -1;
//    }
//    
//    //  数据
//    for(uint32_t i=0; i<len; i++){
//        data[i]=IIC_RECEIVE_DATA();
//        if(i>=(len-1)){
//            IIC_MAKE_NACK();
//        }else{
//            IIC_MAKE_ACK();
//        }
//    }
//    
//    //  结束
//    IIC_MAKE_STOP();
//    
//    return 1;
//}
////static inline int IIC_TX_DATA_10bit(uint16_t ICaddr, uint8_t REGaddr, uint8_t const *data, uint32_t len)
////{
////    //  起始
////    IIC_MAKE_START();
////    
////    //  设备地址，高两位
////    IIC_SEND_DATA((((ICaddr>>8)&0x03)<<1)|0xF0);
////    if(IIC_READ_ACK()==0){
//////        IIC_MAKE_STOP();
////        return -1;
////    }
////    
////    //  设备地址，低8位
////    IIC_SEND_DATA(ICaddr&0xFF);
////    if(IIC_READ_ACK()==0){
//////        IIC_MAKE_STOP();
////        return -1;
////    }
////    
////    //  寄存器地址
////    IIC_SEND_DATA(REGaddr);
////    if(IIC_READ_ACK()==0){
//////        IIC_MAKE_STOP();
////        return -2;
////    }
////    
////    //  数据
////    for(uint32_t i=0; i<len; i++){
////        IIC_SEND_DATA(data[i]);
////        if(IIC_READ_ACK()==0){
//////            IIC_MAKE_STOP();
////            return -3;
////        }
////    }
////    
////    //  结束
////    IIC_MAKE_STOP();
////    
////    return 1;
////}
////static inline int IIC_RX_DATA_10bit(uint16_t ICaddr, uint8_t REGaddr, uint8_t *data, uint32_t len)
////{
////    //  起始
////    IIC_MAKE_START();
////    
////    //  设备地址，高两位
////    IIC_SEND_DATA((((ICaddr>>8)&0x03)<<1)|0xF0);
////    if(IIC_READ_ACK()==0){
//////        IIC_MAKE_STOP();
////        return -1;
////    }
////    
////    //  设备地址，低8位
////    IIC_SEND_DATA(ICaddr&0xFF);
////    if(IIC_READ_ACK()==0){
//////        IIC_MAKE_STOP();
////        return -1;
////    }
////    
////    //  寄存器地址
////    IIC_SEND_DATA(REGaddr);
////    if(IIC_READ_ACK()==0){
//////        IIC_MAKE_STOP();
////        return -2;
////    }
////    
////    //  起始
////    IIC_MAKE_START();
////    
////    //  设备地址，高两位
////    IIC_SEND_DATA(((((ICaddr>>8)&0x03)<<1)|0xF0)+1);
////    if(IIC_READ_ACK()==0){
//////        IIC_MAKE_STOP();
////        return -1;
////    }
////    
////    //  数据
////    for(uint32_t i=0; i<len; i++){
////        data[i]=IIC_RECEIVE_DATA();
////        if(i>=(len-1)){
////            IIC_MAKE_NACK();
////        }else{
////            IIC_MAKE_ACK();
////        }
////    }
////    
////    //  结束
////    IIC_MAKE_STOP();
////    
////    return 1;
////}


//static inline int IIC_TX_DATA_16bitreg(uint8_t ICaddr, uint16_t REGaddr, uint8_t const *data, uint32_t len)
//{
//    //  起始
//    IIC_MAKE_START();
//    
//    //  设备地址
//    IIC_SEND_DATA((ICaddr<<1)+0);
//    if(IIC_READ_ACK()==0){
//        return -1;
//    }
//    
//    //  寄存器地址
//    IIC_SEND_DATA(REGaddr>>8);
//    if(IIC_READ_ACK()==0){
//        return -2;
//    }
//    IIC_SEND_DATA(REGaddr);
//    if(IIC_READ_ACK()==0){
//        return -2;
//    }
//    
//    //  数据
//    for(uint32_t i=0; i<len; i++){
//        IIC_SEND_DATA(data[i]);
//        if(IIC_READ_ACK()==0){
//            return -3;
//        }
//    }
//    
//    //  结束
//    IIC_MAKE_STOP();
//    
//    return 1;
//}
//static inline int IIC_RX_DATA_16bitreg(uint8_t ICaddr, uint16_t REGaddr, uint8_t *data, uint32_t len)
//{
//    //  起始
//    IIC_MAKE_START();
//    
//    //  设备地址
//    IIC_SEND_DATA((ICaddr<<1)+0);
//    if(IIC_READ_ACK()==0){
//        return -1;
//    }
//    
//    //  寄存器地址
//    IIC_SEND_DATA(REGaddr>>8);
//    if(IIC_READ_ACK()==0){
//        return -2;
//    }
//    IIC_SEND_DATA(REGaddr);
//    if(IIC_READ_ACK()==0){
//        return -2;
//    }
//    
//    //  起始
//    IIC_MAKE_START();
//    
//    //  设备地址
//    IIC_SEND_DATA((ICaddr<<1)+1);
//    if(IIC_READ_ACK()==0){
//        return -1;
//    }
//    
//    //  数据
//    for(uint32_t i=0; i<len; i++){
//        data[i]=IIC_RECEIVE_DATA();
//        if(i>=(len-1)){
//            IIC_MAKE_NACK();
//        }else{
//            IIC_MAKE_ACK();
//        }
//    }
//    
//    //  结束
//    IIC_MAKE_STOP();
//    
//    return 1;
//}







static AvRet FUN_Init(void)
{    
    BSP_FUN_GPIO.Init(&GPIO_SDA);
    BSP_FUN_GPIO.Init(&GPIO_SCL);
    //BSP_FUN_GPIO.Init(&GPIO_RESET);	
//    BSP_USART_FUN.Init(115200);
    BSP_GSV2011_FUN0.RST_Enable();

	
    return AvOk;
}


static AvRet FUN_Init1(void)
{
			
		BSP_FUN_GPIO.Init(&GPIO_SDA1);
    BSP_FUN_GPIO.Init(&GPIO_SCL1);
//		BSP_FUN_GPIO.Init(&GPIO_RESET);

//    BSP_USART_FUN.Init(115200);
//    BSP_GSV2011_FUN1.RST_Enable();
    return AvOk;
}


static AvRet FUN_Init2(void)
{
			
	BSP_FUN_GPIO.Init(&GPIO_SDA2);
    BSP_FUN_GPIO.Init(&GPIO_SCL2);
		//BSP_FUN_GPIO.Init(&GPIO_RESET);

//    BSP_USART_FUN.Init(115200);
//    BSP_GSV2011_FUN1.RST_Enable();
    return AvOk;
}
static AvRet FUN_Init3(void)
{
			
	BSP_FUN_GPIO.Init(&GPIO_SDA3);
    BSP_FUN_GPIO.Init(&GPIO_SCL3);
		//BSP_FUN_GPIO.Init(&GPIO_RESET);

//    BSP_USART_FUN.Init(115200);
//    BSP_GSV2011_FUN1.RST_Enable();
    return AvOk;
}
static AvRet FUN_Init4(void)
{
	BSP_FUN_GPIO.Init(&GPIO_SDA4);
    BSP_FUN_GPIO.Init(&GPIO_SCL4);
		//BSP_FUN_GPIO.Init(&GPIO_RESET);

//    BSP_USART_FUN.Init(115200);
//    BSP_GSV2011_FUN1.RST_Enable();
    return AvOk;
}

/* Includes ------------------------------------------------------------------*/
#define MANI2CFREQ     50




void I2CDelay(uint16 delay)
{
    uint16 j = 0;
    for(j=0;j<delay;j++);
    return;
}

/*
PB0-1: index 0 I2C 0:SDA, 1:SCL
PB2-3: index 1 I2C 2:SDA, 3:SCL
PB4-5: index 2 I2C 4:SDA, 5:SCL
*/

void ManReleaseScl(uint8 index)
{
//    switch(index)
//    {
//    case 0:
//        SCL0_IN;
//        break;
//    case 1:
//        SCL1_IN;
//        break;
//    case 2:
//        SCL2_IN;
//        break;
//    }
BSP_FUN_GPIO.Set(&GPIO_SCL);
}

void ManDriveScl(uint8 index)
{
//    switch(index)
//    {
//    case 0:
//        SCL0_OUT;
//        break;
//    case 1:
//        SCL1_OUT;
//        break;
//    case 2:
//        SCL2_OUT;
//        break;
//    }

}

void ManReleaseSda(uint8 index)
{
//    switch(index)
//    {
//    case 0:
//        SDA0_IN;
//        break;
//    case 1:
//        SDA1_IN;
//        break;
//    case 2:
//        SDA2_IN;
//        break;
//    }
BSP_FUN_GPIO.Set(&GPIO_SDA);
}


void ManDriveSda(uint8 index)
{
//    switch(index)
//    {
//    case 0:
//        SDA0_OUT;
//        break;
//    case 1:
//        SDA1_OUT;
//        break;
//    case 2:
//        SDA2_OUT;
//        break;
//    }
    
}

void ManSclHigh(uint8 index)
{
//    switch(index)
//    {
//    case 0:
//        SET_SCL0_1;
//        break;
//    case 1:
//        SET_SCL1_1;
//        break;
//    case 2:
//        SET_SCL2_1;
//        break;
//    }
    
    BSP_FUN_GPIO.Set(&GPIO_SCL);
}

void ManSclLow(uint8 index)
{
//    switch(index)
//    {
//    case 0:
//        SET_SCL0_0;
//        break;
//    case 1:
//        SET_SCL1_0;
//        break;
//    case 2:
//        SET_SCL2_0;
//        break;
//    }
    BSP_FUN_GPIO.Reset(&GPIO_SCL);
}

void ManSdaHigh(uint8 index)
{
//    switch(index)
//    {
//    case 0:
//        SET_SDA0_1;
//        break;
//    case 1:
//        SET_SDA1_1;
//        break;
//    case 2:
//        SET_SDA2_1;
//        break;
//    }
    
    BSP_FUN_GPIO.Set(&GPIO_SDA);
}

void ManSdaLow(uint8 index)
{
//    switch(index)
//    {
//    case 0:
//        SET_SDA0_0;
//        break;
//    case 1:
//        SET_SDA1_0;
//        break;
//    case 2:
//        SET_SDA2_0;
//        break;
//    }
    
    BSP_FUN_GPIO.Reset(&GPIO_SDA);
}

uint8 GetSdaValue(uint8 index)
{
//    uint8 value = 0;
//    switch(index)
//    {
//    case 0:
//        value = GET_SDA0;
//        break;
//    case 1:
//        value = GET_SDA1;
//        break;
//    case 2:
//        value = GET_SDA2;
//        break;
//    }
//    return value;
    return BSP_FUN_GPIO.Read(&GPIO_SDA);
}
//////////////////////////////
//TX1 
void ManReleaseScl_1(uint8 index)
{

BSP_FUN_GPIO.Set(&GPIO_SCL1);
}

void ManReleaseScl_X(uint8 index)
{
    switch(index)
    {
    case 0:
				BSP_FUN_GPIO.Set(&GPIO_SCL);
        break;
    case 1:
				BSP_FUN_GPIO.Set(&GPIO_SCL1);
        break;
    case 2:
				BSP_FUN_GPIO.Set(&GPIO_SCL2);
        break;
		case 3:
				BSP_FUN_GPIO.Set(&GPIO_SCL3);
        break;
		case 4:
				BSP_FUN_GPIO.Set(&GPIO_SCL4);
        break;
    }

}


void ManDriveScl_1(uint8 index)
{

}
void ManDriveScl_X(uint8 index)
{

}

void ManReleaseSda_1(uint8 index)
{

BSP_FUN_GPIO.Set(&GPIO_SDA1);
}

void ManReleaseSda_X(uint8 index)
{
    switch(index)
    {
    case 0:
        BSP_FUN_GPIO.Set(&GPIO_SDA);
        break;
    case 1:
        BSP_FUN_GPIO.Set(&GPIO_SDA1);
        break;
    case 2:
        BSP_FUN_GPIO.Set(&GPIO_SDA2);
        break;
		case 3:
        BSP_FUN_GPIO.Set(&GPIO_SDA3);
        break;
		case 4:
        BSP_FUN_GPIO.Set(&GPIO_SDA4);
        break;
    }

}

void ManDriveSda_1(uint8 index)
{

    
}
void ManDriveSda_X(uint8 index)
{

    
}
void ManScl_1High(uint8 index)
{

    
    BSP_FUN_GPIO.Set(&GPIO_SCL1);
}
void ManScl_XHigh(uint8 index)
{

    
	switch(index)
    {
    case 0:
        BSP_FUN_GPIO.Set(&GPIO_SCL);
        break;
    case 1:
        BSP_FUN_GPIO.Set(&GPIO_SCL1);
        break;
    case 2:
        BSP_FUN_GPIO.Set(&GPIO_SCL2);
        break;
		case 3:
        BSP_FUN_GPIO.Set(&GPIO_SCL3);
        break;
		case 4:
        BSP_FUN_GPIO.Set(&GPIO_SCL4);
        break;
    }
	
}
void ManScl_1Low(uint8 index)
{

    BSP_FUN_GPIO.Reset(&GPIO_SCL1);
}

void ManScl_XLow(uint8 index)
{
switch(index)
    {
    case 0:
        BSP_FUN_GPIO.Reset(&GPIO_SCL);
        break;
    case 1:
        BSP_FUN_GPIO.Reset(&GPIO_SCL1);
        break;
    case 2:
        BSP_FUN_GPIO.Reset(&GPIO_SCL2);
        break;
		case 3:
        BSP_FUN_GPIO.Reset(&GPIO_SCL3);
        break;
		case 4:
        BSP_FUN_GPIO.Reset(&GPIO_SCL4);
        break;
    }
	}

void ManSda_1High(uint8 index)
{

    
    BSP_FUN_GPIO.Set(&GPIO_SDA1);
}
void ManSda_XHigh(uint8 index)
{

    switch(index)
    {
    case 0:
        BSP_FUN_GPIO.Set(&GPIO_SDA);
        break;
    case 1:
        BSP_FUN_GPIO.Set(&GPIO_SDA1);
        break;
    case 2:
        BSP_FUN_GPIO.Set(&GPIO_SDA2);
        break;
		case 3:
        BSP_FUN_GPIO.Set(&GPIO_SDA3);
        break;
		case 4:
        BSP_FUN_GPIO.Set(&GPIO_SDA4);
        break;
    }
//    BSP_FUN_GPIO.Set(&SDA);
}
void ManSda_1Low(uint8 index)
{

    
    BSP_FUN_GPIO.Reset(&GPIO_SDA1);
}
void ManSda_XLow(uint8 index)
{
switch(index)
    {
    case 0:
        BSP_FUN_GPIO.Reset(&GPIO_SDA);
        break;
    case 1:
        BSP_FUN_GPIO.Reset(&GPIO_SDA1);
        break;
    case 2:
        BSP_FUN_GPIO.Reset(&GPIO_SDA2);
        break;
		case 3:
        BSP_FUN_GPIO.Reset(&GPIO_SDA3);
        break;
		case 4:
        BSP_FUN_GPIO.Reset(&GPIO_SDA4);
        break;
    }
    

}
uint8 GetSda_1Value(uint8 index)
{

    return BSP_FUN_GPIO.Read(&GPIO_SDA1);
}

uint8 GetSda_XValue(uint8 index)
{

	switch(index)
  {
    case 0:
       return BSP_FUN_GPIO.Read(&GPIO_SDA);
				//break;
    case 1:
        return BSP_FUN_GPIO.Read(&GPIO_SDA1);
        //break;
    case 2:
        return BSP_FUN_GPIO.Read(&GPIO_SDA2);
        //break;
		case 3:
        return BSP_FUN_GPIO.Read(&GPIO_SDA3);
        //break;
		case 4:
        return BSP_FUN_GPIO.Read(&GPIO_SDA4);
        //break;
    }
  return 0;
}

uint8 ManGetAck(uint8 index)
{
    uint8 value = 0;

    I2CDelay(MANI2CFREQ);
    ManSclLow(index);
    I2CDelay(MANI2CFREQ);
    ManReleaseSda(index);
    I2CDelay(MANI2CFREQ);
    ManSclHigh(index);
    I2CDelay(MANI2CFREQ);
    value = GetSdaValue(index);
    I2CDelay(MANI2CFREQ);
    ManSclLow(index);
    if(value == 0)
        ManSdaLow(index);
    else
        ManSdaHigh(index);
    I2CDelay(MANI2CFREQ);
    ManDriveSda(index);
    I2CDelay(MANI2CFREQ);
    return value;
}

void ManSetAck(uint8 index, uint8 i2cdata)
{
    I2CDelay(MANI2CFREQ);
    ManSclLow(index);
    I2CDelay(MANI2CFREQ);
    if(i2cdata != 0)
        ManSdaHigh(index);
    else
        ManSdaLow(index);
    I2CDelay(MANI2CFREQ);
    ManSclHigh(index);
    I2CDelay(2*MANI2CFREQ);
    ManSclLow(index);
    I2CDelay(MANI2CFREQ);
    ManSdaLow(index);
    I2CDelay(MANI2CFREQ);
}

void ManI2cStart(uint8 index)
{
    ManDriveScl(index);
    ManDriveSda(index);
    I2CDelay(MANI2CFREQ);
    ManSclHigh(index);
    I2CDelay(MANI2CFREQ);
    ManSdaHigh(index);
    I2CDelay(MANI2CFREQ);
    ManSdaLow(index);
    I2CDelay(MANI2CFREQ);
    ManSclLow(index);
    I2CDelay(MANI2CFREQ);

}

void ManI2cStop(uint8 index)
{
    I2CDelay(MANI2CFREQ);
    ManSclHigh(index);
    I2CDelay(MANI2CFREQ);
    ManSdaHigh(index);
    I2CDelay(MANI2CFREQ);
    ManReleaseScl(index);
    ManReleaseSda(index);

}

void ManI2cRestart(uint8 index)
{
    I2CDelay(MANI2CFREQ);
    ManSdaHigh(index);
    I2CDelay(MANI2CFREQ);
    ManSclHigh(index);
    I2CDelay(MANI2CFREQ);
    ManSdaLow(index);
    I2CDelay(MANI2CFREQ);
    ManSclLow(index);
    I2CDelay(MANI2CFREQ);

}

//////////TX1
uint8 ManGetAck_1(uint8 index)
{
    uint8 value = 0;

    I2CDelay(MANI2CFREQ);
    ManScl_1Low(index);
    I2CDelay(MANI2CFREQ);
    ManReleaseSda_1(index);
    I2CDelay(MANI2CFREQ);
    ManScl_1High(index);
    I2CDelay(MANI2CFREQ);
    value = GetSda_1Value(index);
    I2CDelay(MANI2CFREQ);
    ManScl_1Low(index);
    if(value == 0)
        ManSda_1Low(index);
    else
        ManSda_1High(index);
    I2CDelay(MANI2CFREQ);
    ManDriveSda_1(index);
    I2CDelay(MANI2CFREQ);
    return value;
}

void ManSetAck_1(uint8 index, uint8 i2cdata)
{
    I2CDelay(MANI2CFREQ);
    ManScl_1Low(index);
    I2CDelay(MANI2CFREQ);
    if(i2cdata != 0)
        ManSda_1High(index);
    else
        ManSda_1Low(index);
    I2CDelay(MANI2CFREQ);
    ManScl_1High(index);
    I2CDelay(2*MANI2CFREQ);
    ManScl_1Low(index);
    I2CDelay(MANI2CFREQ);
    ManSda_1Low(index);
    I2CDelay(MANI2CFREQ);
}

void ManI2c_1Start(uint8 index)
{
    ManDriveScl_1(index);
    ManDriveSda_1(index);
    I2CDelay(MANI2CFREQ);
    ManScl_1High(index);
    I2CDelay(MANI2CFREQ);
    ManSda_1High(index);
    I2CDelay(MANI2CFREQ);
    ManSda_1Low(index);
    I2CDelay(MANI2CFREQ);
    ManScl_1Low(index);
    I2CDelay(MANI2CFREQ);

}

void ManI2cStop_1(uint8 index)
{
    I2CDelay(MANI2CFREQ);
    ManScl_1High(index);
    I2CDelay(MANI2CFREQ);
    ManSda_1High(index);
    I2CDelay(MANI2CFREQ);
    ManReleaseScl_1(index);
    ManReleaseSda_1(index);

}

void ManI2cRestart_1(uint8 index)
{
    I2CDelay(MANI2CFREQ);
    ManSda_1High(index);
    I2CDelay(MANI2CFREQ);
    ManScl_1High(index);
    I2CDelay(MANI2CFREQ);
    ManSda_1Low(index);
    I2CDelay(MANI2CFREQ);
    ManScl_1Low(index);
    I2CDelay(MANI2CFREQ);

}
///////////////////

uint8 ManGetAck_X(uint8 index)
{
    uint8 value = 0;

    I2CDelay(MANI2CFREQ);
    ManScl_XLow(index);
    I2CDelay(MANI2CFREQ);
    ManReleaseSda_X(index);
    I2CDelay(MANI2CFREQ);
    ManScl_XHigh(index);
    I2CDelay(MANI2CFREQ);
    value = GetSda_XValue(index);
    I2CDelay(MANI2CFREQ);
    ManScl_XLow(index);
    if(value == 0)
        ManSda_XLow(index);
    else
        ManSda_XHigh(index);
    I2CDelay(MANI2CFREQ);
    ManDriveSda_X(index);
    I2CDelay(MANI2CFREQ);


		
    return value;
}

void ManSetAck_X(uint8 index, uint8 i2cdata)
{

    I2CDelay(MANI2CFREQ);
    ManScl_XLow(index);
    I2CDelay(MANI2CFREQ);
    if(i2cdata != 0)
        ManSda_XHigh(index);
    else
        ManSda_XLow(index);
    I2CDelay(MANI2CFREQ);
    ManScl_XHigh(index);
    I2CDelay(2*MANI2CFREQ);
    ManScl_XLow(index);
    I2CDelay(MANI2CFREQ);
    ManSda_XLow(index);
    I2CDelay(MANI2CFREQ);
	
}

void ManI2c_XStart(uint8 index)
{
    ManDriveScl_X(index);
    ManDriveSda_X(index);
    I2CDelay(MANI2CFREQ);
    ManScl_XHigh(index);
    I2CDelay(MANI2CFREQ);
    ManSda_XHigh(index);
    I2CDelay(MANI2CFREQ);
    ManSda_XLow(index);
    I2CDelay(MANI2CFREQ);
    ManScl_XLow(index);
    I2CDelay(MANI2CFREQ);

}

void ManI2cStop_X(uint8 index)
{
    I2CDelay(MANI2CFREQ);
    ManScl_XHigh(index);
    I2CDelay(MANI2CFREQ);
    ManSda_XHigh(index);
    I2CDelay(MANI2CFREQ);
    ManReleaseScl_X(index);
    ManReleaseSda_X(index);

}

void ManI2cRestart_X(uint8 index)
{
    I2CDelay(MANI2CFREQ);
    ManSda_XHigh(index);
    I2CDelay(MANI2CFREQ);
    ManScl_XHigh(index);
    I2CDelay(MANI2CFREQ);
    ManSda_XLow(index);
    I2CDelay(MANI2CFREQ);
    ManScl_XLow(index);
    I2CDelay(MANI2CFREQ);

}

///////////////////
/////////////////

uint8 ManI2cSendByte(uint8 index, uint8 i2cdata)
{
    uint8  i;
    uint32 value;
    uint8  ack;

    /* Send byte */
    for(i=0;i<8;i++)
    {
        I2CDelay(MANI2CFREQ);
        ManSclLow(index);
        I2CDelay(MANI2CFREQ);
        value = (i2cdata>>(7-i))&0x01;
        if(value == 0)
            ManSdaLow(index);
        else
            ManSdaHigh(index);
        I2CDelay(MANI2CFREQ);
        ManSclHigh(index);
        I2CDelay(MANI2CFREQ);
    }
    I2CDelay(MANI2CFREQ);
    ManSclLow(index);
    I2CDelay(MANI2CFREQ);
    /* Release Sda to get ACK */
    ManReleaseSda(index);
    ack = ManGetAck(index);
    ManDriveSda(index);
    return ack;
}

uint8 ManI2cReadByte(uint8 index, uint8 FinalByte)
{
    uint8  i;
    uint32 value = 0;
    uint8  i2cdata = 0;

    ManReleaseSda(index);
    for(i=0;i<8;i++)
    {
        ManSclLow(index);
        I2CDelay(2*MANI2CFREQ);
        ManSclHigh(index);
        I2CDelay(MANI2CFREQ);
        value = GetSdaValue(index);
        if(value != 0)
            i2cdata = (i2cdata<<1)+1;
        else
            i2cdata = i2cdata<<1;
        I2CDelay(MANI2CFREQ);
    }
    ManSclLow(index);
    I2CDelay(MANI2CFREQ);
    ManDriveSda(index);
    I2CDelay(MANI2CFREQ);

    if(FinalByte == 0x0)
        ManSetAck(index,0);
    else
        ManSetAck(index,1);
    return i2cdata;
}



static inline AvRet ManI2cWrite(uint32 devAddress, uint32 regAddress, uint8 *i2cdata, uint16 count, uint8 index, uint8 Flag16bit)
{
    uint8  value;
    uint8  ack = 1;
    uint16 i;
    ManI2cStart(index);
    value = devAddress & 0xfe;
    ack = ManI2cSendByte(index, value);
    if(Flag16bit == 1)
    {
        value = (regAddress>>8) & 0xff;
        ManI2cSendByte(index, value);
    }
    value = regAddress & 0xff;
    ManI2cSendByte(index, value);
    for(i=0;i<count;i++)
    {
        if(i==count-1)
            ManI2cSendByte(index, i2cdata[i]);
        else
            ManI2cSendByte(index, i2cdata[i]);
    }
    ManI2cStop(index);

    I2CDelay(0x3f);

    if(ack == 0)
        return AvOk;
    else
        return AvError;
}
static inline AvRet ManI2cRead(uint32 devAddress, uint32 regAddress, uint8 *i2cdata, uint16 count, uint8 index, uint8 Flag16bit)
{
    uint8  value;
    uint8  ack = 1;
    uint16 i;
    ManI2cStart(index);
    value = devAddress & 0xfe;
    ack = ManI2cSendByte(index,value);
    if(Flag16bit == 1)
    {
        value = (regAddress>>8) & 0xff;
        ManI2cSendByte(index, value);
    }
    value = regAddress & 0xff;
    ManI2cSendByte(index,value);

    ManI2cRestart(index);

    value = (devAddress & 0xff) | 0x01;
    ManI2cSendByte(index,value);
    for(i=0;i<count;i++)
    {
        if(i==count-1)
            i2cdata[i] = ManI2cReadByte(index, 1);
        else
            i2cdata[i] = ManI2cReadByte(index, 0);
    }
    ManI2cStop(index);

    I2CDelay(0x3f);

    if(ack == 0)
        return AvOk;
    else
        return AvError;
}
//////////////TX1


uint8 ManI2c_1SendByte(uint8 index, uint8 i2cdata)
{
    uint8  i;
    uint32 value;
    uint8  ack;

    /* Send byte */
    for(i=0;i<8;i++)
    {
        I2CDelay(MANI2CFREQ);
        ManScl_1Low(index);
        I2CDelay(MANI2CFREQ);
        value = (i2cdata>>(7-i))&0x01;
        if(value == 0)
            ManSda_1Low(index);
        else
            ManSda_1High(index);
        I2CDelay(MANI2CFREQ);
        ManScl_1High(index);
        I2CDelay(MANI2CFREQ);
    }
    I2CDelay(MANI2CFREQ);
    ManScl_1Low(index);
    I2CDelay(MANI2CFREQ);
    /* Release Sda to get ACK */
    ManReleaseSda_1(index);
    ack = ManGetAck_1(index);
    ManDriveSda_1(index);
    return ack;
}

uint8 ManI2cReadByte_1(uint8 index, uint8 FinalByte)
{
    uint8  i;
    uint32 value = 0;
    uint8  i2cdata = 0;

    ManReleaseSda_1(index);
    for(i=0;i<8;i++)
    {
        ManScl_1Low(index);
        I2CDelay(2*MANI2CFREQ);
        ManScl_1High(index);
        I2CDelay(MANI2CFREQ);
        value = GetSda_1Value(index);
        if(value != 0)
            i2cdata = (i2cdata<<1)+1;
        else
            i2cdata = i2cdata<<1;
        I2CDelay(MANI2CFREQ);
    }
    ManScl_1Low(index);
    I2CDelay(MANI2CFREQ);
    ManDriveSda_1(index);
    I2CDelay(MANI2CFREQ);

    if(FinalByte == 0x0)
        ManSetAck_1(index,0);
    else
        ManSetAck_1(index,1);
    return i2cdata;
}



static inline AvRet ManI2cWrite_1(uint32 devAddress, uint32 regAddress, uint8 *i2cdata, uint16 count, uint8 index, uint8 Flag16bit)
{
    uint8  value;
    uint8  ack = 1;
    uint16 i;
    ManI2c_1Start(index);
    value = devAddress & 0xfe;
    ack = ManI2c_1SendByte(index, value);
    if(Flag16bit == 1)
    {
        value = (regAddress>>8) & 0xff;
        ManI2c_1SendByte(index, value);
    }
    value = regAddress & 0xff;
    ManI2c_1SendByte(index, value);
    for(i=0;i<count;i++)
    {
        if(i==count-1)
            ManI2c_1SendByte(index, i2cdata[i]);
        else
            ManI2c_1SendByte(index, i2cdata[i]);
    }
    ManI2cStop_1(index);

    I2CDelay(0x3f);

    if(ack == 0)
        return AvOk;
    else
        return AvError;
}
static inline AvRet ManI2cRead_1(uint32 devAddress, uint32 regAddress, uint8 *i2cdata, uint16 count, uint8 index, uint8 Flag16bit)
{
    uint8  value;
    uint8  ack = 1;
    uint16 i;
    ManI2c_1Start(index);
    value = devAddress & 0xfe;
    ack = ManI2c_1SendByte(index,value);
    if(Flag16bit == 1)
    {
        value = (regAddress>>8) & 0xff;
        ManI2c_1SendByte(index, value);
    }
    value = regAddress & 0xff;
    ManI2c_1SendByte(index,value);

    ManI2cRestart_1(index);

    value = (devAddress & 0xff) | 0x01;
    ManI2c_1SendByte(index,value);
    for(i=0;i<count;i++)
    {
        if(i==count-1)
            i2cdata[i] = ManI2cReadByte_1(index, 1);
        else
            i2cdata[i] = ManI2cReadByte_1(index, 0);
    }
    ManI2cStop_1(index);

    I2CDelay(0x3f);

    if(ack == 0)
        return AvOk;
    else
        return AvError;
}

////////X

uint8 ManI2c_XSendByte(uint8 index, uint8 i2cdata)
{
    uint8  i;
    uint32 value;
    uint8  ack;

    /* Send byte */
    for(i=0;i<8;i++)
    {
        I2CDelay(MANI2CFREQ);
        ManScl_XLow(index);
        I2CDelay(MANI2CFREQ);
        value = (i2cdata>>(7-i))&0x01;
        if(value == 0)
            ManSda_XLow(index);
        else
            ManSda_XHigh(index);
        I2CDelay(MANI2CFREQ);
        ManScl_XHigh(index);
        I2CDelay(MANI2CFREQ);
    }
    I2CDelay(MANI2CFREQ);
    ManScl_XLow(index);
    I2CDelay(MANI2CFREQ);
    /* Release Sda to get ACK */
    ManReleaseSda_X(index);
    ack = ManGetAck_X(index);
    ManDriveSda_X(index);
    return ack;
}

uint8 ManI2cReadByte_X(uint8 index, uint8 FinalByte)
{
    uint8  i;
    uint32 value = 0;
    uint8  i2cdata = 0;

    ManReleaseSda_X(index);
    for(i=0;i<8;i++)
    {
        ManScl_XLow(index);
        I2CDelay(2*MANI2CFREQ);
        ManScl_XHigh(index);
        I2CDelay(MANI2CFREQ);
        value = GetSda_XValue(index);
        if(value != 0)
            i2cdata = (i2cdata<<1)+1;
        else
            i2cdata = i2cdata<<1;
        I2CDelay(MANI2CFREQ);
    }
    ManScl_XLow(index);
    I2CDelay(MANI2CFREQ);
    ManDriveSda_X(index);
    I2CDelay(MANI2CFREQ);

    if(FinalByte == 0x0)
        ManSetAck_X(index,0);
    else
        ManSetAck_X(index,1);
    return i2cdata;
}



static inline AvRet ManI2cWrite_X(uint32 devAddress, uint32 regAddress, uint8 *i2cdata, uint16 count, uint8 index, uint8 Flag16bit)
{
    uint8  value;
    uint8  ack = 1;
    uint16 i;
    ManI2c_XStart(index);
    value = devAddress & 0xfe;
    ack = ManI2c_XSendByte(index, value);
    if(Flag16bit == 1)
    {
        value = (regAddress>>8) & 0xff;
        ManI2c_XSendByte(index, value);
    }
    value = regAddress & 0xff;
    ManI2c_XSendByte(index, value);
    for(i=0;i<count;i++)
    {
        if(i==count-1)
            ManI2c_XSendByte(index, i2cdata[i]);
        else
            ManI2c_XSendByte(index, i2cdata[i]);
    }
    ManI2cStop_X(index);

    I2CDelay(0x3f);

    if(ack == 0)
        return AvOk;
    else
        return AvError;
}
static inline AvRet ManI2cRead_X(uint32 devAddress, uint32 regAddress, uint8 *i2cdata, uint16 count, uint8 index, uint8 Flag16bit)
{
    uint8  value;
    uint8  ack = 1;
    uint16 i;
    ManI2c_XStart(index);
    value = devAddress & 0xfe;
    ack = ManI2c_XSendByte(index,value);
    if(Flag16bit == 1)
    {
        value = (regAddress>>8) & 0xff;
        ManI2c_XSendByte(index, value);
    }
    value = regAddress & 0xff;
    ManI2c_XSendByte(index,value);

    ManI2cRestart_X(index);

    value = (devAddress & 0xff) | 0x01;
    ManI2c_XSendByte(index,value);
    for(i=0;i<count;i++)
    {
        if(i==count-1)
            i2cdata[i] = ManI2cReadByte_X(index, 1);
        else
            i2cdata[i] = ManI2cReadByte_X(index, 0);
    }
    ManI2cStop_X(index);

    I2CDelay(0x3f);

    if(ack == 0)
        return AvOk;
    else
        return AvError;
}

////////


static AvRet FUN_I2cRead(uint32 devAddress, uint32 regAddress, uint8 *data, uint16 count)
{
    AvRet ret = AvOk;
    uint8 deviceAddress = (uint8)AvGetI2cDeviceAddress(devAddress);
    uint8 busAddress = (uint8)AvGetI2cBusAddress(devAddress);
    uint8 regAddressWidth = (uint8)AvGetRegAddressWidth(devAddress);
    regAddressWidth = (regAddressWidth == 0) ? 0 : 1;
    uint16 regAdress = (uint32)((AvGetRegAddress(devAddress)<<8) | AvGetRegAddress(regAddress));
    ManI2cRead(deviceAddress, regAdress, data, count, busAddress, regAddressWidth);
    return ret;
////////    volatile uint8 deviceAddress = (((uint8)AvGetI2cDeviceAddress(devAddress))>>1);
////////    volatile uint8 regAddressWidth = (uint8)AvGetRegAddressWidth(devAddress);
////////    volatile uint16 regAdress = (AvGetRegAddress(devAddress)<<8) | AvGetRegAddress(regAddress);
////////    volatile uint8 busAddress = (uint8)AvGetI2cBusAddress(devAddress);
////////    
////////    if(busAddress!=0){
//////////        return AvOk;
////////        while(1);
////////    }
////////    
////////    if(regAddressWidth){
////////        //  16位寄存器地址
////////        if(IIC_RX_DATA_16bitreg(deviceAddress, regAdress, data, count) == 1){
////////            return AvOk;
////////        }
////////    }else{
////////        //  7位器件地址
//////////        if(IIC_RX_DATA(deviceAddress, regAdress, data, count) == 1){
//////////            return AvOk;
//////////        }
////////        while(1);
////////    }
////////    
////////    BSP_SYSTEMCLOCK_FUN.Delay_us(1000);
////////    return AvError;
}



static AvRet FUN_I2cRead_1(uint32 devAddress, uint32 regAddress, uint8 *data, uint16 count)
{
    AvRet ret = AvOk;
    uint8 deviceAddress = (uint8)AvGetI2cDeviceAddress(devAddress);
//    uint8 busAddress = (uint8)AvGetI2cBusAddress(devAddress);
    uint8 regAddressWidth = (uint8)AvGetRegAddressWidth(devAddress);
    regAddressWidth = (regAddressWidth == 0) ? 0 : 1;
    uint16 regAdress = (uint32)((AvGetRegAddress(devAddress)<<8) | AvGetRegAddress(regAddress));
    ManI2cRead_1(deviceAddress, regAdress, data, count, 1, regAddressWidth);
    return ret;

}

static AvRet FUN_I2cRead_2(uint32 devAddress, uint32 regAddress, uint8 *data, uint16 count)
{
    AvRet ret = AvOk;
    uint8 deviceAddress = (uint8)AvGetI2cDeviceAddress(devAddress);
//    uint8 busAddress = (uint8)AvGetI2cBusAddress(devAddress);
    uint8 regAddressWidth = (uint8)AvGetRegAddressWidth(devAddress);
    regAddressWidth = (regAddressWidth == 0) ? 0 : 1;
    uint16 regAdress = (uint32)((AvGetRegAddress(devAddress)<<8) | AvGetRegAddress(regAddress));
    ManI2cRead_X(deviceAddress, regAdress, data, count, 2, regAddressWidth);
    return ret;

}
static AvRet FUN_I2cRead_3(uint32 devAddress, uint32 regAddress, uint8 *data, uint16 count)
{
    AvRet ret = AvOk;
    uint8 deviceAddress = (uint8)AvGetI2cDeviceAddress(devAddress);
   // uint8 busAddress = (uint8)AvGetI2cBusAddress(devAddress);
    uint8 regAddressWidth = (uint8)AvGetRegAddressWidth(devAddress);
    regAddressWidth = (regAddressWidth == 0) ? 0 : 1;
    uint16 regAdress = (uint32)((AvGetRegAddress(devAddress)<<8) | AvGetRegAddress(regAddress));
    ManI2cRead_X(deviceAddress, regAdress, data, count, 3, regAddressWidth);
    return ret;

}
static AvRet FUN_I2cRead_4(uint32 devAddress, uint32 regAddress, uint8 *data, uint16 count)
{
    AvRet ret = AvOk;
    uint8 deviceAddress = (uint8)AvGetI2cDeviceAddress(devAddress);
//    uint8 busAddress = (uint8)AvGetI2cBusAddress(devAddress);
    uint8 regAddressWidth = (uint8)AvGetRegAddressWidth(devAddress);
    regAddressWidth = (regAddressWidth == 0) ? 0 : 1;
    uint16 regAdress = (uint32)((AvGetRegAddress(devAddress)<<8) | AvGetRegAddress(regAddress));
    ManI2cRead_X(deviceAddress, regAdress, data, count, 4, regAddressWidth);
    return ret;

}
static AvRet FUN_I2cWrite(uint32 devAddress, uint32 regAddress, uint8 *data, uint16 count)
{
    AvRet ret = AvOk;
    uint8 deviceAddress = (uint8)AvGetI2cDeviceAddress(devAddress);
    uint8 busAddress = (uint8)AvGetI2cBusAddress(devAddress);
    uint8 regAddressWidth = (uint8)AvGetRegAddressWidth(devAddress);
    regAddressWidth = (regAddressWidth == 0) ? 0 : 1;
    uint16 regAdress = (uint32)((AvGetRegAddress(devAddress)<<8) | AvGetRegAddress(regAddress));
    ManI2cWrite(deviceAddress, regAdress, data, count, busAddress, regAddressWidth);
    return ret;
////////    volatile uint8 deviceAddress = (((uint8)AvGetI2cDeviceAddress(devAddress))>>1);
////////    volatile uint8 regAddressWidth = (uint8)AvGetRegAddressWidth(devAddress);
////////    volatile uint16 regAdress = (AvGetRegAddress(devAddress)<<8) | AvGetRegAddress(regAddress);
////////    volatile uint8 busAddress = (uint8)AvGetI2cBusAddress(devAddress);
////////    
////////    if(busAddress!=0){
////////        while(1);
////////    }
////////    
////////    if(regAddressWidth){
////////        //  16位寄存器地址
////////        if(IIC_TX_DATA_16bitreg(deviceAddress, regAdress, data, count) == 1){
////////            return AvOk;
////////        }
////////    }else{
//////////        if(IIC_TX_DATA(deviceAddress, regAdress, data, count) == 1){
//////////            return AvOk;
//////////        }
////////        while(1);
////////    }
////////    BSP_SYSTEMCLOCK_FUN.Delay_us(1000);
////////    return AvError;
}

static AvRet FUN_I2cWrite_1(uint32 devAddress, uint32 regAddress, uint8 *data, uint16 count)
{
    AvRet ret = AvOk;
    uint8 deviceAddress = (uint8)AvGetI2cDeviceAddress(devAddress);
    //uint8 busAddress = (uint8)AvGetI2cBusAddress(devAddress);
    uint8 regAddressWidth = (uint8)AvGetRegAddressWidth(devAddress);
    regAddressWidth = (regAddressWidth == 0) ? 0 : 1;
    uint16 regAdress = (uint32)((AvGetRegAddress(devAddress)<<8) | AvGetRegAddress(regAddress));
    ManI2cWrite_1(deviceAddress, regAdress, data, count, 1, regAddressWidth);
    return ret;
}
static AvRet FUN_I2cWrite_2(uint32 devAddress, uint32 regAddress, uint8 *data, uint16 count)
{
    AvRet ret = AvOk;
    uint8 deviceAddress = (uint8)AvGetI2cDeviceAddress(devAddress);
    //uint8 busAddress = (uint8)AvGetI2cBusAddress(devAddress);
    uint8 regAddressWidth = (uint8)AvGetRegAddressWidth(devAddress);
    regAddressWidth = (regAddressWidth == 0) ? 0 : 1;
    uint16 regAdress = (uint32)((AvGetRegAddress(devAddress)<<8) | AvGetRegAddress(regAddress));
    ManI2cWrite_X(deviceAddress, regAdress, data, count, 2, regAddressWidth);
    return ret;
}
static AvRet FUN_I2cWrite_3(uint32 devAddress, uint32 regAddress, uint8 *data, uint16 count)
{
    AvRet ret = AvOk;
    uint8 deviceAddress = (uint8)AvGetI2cDeviceAddress(devAddress);
    //uint8 busAddress = (uint8)AvGetI2cBusAddress(devAddress);
    uint8 regAddressWidth = (uint8)AvGetRegAddressWidth(devAddress);
    regAddressWidth = (regAddressWidth == 0) ? 0 : 1;
    uint16 regAdress = (uint32)((AvGetRegAddress(devAddress)<<8) | AvGetRegAddress(regAddress));
    ManI2cWrite_X(deviceAddress, regAdress, data, count, 3, regAddressWidth);
    return ret;
}
static AvRet FUN_I2cWrite_4(uint32 devAddress, uint32 regAddress, uint8 *data, uint16 count)
{
    AvRet ret = AvOk;
    uint8 deviceAddress = (uint8)AvGetI2cDeviceAddress(devAddress);
    //uint8 busAddress = (uint8)AvGetI2cBusAddress(devAddress);
    uint8 regAddressWidth = (uint8)AvGetRegAddressWidth(devAddress);
    regAddressWidth = (regAddressWidth == 0) ? 0 : 1;
    uint16 regAdress = (uint32)((AvGetRegAddress(devAddress)<<8) | AvGetRegAddress(regAddress));
    ManI2cWrite_X(deviceAddress, regAdress, data, count, 4, regAddressWidth);
    return ret;
}
//static AvRet FUN_UartSendByte(uint8 *data, uint16 size)
//{
//    BSP_USART_FUN.Write(data, size);
//    return AvOk;
//}
#include "SEGGER_RTT.h"
#include "string.h"
static AvRet FUN_UartSendByte0(uint8 *data, uint16 size)
{
//    BSP_USART_FUN.Write(data, size);
	
	char str[size+1];
	memcpy(str, data, size);
    str[size]='\0';
//SEGGER_RTT_TerminalOut(1, str);
    extern void GSV2011_LOG(uint8_t id, const char * str);
    GSV2011_LOG(1, str);
    return AvOk;
}

static AvRet FUN_UartSendByte1(uint8 *data, uint16 size)
{
//    BSP_USART_FUN.Write(data, size);
	
	char str[size+1];
	memcpy(str, data, size);
    str[size]='\0';
//SEGGER_RTT_TerminalOut(2, str);
    extern void GSV2011_LOG(uint8_t id, const char * str);
    GSV2011_LOG(2, str);
    return AvOk;
}
static AvRet FUN_UartSendByte2(uint8 *data, uint16 size)
{
//    BSP_USART_FUN.Write(data, size);
	
	char str[size+1];
	memcpy(str, data, size);
    str[size]='\0';
//SEGGER_RTT_TerminalOut(3, str);
    
    extern void GSV2011_LOG(uint8_t id, const char * str);
    GSV2011_LOG(3, str);
    return AvOk;
}
static AvRet FUN_UartSendByte3(uint8 *data, uint16 size)
{
//    BSP_USART_FUN.Write(data, size);
	
	char str[size+1];
	memcpy(str, data, size);
    str[size]='\0';
//SEGGER_RTT_TerminalOut(4, str);
    extern void GSV2011_LOG(uint8_t id, const char * str);
    GSV2011_LOG(4, str);
    return AvOk;
}

static AvRet FUN_UartSendByte4(uint8 *data, uint16 size)
{
//    BSP_USART_FUN.Write(data, size);
	
	char str[size+1];
	memcpy(str, data, size);
    str[size]='\0';
//SEGGER_RTT_TerminalOut(5, str);
    extern void GSV2011_LOG(uint8_t id, const char * str);
    GSV2011_LOG(5, str);
    return AvOk;
}
static AvRet FUN_UartGetByte(uint8 *data)
{
//    if(BSP_USART_FUN.Read(data)){
//        
//        return AvOk;
//    }else{
//        return AvError;
//    }
    return AvError;
}

static AvRet FUN_RETURN_OK(uint8 *data)
{
    return AvOk;
}

static AvRet FUN_RETURN_AvNotAvailable(uint8 *data)
{
    return AvNotAvailable;
}

static void FUN_RST_ENABLE(void)
{
    BSP_FUN_GPIO.Reset(&GPIO_RESET);
}
static void FUN_RST_DISABLE(void)
{
    BSP_FUN_GPIO.Set(&GPIO_RESET);
}

BSP_GSV2011_FUN_typestruct BSP_GSV2011_FUN0 =
{
    .Init = FUN_Init,
    .I2cRead = FUN_I2cRead,
    .I2cWrite = FUN_I2cWrite,
    .UartSendByte = FUN_UartSendByte0,
    .UartGetByte = FUN_UartGetByte,
    .GetKey = FUN_RETURN_AvNotAvailable,
    .IrdaGetByte = FUN_RETURN_OK,
    .RST_Enable = FUN_RST_ENABLE,
    .RST_Disable = FUN_RST_DISABLE,
} ;
BSP_GSV2011_FUN_typestruct BSP_GSV2011_FUN1 =
{
    .Init = FUN_Init1,
    .I2cRead = FUN_I2cRead_1,
    .I2cWrite = FUN_I2cWrite_1,
    .UartSendByte = FUN_UartSendByte1,
    .UartGetByte = FUN_UartGetByte,
    .GetKey = FUN_RETURN_AvNotAvailable,
    .IrdaGetByte = FUN_RETURN_OK,
    .RST_Enable = FUN_RST_ENABLE,
    .RST_Disable = FUN_RST_DISABLE,
} ;
BSP_GSV2011_FUN_typestruct BSP_GSV2011_FUN2 =
{
    .Init = FUN_Init2,
    .I2cRead = FUN_I2cRead_2,
    .I2cWrite = FUN_I2cWrite_2,
    .UartSendByte = FUN_UartSendByte2,
    .UartGetByte = FUN_UartGetByte,
    .GetKey = FUN_RETURN_AvNotAvailable,
    .IrdaGetByte = FUN_RETURN_OK,
    .RST_Enable = FUN_RST_ENABLE,
    .RST_Disable = FUN_RST_DISABLE,
} ;
BSP_GSV2011_FUN_typestruct BSP_GSV2011_FUN3 =
{
    .Init = FUN_Init3,
    .I2cRead = FUN_I2cRead_3,
    .I2cWrite = FUN_I2cWrite_3,
    .UartSendByte = FUN_UartSendByte3,
    .UartGetByte = FUN_UartGetByte,
    .GetKey = FUN_RETURN_AvNotAvailable,
    .IrdaGetByte = FUN_RETURN_OK,
    .RST_Enable = FUN_RST_ENABLE,
    .RST_Disable = FUN_RST_DISABLE,
} ;
BSP_GSV2011_FUN_typestruct BSP_GSV2011_FUN4 =
{
    .Init = FUN_Init4,
    .I2cRead = FUN_I2cRead_4,
    .I2cWrite = FUN_I2cWrite_4,
    .UartSendByte = FUN_UartSendByte4,
    .UartGetByte = FUN_UartGetByte,
    .GetKey = FUN_RETURN_AvNotAvailable,
    .IrdaGetByte = FUN_RETURN_OK,
    .RST_Enable = FUN_RST_ENABLE,
    .RST_Disable = FUN_RST_DISABLE,
} ;

