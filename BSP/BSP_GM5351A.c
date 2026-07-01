
#include "BSP_GM5351A.h"

#include "BSP.h"
#include "BSP_SYSTEMCLOCK.h"

static BSP_GPIO_INFO_typedefstruct const GPIO_SDA =
{
    .mode = BSP_GPIO_MODE_AF_OD,
    .option = 1,
    .num = GPIO_PIN_7,
    .port = GPIOB,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_LOW,
} ;

static BSP_GPIO_INFO_typedefstruct const GPIO_SCL =
{
    .mode = BSP_GPIO_MODE_AF_OD,
    .option = 1,
    .num = GPIO_PIN_6,
    .port = GPIOB,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_LOW,
} ;

static I2C_Module * const module = I2C1;

static inline int i2c_master_send(uint8_t ICaddr, uint8_t* data, int len, uint32_t timeout)
{
    uint32_t time_cnt=0;
    
    //  等待空闲
    while (I2C_GetFlag(module, I2C_FLAG_BUSY))
    {
        time_cnt++;
        if(time_cnt>timeout){
            return -1;
        }
        BSP_SYSTEMCLOCK_FUN.Delay_us(10);
    }
    
    //  发送起始信号
    I2C_GenerateStart(module, ENABLE);
    while (!I2C_CheckEvent(module, I2C_EVT_MASTER_MODE_FLAG)) // EV5
    {
        time_cnt++;
        if(time_cnt>timeout){
            return -2;
        }
        BSP_SYSTEMCLOCK_FUN.Delay_us(10);
    }
    
    //  发送器件IC的地址
    I2C_SendAddr7bit(module, ICaddr, I2C_DIRECTION_SEND);   
    while (!I2C_CheckEvent(module, I2C_EVT_MASTER_TXMODE_FLAG)) // EV6
    {
        time_cnt++;
        if(time_cnt>timeout){
            return -3;
        }
        BSP_SYSTEMCLOCK_FUN.Delay_us(10);
    }
    
    // 发送数据
    for(uint16_t i=0; i<len; i++)
    {
        I2C_SendData(module, data[i]);
        while (!I2C_CheckEvent(module, I2C_EVT_MASTER_DATA_SENDING)) // EV8
        {
            time_cnt++;
            if(time_cnt>timeout){
                return i+1;
            }
            BSP_SYSTEMCLOCK_FUN.Delay_us(10);
        }
    }

    //  等待总线空闲
    while (!I2C_CheckEvent(module, I2C_EVT_MASTER_DATA_SENDED)) // EV8-2
    {
        time_cnt++;
        if(time_cnt>timeout){
            return -5;
        }
        BSP_SYSTEMCLOCK_FUN.Delay_us(10);
    }
    
    //  发送结束信号
    I2C_GenerateStop(module, ENABLE);
    while (I2C_GetFlag(module, I2C_FLAG_BUSY))
    {
        time_cnt++;
        if(time_cnt>timeout){
            return -6;
        }
        BSP_SYSTEMCLOCK_FUN.Delay_us(10);
    }
    
    return 0;
}

static inline int write_reg(uint8_t addr , uint8_t data)
{
    uint8_t buf[2]={addr, data};
    return i2c_master_send(0xC0 , buf, 2, 0xFFFF);
}

static void FUN_GM5351A_Init(void)
{
    I2C_InitType InitType;
    
    GPIO_ConfigPinRemap(GPIO_RMP_I2C1, DISABLE);
    
    BSP_FUN_GPIO.Init(&GPIO_SDA);
    BSP_FUN_GPIO.Init(&GPIO_SCL);
    
    BSP_FUN_GPIO.Set(&GPIO_SDA);
    BSP_FUN_GPIO.Set(&GPIO_SCL);
    
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_I2C1, ENABLE);
    
    I2C_DeInit(module);
    I2C_InitStruct(&InitType);
    InitType.BusMode     = I2C_BUSMODE_I2C;
    InitType.FmDutyCycle = I2C_FMDUTYCYCLE_2;
    InitType.OwnAddr1    = 0;
    InitType.AckEnable   = I2C_ACKEN;
    InitType.AddrMode    = I2C_ADDR_MODE_7BIT;
    InitType.ClkSpeed    = 100000; // 100K
    I2C_Init(module, &InitType);
    I2C_Enable(module, ENABLE);
}




