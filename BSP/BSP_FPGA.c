
#include "BSP_SYSTEMCLOCK.h"
#include "BSP_FPGA.h"
#include "BSP.h"
#include "main.h"

//  FPGA反馈配置文件是否正确，1代表正确
static BSP_GPIO_INFO_typedefstruct const GPIO_INIT_FLAG_N ={
    .mode = BSP_GPIO_MODE_OUTPUT_OD,
    .option = 0,
    .num = GPIO_PIN_10,
    .port = GPIOC,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;

//  FPGA反馈配置是否完成，1代表完成
static BSP_GPIO_INFO_typedefstruct const GPIO_CFG_DONE ={
    .mode = BSP_GPIO_MODE_INPUT,
    .option = 0,
    .num = GPIO_PIN_13,
    .port = GPIOC,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;

//  复位FPGA，低电平复位
static BSP_GPIO_INFO_typedefstruct const GPIO_RSTN ={
    .mode = BSP_GPIO_MODE_OUTPUT_OD,
    .option = 0,
    .num = GPIO_PIN_11,
    .port = GPIOC,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;

//  CFG_CLK 向FPGA下载固件时，使用SPI总线，这个连接到单片机的SPI-CLK引脚上
static BSP_GPIO_INFO_typedefstruct const GPIO_CFG_CLK ={
    .mode = BSP_GPIO_MODE_AF_PP,
    .option = 0,
    .num = GPIO_PIN_3,
    .port = GPIOB,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;

//  DI 向FPGA下载固件时，使用SPI总线，这个连接到单片机的SPI-MOSI引脚上
static BSP_GPIO_INFO_typedefstruct const GPIO_DI ={
    .mode = BSP_GPIO_MODE_AF_PP,
    .option = 0,
    .num = GPIO_PIN_5,
    .port = GPIOB,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;
//static BSP_GPIO_INFO_typedefstruct const GPIO_RST ={
//    .mode = BSP_GPIO_MODE_OUTPUT_OD,
//    .option = 0,
//    .num = GPIO_PIN_0,
//    .port = GPIOA,
//    .pull = BSP_GPIO_PULL_NO,
//    .speed = BSP_GPIO_SPEED_HIGH,
//} ;

static SPI_Module * const hardware = SPI1;              //  总线
static DMA_ChannelType * const dma_chx = DMA1_CH3;      //  DMA通道
static volatile uint8_t tx_buf[512];                    //  DMA发送缓存
static volatile uint32_t tx_point;                      //  发送指针
static const uint32_t Firmware_ADDRESS  = 0x90000000;   //  FPGA固件的存放地址，对于本程序来说只读，由bootloader负责更改
static const uint32_t Firmware_SIZE     = 0x003C0000;   //  FPGA固件的存放地址，对于本程序来说只读，由bootloader负责更改
static volatile bool FPGA_READY=false;

#include "string.h"
static bool FPGA_DeInit(void)
{
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_SPI1, ENABLE);
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPH_DMA1, ENABLE);
    
    DMA_EnableChannel(dma_chx, DISABLE);                     //  DMA 通道禁用
    SPI_Enable(hardware, DISABLE);                           //  SPI 使能
    
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_SPI1, DISABLE);
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPH_DMA1, DISABLE);
    
    return true;
}
static bool FPGA_init(void)
{
    SPI_InitType InitStruct={0};
    DMA_InitType DMA_InitStructure={0};
    NVIC_InitType NVIC_InitStructure={0};
    
    tx_point = 0;
    for(uint16_t i=0; i<sizeof(tx_buf); i++){
        tx_buf[i] = ((uint8_t*)(tx_point+Firmware_ADDRESS))[i];
    }
    tx_point+=sizeof(tx_buf);
    
    FPGA_DeInit();
    
    //  初始化SPI的引脚
    BSP_FUN_GPIO.Init(&GPIO_CFG_CLK) ;
    BSP_FUN_GPIO.Init(&GPIO_DI) ;
    BSP_FUN_GPIO.Init(&GPIO_RST) ;
    GPIO_ConfigPinRemap(GPIO_RMP1_SPI1, ENABLE);
        
    //  配置SPI总线
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_SPI1, ENABLE);
    SPI_InitStruct(&InitStruct);
    InitStruct.DataDirection=   SPI_DIR_SINGLELINE_TX;
    InitStruct.SpiMode      =   SPI_MODE_MASTER;
    InitStruct.DataLen      =   SPI_DATA_SIZE_8BITS;
    InitStruct.CLKPOL       =   SPI_CLKPOL_LOW;
    InitStruct.CLKPHA       =   SPI_CLKPHA_FIRST_EDGE;
    InitStruct.NSS          =   SPI_NSS_SOFT;
    InitStruct.BaudRatePres =   SPI_BR_PRESCALER_4;
    InitStruct.FirstBit     =   SPI_FB_MSB;
    InitStruct.CRCPoly      =   7;
    SPI_Init(hardware, &InitStruct);
    
    //  配置SPI总线TX对应的DMA通道
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPH_DMA1, ENABLE);
    DMA_DeInit(dma_chx);
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.PeriphAddr     = (uint32_t)(&hardware->DAT);
    DMA_InitStructure.MemAddr        = (uint32_t)tx_buf;
    DMA_InitStructure.Direction      = DMA_DIR_PERIPH_DST;
    DMA_InitStructure.BufSize        = sizeof(tx_buf);
    DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_DISABLE;
    DMA_InitStructure.DMA_MemoryInc  = DMA_MEM_INC_ENABLE;
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_SIZE_BYTE;
    DMA_InitStructure.MemDataSize    = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.CircularMode   = DMA_MODE_CIRCULAR;
    DMA_InitStructure.Priority       = DMA_PRIORITY_VERY_HIGH;
    DMA_InitStructure.Mem2Mem        = DMA_M2M_DISABLE;
    DMA_Init(dma_chx, &DMA_InitStructure);
    
    
    //  SPI的中断配置
    NVIC_InitStructure.NVIC_IRQChannel                   = SPI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    //  DMA通道的中断配置
    NVIC_InitStructure.NVIC_IRQChannel                   = DMA1_Channel3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    BSP_FUN_GPIO.Init(&GPIO_INIT_FLAG_N) ;
    BSP_FUN_GPIO.Init(&GPIO_CFG_DONE) ;
    BSP_FUN_GPIO.Init(&GPIO_RSTN) ;
    
    BSP_FUN_GPIO.Set(&GPIO_INIT_FLAG_N) ;
    BSP_FUN_GPIO.Reset(&GPIO_RSTN) ;
    
    //  启用DMA的中断标志
    DMA_ConfigInt(dma_chx, DMA_INT_TXC, ENABLE);
    DMA_ConfigInt(dma_chx, DMA_INT_HTX, ENABLE);
    
    
    SPI_I2S_EnableDma(hardware, SPI_I2S_DMA_TX, ENABLE);    //  发送缓存 DMA 使能
    SPI_Enable(hardware, ENABLE);                           //  SPI 使能
