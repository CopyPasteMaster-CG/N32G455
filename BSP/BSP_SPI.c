#include "BSP_SPI.h"
#include "BSP.h"


static BSP_GPIO_INFO_typedefstruct const GPIO_SPI_FPGA_CS ={
    .mode = BSP_GPIO_MODE_OUTPUT_PP,
    .option = 0,
    .num = GPIO_PIN_12,
    .port = GPIOB,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;

static BSP_GPIO_INFO_typedefstruct const GPIO_SPI_FPGA_SCLK ={
    .mode = BSP_GPIO_MODE_AF_PP,
    .option = 0,
    .num = GPIO_PIN_13,
    .port = GPIOB,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;

static BSP_GPIO_INFO_typedefstruct const GPIO_SPI_FPGA_MISO ={
    .mode = BSP_GPIO_MODE_INPUT,
    .option = 0,
    .num = GPIO_PIN_14,
    .port = GPIOB,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;

static BSP_GPIO_INFO_typedefstruct const GPIO_SPI_FPGA_MOSI ={
    .mode = BSP_GPIO_MODE_AF_PP,
    .option = 0,
    .num = GPIO_PIN_15,
    .port = GPIOB,
    .pull = BSP_GPIO_PULL_NO,
    .speed = BSP_GPIO_SPEED_HIGH,
} ;

static SPI_Module * const hardware = SPI2;              //  总线


static void SPI_Configuratio(){
    SPI_InitType InitStruct={0};
    
    //  初始化SPI的引脚
    BSP_FUN_GPIO.Init(&GPIO_SPI_FPGA_CS) ;
    BSP_FUN_GPIO.Init(&GPIO_SPI_FPGA_SCLK) ;
    BSP_FUN_GPIO.Init(&GPIO_SPI_FPGA_MISO) ;
    BSP_FUN_GPIO.Init(&GPIO_SPI_FPGA_MOSI) ;
    
    BSP_FUN_GPIO.Set(&GPIO_SPI_FPGA_CS);
    
    GPIO_ConfigPinRemap(GPIO_RMP1_SPI2, DISABLE);
        
    //  配置SPI总线
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_SPI2, ENABLE);
    
    
    SPI_InitStruct(&InitStruct);
	InitStruct.DataDirection = SPI_DIR_DOUBLELINE_FULLDUPLEX;
    InitStruct.SpiMode       = SPI_MODE_MASTER;
    InitStruct.DataLen       = SPI_DATA_SIZE_8BITS;
    InitStruct.CLKPOL        = SPI_CLKPOL_LOW;
    InitStruct.CLKPHA        = SPI_CLKPHA_FIRST_EDGE;
    InitStruct.NSS           = SPI_NSS_SOFT;
    InitStruct.BaudRatePres  = SPI_BR_PRESCALER_4;
    InitStruct.FirstBit      = SPI_FB_MSB;
    InitStruct.CRCPoly       = 7;
    SPI_Init(hardware, &InitStruct);
    SPI_SSOutputEnable(hardware, ENABLE);
    
    SPI_Enable(hardware, ENABLE);

}
void SPI_Write( void * const tx_buf, void * rx_buf, uint8_t len) 
{
    uint32_t errcnt;
    uint8_t data;
    uint32_t rx_len = 0;
    uint32_t tx_len = 0;
    BSP_FUN_GPIO.Reset(&GPIO_SPI_FPGA_CS);
    while((rx_len<len) || (tx_len<len)){
        if(SPI_I2S_GetStatus(hardware, SPI_I2S_RNE_FLAG) == SET){
            data = SPI_I2S_ReceiveData(hardware);
            if(rx_len<len){
                ((uint8_t*)rx_buf)[rx_len] = data;
                rx_len++;
            }
        }
        if(SPI_I2S_GetStatus(hardware, SPI_I2S_TE_FLAG) == SET){
            if(tx_len<len){
                SPI_I2S_TransmitData(hardware, ((uint8_t*)tx_buf)[tx_len]);
                tx_len++;
            }
        }
        if(SPI_I2S_GetStatus(hardware, SPI_I2S_BUSY_FLAG) == SET){
            errcnt=0;
        }else{
            errcnt++;
            if(errcnt>1000){
                break;
            }
        }
    }
    while(SPI_I2S_GetStatus(hardware, SPI_I2S_RNE_FLAG) == SET){  //  等待 SPI_STS.RNE 标志位置 1
        SPI_I2S_ReceiveData(hardware);
    } 
    while(SPI_I2S_GetStatus(hardware, SPI_I2S_BUSY_FLAG) == SET){}  //  等待 SPI_STS.BUSY 标志位清 0；
    BSP_FUN_GPIO.Set(&GPIO_SPI_FPGA_CS);
}


BSP_SPI_FUN_typestruct BSP_SPI_FUN={
    .Init   =   SPI_Configuratio,
    .Write  =   SPI_Write,
};