//  寄存器地址，固定有哪些寄存器
static const uint8_t Address[]= {
				0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,//device,Add[0:8],i=0~8
				0x10,0x11,0x12,0x13,0x14,					//mpll,Add[10;14],i=9~13
                0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,0x31,0x32,0x33,//mspll0,,Add[20:33],,i=14~33
                0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0x50,0x51,0x52,0x53,//mspll1,,Add[40:53],,i=34~53
                0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x70,0x71,0x72,0x73};//mspll1,,Add[60:73],,i=54~73

//  寄存器的数据，按照寄存器地址排列，不同的输出频率，要求的寄存器的值各不相同
static uint8_t Registers[]={
				0x00,0x60,0x80,0x00,0x33,0x33,0x33,0x0A,0xF4,//device,Add[0:8],i=0~8
				0x80,0x74,0x49,0x00,0x00,					 //mpll,Add[10:14],i=9~13
                0x80,0x02,0x42,0x00,0x00,0x02,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x04,0x01, //mspll0,,Add[20:33],,i=14~33
                0x80,0x02,0x42,0x00,0x00,0x02,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x08,0x01, //mspll1,,Add[40:53],,i=34~53
                0xD0,0x02,0x42,0x00,0x00,0x02,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x28,0x01,};//mspll1,,Add[60:73],,i=54~73


const u8 F_xtal = 27; //★★★定义使用的晶振为27MHz

////指数计算函数
static u32 pow(u8 a,u8 b)
{
	if(b == 0)
		return 1;
	else if (b>=1)
		return a * pow (a,b-1);
    
    return 0;
}