//    DMA_EnableChannel(dma_chx, ENABLE);                     //  DMA 通道使能
    
    
    FPGA_READY=false;
    BSP_SYSTEMCLOCK_FUN.Delay_us(50);
    BSP_FUN_GPIO.Set(&GPIO_RSTN) ;
    BSP_FUN_GPIO.Reset(&GPIO_RST);
    
    while(BSP_FUN_GPIO.Read(&GPIO_INIT_FLAG_N)!=true){
        BSP_SYSTEMCLOCK_FUN.Delay_us(5);
    }
    BSP_SYSTEMCLOCK_FUN.Delay_us(1000);
    
    
    FPGA_READY=true;
    DMA_EnableChannel(dma_chx, ENABLE);                     //  DMA 通道使能
    return true;
}

static inline void DMA_DATA_COPY(uint32_t address)
{
//    if(FPGA_READY){
//        if((tx_point<Firmware_SIZE)){
//            for(uint16_t i=0; i<(sizeof(tx_buf)/2); i++){
//                tx_buf[i+address] = ((uint8_t*)(tx_point+Firmware_ADDRESS))[i];
//            }
//            tx_point+=sizeof(tx_buf)/2;
//        }else{
//            for(uint16_t i=0; i<(sizeof(tx_buf)/2); i++){
//                tx_buf[i+address] = 0xFF;

//            }
//        }
//    }
    
     if(FPGA_READY){
        if((tx_point<Firmware_SIZE)){
            register uint32_t temp_1,temp_2;
            for(uint16_t i=0; i<(sizeof(tx_buf)/8); i+=2){
                temp_1 =((uint32_t*) (tx_point+Firmware_ADDRESS)) [i];
                temp_2 =((uint32_t*) (tx_point+Firmware_ADDRESS)) [i+1];
                
                tx_buf[address+i*4+7] =temp_2>>0;
                tx_buf[address+i*4+6] =temp_2>>8;
                tx_buf[address+i*4+5] =temp_2>>16;
                tx_buf[address+i*4+4] =temp_2>>24;
                
                tx_buf[address+i*4+3] =temp_1>>0;
                tx_buf[address+i*4+2] =temp_1>>8;
                tx_buf[address+i*4+1] =temp_1>>16;
                tx_buf[address+i*4+0] =temp_1>>24;
            }
            tx_point+=sizeof(tx_buf)/2;
        }else{
            for(uint16_t i=0; i<(sizeof(tx_buf)/2); i++){
                tx_buf[i+address] = 0xFF;
                DMA_EnableChannel (dma_chx, DISABLE);
            }
        }
    }
    
    
}
static bool FPGA_CFG_DONE(void)
{
    if(BSP_FUN_GPIO.Read(&GPIO_CFG_DONE)==true){
        BSP_FUN_GPIO.Reset(&GPIO_RST);
        BSP_SYSTEMCLOCK_FUN.Delay_us(100000);   
        BSP_FUN_GPIO.Set(&GPIO_RST);
    }
    return BSP_FUN_GPIO.Read(&GPIO_CFG_DONE)==true;
}

//  传输了一半
void DMA1_CH3_IRQ_HT_Callback(void)
{
    DMA_DATA_COPY(0);
}

//  传输完成
void DMA1_CH3_IRQ_TC_Callback(void)
{
    DMA_DATA_COPY(sizeof(tx_buf)/2);
}

BSP_FPGA_FUN_typestruct BSP_FPGA_FUN={
    .Init = FPGA_init,
    .CFG_DONE = FPGA_CFG_DONE,
};