// 设置GM5351A输出频率，单位是MHz
static inline void GM5351A_Set_Frequency (double F_out0,double F_out1,double F_out2)
{

	u8 MPLL_INPUT_DIV = 1;
	u8 MPLL_LOOP_DIV1ST = 4;  //bit[6:5]=10
	u8 MPLL_LOOP_DIV2ND = 9;  //bit[4:0]=01001
	u16 F_MPLL = 0;			  //搭配25~27M晶振时默认450MHz
	
	u32 SPLL0_POS_DIV,SPLL1_POS_DIV,SPLL2_POS_DIV;

	u8 SPLL0_LOOP_DIV1ST = 1;
	u8 SPLL1_LOOP_DIV1ST = 1;
	u8 SPLL2_LOOP_DIV1ST = 1;
	
	u8 SPLL0_LOOP_DIV2ND = 8;
	u8 SPLL1_LOOP_DIV2ND = 8;
	u8 SPLL2_LOOP_DIV2ND = 8;
	
	u8 SPLL0_INPUT_DIV = 1;
	u8 SPLL1_INPUT_DIV = 1;
	u8 SPLL2_INPUT_DIV = 1;

	u32 SPLL0_SYN_SET,SPLL1_SYN_SET,SPLL2_SYN_SET;
	float  F_SPLL0,F_SPLL1,F_SPLL2;
	float  F_syn0,F_syn1,F_syn2;

	F_MPLL = (F_xtal * MPLL_LOOP_DIV1ST * MPLL_LOOP_DIV2ND) / (MPLL_INPUT_DIV * 2);

/*---------SPLL0--------------------------------*/
	if (F_out0 >= 180 & F_out0 < 250)
		SPLL0_POS_DIV = 2;
	else if (F_out0 >= 120 & F_out0 < 180)
		SPLL0_POS_DIV = 3;
	else if (F_out0 >= 90 & F_out0 < 120)
		SPLL0_POS_DIV = 4;
	else if (F_out0 >= 70 & F_out0 < 90)
		SPLL0_POS_DIV = 5;
	else if (F_out0 >= 60 & F_out0 < 70)
		SPLL0_POS_DIV = 6;
	else if (F_out0 >= 45 & F_out0 < 60)
		SPLL0_POS_DIV = 8;
	else if (F_out0 >= 30 & F_out0 < 45)
		SPLL0_POS_DIV = 10;
	else if (F_out0 >= 23 & F_out0 < 30)
		SPLL0_POS_DIV = 16;
	else if (F_out0 >= 15 & F_out0 < 23)
		SPLL0_POS_DIV = 22;
	else if (F_out0 >= 10 & F_out0 < 15)
		SPLL0_POS_DIV = 32;
	else if (F_out0 >= 7 & F_out0 < 10)
		SPLL0_POS_DIV = 50;
	else if (F_out0 >= 5 & F_out0 < 7)
		SPLL0_POS_DIV = 72;
	else if (F_out0 >= 3 & F_out0 < 5)
		SPLL0_POS_DIV = 110;
	else if (F_out0 >= 3 & F_out0 < 5)
		SPLL0_POS_DIV = 110;
	else if (F_out0 >= 1.5 & F_out0 < 3)
		SPLL0_POS_DIV = 200;
	else if (F_out0 >= 0.8 & F_out0 < 1.5)
		SPLL0_POS_DIV = 400;
	else if (F_out0 >= 0.5 & F_out0 < 0.8)
		SPLL0_POS_DIV = 800;
	else if (F_out0 >= 0.2 & F_out0 < 0.5)
		SPLL0_POS_DIV = 1300;
	else if (F_out0 >= 0.1 & F_out0 < 0.2)
		SPLL0_POS_DIV = 3300;
	else if (F_out0 >= 0.05 & F_out0 < 0.1)
		SPLL0_POS_DIV = 6000;
	else if (F_out0 >= 0.025 & F_out0 < 0.05)
		SPLL0_POS_DIV = 12000;
	else if (F_out0 >= 0.0125 & F_out0 < 0.025)
		SPLL0_POS_DIV = 24000;
	else if (F_out0 >= 0.00625 & F_out0 < 0.0125)
		SPLL0_POS_DIV = 48000;
	else if (F_out0 >= 0.003125 & F_out0 < 0.00625)
		SPLL0_POS_DIV = 96000;
	else if (F_out0 >= 0.0015625 & F_out0 < 0.003125)
		SPLL0_POS_DIV = 192000;
	else if (F_out0 >= 0.00078125 & F_out0 < 0.0015625)
		SPLL0_POS_DIV = 384000;
	else if (F_out0 >= 0.000390625 & F_out0 < 0.00078125)
		SPLL0_POS_DIV = 768000;

	else
		SPLL0_POS_DIV = 1048575; //最大为2^20.
	
	F_SPLL0 = F_out0 * SPLL0_POS_DIV * 2 ;
	F_syn0 = (F_SPLL0 * SPLL0_INPUT_DIV) / (SPLL0_LOOP_DIV1ST * SPLL0_LOOP_DIV2ND) ;
	SPLL0_SYN_SET = (unsigned int)((F_MPLL * pow(2,23)) / F_syn0 ); 

	Registers[19] = (SPLL0_SYN_SET / 16777216) % 256;
	Registers[20] = (SPLL0_SYN_SET / 65536) % 256;
	Registers[21] = (SPLL0_SYN_SET / 256) % 256;
	Registers[22] = SPLL0_SYN_SET % 256;
	
	Registers[30] = (SPLL0_POS_DIV / 65536) % 256;
	Registers[31] = (SPLL0_POS_DIV / 256) % 256;
	Registers[32] = SPLL0_POS_DIV % 256;
	
/*---------SPLL1--------------------------------*/
	if (F_out1 >= 180 & F_out1 < 250)
		SPLL1_POS_DIV = 2;
	else if (F_out1 >= 120 & F_out1 < 180)
		SPLL1_POS_DIV = 3;
	else if (F_out1 >= 90 & F_out1 < 120)
		SPLL1_POS_DIV = 4;
	else if (F_out1 >= 70 & F_out1 < 90)
		SPLL1_POS_DIV = 5;
	else if (F_out1 >= 60 & F_out1 < 70)
		SPLL1_POS_DIV = 6;
	else if (F_out1 >= 45 & F_out1 < 60)
		SPLL1_POS_DIV = 8;
	else if (F_out1 >= 30 & F_out1 < 45)
		SPLL1_POS_DIV = 10;
	else if (F_out1 >= 23 & F_out1 < 30)
		SPLL1_POS_DIV = 16;
	else if (F_out1 >= 15 & F_out1 < 23)
		SPLL1_POS_DIV = 22;
	else if (F_out1 >= 10 & F_out1 < 15)
		SPLL1_POS_DIV = 32;
	else if (F_out1 >= 7 & F_out1 < 10)
		SPLL1_POS_DIV = 50;
	else if (F_out1 >= 5 & F_out1 < 7)
		SPLL1_POS_DIV = 72;
	else if (F_out1 >= 3 & F_out1 < 5)
		SPLL1_POS_DIV = 110;
	else if (F_out1 >= 3 & F_out1 < 5)
		SPLL1_POS_DIV = 110;
	else if (F_out1 >= 1.5 & F_out1 < 3)
		SPLL1_POS_DIV = 200;
	else if (F_out1 >= 0.8 & F_out1 < 1.5)
		SPLL1_POS_DIV = 400;
	else if (F_out1 >= 0.5 & F_out1 < 0.8)
		SPLL1_POS_DIV = 800;
	else if (F_out1 >= 0.2 & F_out1 < 0.5)
		SPLL1_POS_DIV = 1300;
	else if (F_out1 >= 0.1 & F_out1 < 0.2)
		SPLL1_POS_DIV = 3300;
	else if (F_out1 >= 0.05 & F_out1 < 0.1)
		SPLL1_POS_DIV = 6000;
	else if (F_out1 >= 0.025 & F_out1 < 0.05)
		SPLL1_POS_DIV = 12000;
	else if (F_out1 >= 0.0125 & F_out1 < 0.025)
		SPLL1_POS_DIV = 24000;
	else if (F_out1 >= 0.00625 & F_out1 < 0.0125)
		SPLL1_POS_DIV = 48000;
	else if (F_out1 >= 0.003125 & F_out1 < 0.00625)
		SPLL1_POS_DIV = 96000;
	else if (F_out1 >= 0.0015625 & F_out1 < 0.003125)
		SPLL1_POS_DIV = 192000;
	else if (F_out1 >= 0.00078125 & F_out1 < 0.0015625)
		SPLL1_POS_DIV = 384000;
	else if (F_out1 >= 0.000390625 & F_out1 < 0.00078125)
		SPLL1_POS_DIV = 768000;

	else
		SPLL1_POS_DIV = 1048575; //最大为2^20.
	
	
	F_SPLL1 = F_out1 * SPLL1_POS_DIV * 2 ;
	F_syn1 = (F_SPLL1 * SPLL1_INPUT_DIV) / (SPLL1_LOOP_DIV1ST * SPLL1_LOOP_DIV2ND) ;
	SPLL1_SYN_SET = (unsigned int)((F_MPLL * pow(2,23)) / F_syn1 );
	
	Registers[39] = (SPLL1_SYN_SET / 16777216) % 256;
	Registers[40] = (SPLL1_SYN_SET / 65536) % 256;
	Registers[41] = (SPLL1_SYN_SET / 256) % 256;
	Registers[42] = SPLL1_SYN_SET % 256;
	
	Registers[50] = (SPLL1_POS_DIV / 65536) % 256;
	Registers[51] = (SPLL1_POS_DIV / 256) % 256;
	Registers[52] = SPLL1_POS_DIV % 256; 	
	
/*---------SPLL2--------------------------------*/
	if (F_out2 >= 180 & F_out2 < 250)
		SPLL2_POS_DIV = 2;
	else if (F_out2 >= 120 & F_out2 < 180)
		SPLL2_POS_DIV = 3;
	else if (F_out2 >= 90 & F_out2 < 120)
		SPLL2_POS_DIV = 4;
	else if (F_out2 >= 70 & F_out2 < 90)
		SPLL2_POS_DIV = 5;
	else if (F_out2 >= 60 & F_out2 < 70)
		SPLL2_POS_DIV = 6;
	else if (F_out2 >= 45 & F_out2 < 60)
		SPLL2_POS_DIV = 8;
	else if (F_out2 >= 30 & F_out2 < 45)
		SPLL2_POS_DIV = 10;
	else if (F_out2 >= 23 & F_out2 < 30)
		SPLL2_POS_DIV = 16;
	else if (F_out2 >= 15 & F_out2 < 23)
		SPLL2_POS_DIV = 22;
	else if (F_out2 >= 10 & F_out2 < 15)
		SPLL2_POS_DIV = 32;
	else if (F_out2 >= 7 & F_out2 < 10)
		SPLL2_POS_DIV = 50;
	else if (F_out2 >= 5 & F_out2 < 7)
		SPLL2_POS_DIV = 72;
	else if (F_out2 >= 3 & F_out2 < 5)
		SPLL2_POS_DIV = 110;
	else if (F_out2 >= 3 & F_out2 < 5)
		SPLL2_POS_DIV = 110;
	else if (F_out2 >= 1.5 & F_out2 < 3)
		SPLL2_POS_DIV = 200;
	else if (F_out2 >= 0.8 & F_out2 < 1.5)
		SPLL2_POS_DIV = 400;
	else if (F_out2 >= 0.5 & F_out2 < 0.8)
		SPLL2_POS_DIV = 800;
	else if (F_out2 >= 0.2 & F_out2 < 0.5)
		SPLL2_POS_DIV = 1300;
	else if (F_out2 >= 0.1 & F_out2 < 0.2)
		SPLL2_POS_DIV = 3300;
	else if (F_out2 >= 0.05 & F_out2 < 0.1)
		SPLL2_POS_DIV = 6000;
	else if (F_out2 >= 0.025 & F_out2 < 0.05)
		SPLL2_POS_DIV = 12000;
	else if (F_out2 >= 0.0125 & F_out2 < 0.025)
		SPLL2_POS_DIV = 24000;
	else if (F_out2 >= 0.00625 & F_out2 < 0.0125)
		SPLL2_POS_DIV = 48000;
	else if (F_out2 >= 0.003125 & F_out2 < 0.00625)
		SPLL2_POS_DIV = 96000;
	else if (F_out2 >= 0.0015625 & F_out2 < 0.003125)
		SPLL2_POS_DIV = 192000;
	else if (F_out2 >= 0.00078125 & F_out2 < 0.0015625)
		SPLL2_POS_DIV = 384000;
	else if (F_out2 >= 0.000390625 & F_out2 < 0.00078125)
		SPLL2_POS_DIV = 768000;

	else
		SPLL2_POS_DIV = 1048575; //最大为2^20.
	
	
	F_SPLL2 = F_out2 * SPLL2_POS_DIV * 2 ;
	F_syn2 = (F_SPLL2 * SPLL2_INPUT_DIV) / (SPLL2_LOOP_DIV1ST * SPLL2_LOOP_DIV2ND) ;
	SPLL2_SYN_SET = (unsigned int)((F_MPLL * pow(2,23)) / F_syn2 ); 
	
	Registers[59] = (SPLL2_SYN_SET / 16777216) % 256;
	Registers[60] = (SPLL2_SYN_SET / 65536) % 256;
	Registers[61] = (SPLL2_SYN_SET / 256) % 256;
	Registers[62] = SPLL2_SYN_SET % 256;
	
	Registers[70] = (SPLL2_POS_DIV / 65536) % 256;
	Registers[71] = (SPLL2_POS_DIV / 256) % 256;
	Registers[72] = SPLL2_POS_DIV % 256;

}

static char FUN_GM5351A_Set(double out1 , double out2 , double out3)
{
    if(write_reg(Address[0], Registers[0])!=0){
        return 0;
    }
    BSP_SYSTEMCLOCK_FUN.Delay_us(100);
    
    GM5351A_Set_Frequency(out1, out2, out3);
    
    for(uint8_t i=0; i<(sizeof(Address)); i++){
        if(write_reg(Address[i], Registers[i])!=0){
            return 0;
        }
    }
    return 1;
}

BSP_GM5351A_FUN_typestruct BSP_GM5351A_FUN =
{
    .Init = FUN_GM5351A_Init,
    .Set = FUN_GM5351A_Set,
} ;
