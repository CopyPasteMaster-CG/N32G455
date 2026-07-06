
#include "APP_FPGA.h"
#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "BSP_FPGA.h"
#include "BSP_SPI.h"
#include "BSP_GM5351A.h"
#include "BSP_USART.h"

/*******************任务配置开始*************************************************/
static APP_FPGA_DATA_Typedefstruct app_data;    //  本任务与其它任务共享的数据
static rt_thread_t handle = RT_NULL;            //  任务句柄
static rt_event_t event_handle = RT_NULL;       //  任务对应的事件句柄，事件已经全部定义完成，不要更改
static rt_mutex_t mutex_handle = RT_NULL;       //  任务共享数据用到的互斥量句柄
static void function_thread(void * pragram);    //  任务对应的函数

//  向共享内存，写入数据，本任务专用
static inline void FUN_WRITE(APP_FPGA_DATA_Typedefstruct * data)
{
    rt_mutex_take(mutex_handle, RT_TICK_MAX);
    memcpy(&app_data, data, sizeof(APP_FPGA_DATA_Typedefstruct));
    rt_mutex_release(mutex_handle);
}

//  从共享内存读取数据
static int FUN_READ(void * data)
{
    //  判断互斥量的句柄是否被定义
    while(mutex_handle == RT_NULL){
        rt_thread_delay(1000);
    }
    rt_mutex_take(mutex_handle, RT_TICK_MAX);
    memcpy(data, &app_data, sizeof(APP_FPGA_DATA_Typedefstruct));
    rt_mutex_release(mutex_handle);
    return 1;
}

//  共享内存用到的互斥量
static void FUN_EVENT(int event)
{
    //  判断事件的句柄是否被定义
    while(event_handle == NULL){
        rt_thread_delay(1000);
    }
    
    rt_event_recv(  event_handle        /*  事件的句柄，私有句柄*/
                ,   event               /*  等待发生的事件*/
                ,   RT_EVENT_FLAG_AND   /*  所有事件都需要发生*/
                ,   RT_TICK_MAX         /*  死等*/
                ,   RT_NULL );          /*  无回调函数*/
}

//  创建任务用到的函数
static int FUN_CREATE(void)
{
    if(handle == RT_NULL){
        handle = rt_thread_create(  "FPGA",             /*  线程名字*/                    
                                    function_thread,    /*  线程入口函数*/
                                    NULL,               /*  线程入口函数参数*/
                                    4096,               /*  线程栈大小*/
                                    3 ,                 /*  线程优先级*/
                                    20);                /*  线程时间片*/
    }
    if(event_handle == RT_NULL){
        event_handle = rt_event_create( "FPGA", RT_IPC_FLAG_PRIO );
    }
    if(mutex_handle == RT_NULL){
        mutex_handle = rt_mutex_create( "FPGA", RT_IPC_FLAG_PRIO);														
    }
    if(event_handle == RT_NULL){
        return APP_CREATE_ERR_EVENT;
    }else if(handle == RT_NULL){
        return APP_CREATE_ERR_HANDLE;
    }else if(mutex_handle == RT_NULL){
        return APP_CREATE_ERR_MUTEX;
    }else{
        rt_thread_startup(handle);	
        return APP_CREATE_OK;
    }
}

const APP_HANDLE_typedefstruct APP_FPGA_FUN={
    .Create = FUN_CREATE,
    .Read = FUN_READ,
    .Event = FUN_EVENT,
};
/*******************任务配置结束*************************************************/

/*******************以下全部是个性化自定义任务逻辑*************************************************/
static void write_fpga_reg_from_usart(void)
{
    //  串口写寄存器
    extern rt_mq_t APP_USART_QUEUE_RX;
    extern rt_mq_t APP_USART_QUEUE_TX;
    APP_USART_QURUR_DATA_Typedefstruct queue_data;
    
    if(rt_mq_recv(APP_USART_QUEUE_RX, &queue_data, sizeof(queue_data), 0) == RT_EOK){
        //  有数据需要发送
        BSP_SPI_FUN.Write(queue_data.buf, queue_data.buf, 3);
        rt_mq_send(APP_USART_QUEUE_TX, &queue_data, sizeof(queue_data));
    }
}
static bool write_fpga_mode(uint8_t address, uint16_t data, uint16_t mask)
{
    uint8_t err_count=0;
    uint8_t spi_tx_buf[5];
    uint8_t spi_rx_buf[5];
    uint16_t temp;
    
    while(1){
        spi_tx_buf[0] = 0X80|address;
        spi_tx_buf[1] = 0;
        spi_tx_buf[2] = 0;
        BSP_SPI_FUN.Write(spi_tx_buf, spi_rx_buf, 3);
        temp=spi_rx_buf[1]*256+spi_rx_buf[2];
        if((temp&mask) == (data&mask)){
            return true;
        }else{
            err_count++;
            if(err_count>5){
                return false;
            }
        }
        
        rt_thread_delay(1);
        spi_tx_buf[0] = address;
        spi_tx_buf[1] = (data&mask)/0x100;
        spi_tx_buf[2] = (data&mask)%0x100;
        BSP_SPI_FUN.Write(spi_tx_buf, spi_rx_buf, 3);
        rt_thread_delay(1);
    }
}


struct _FPGA_OUT_CONFIG{
    double  fre;                //  GM5351A的输出频率
    uint16_t x;                 //  输出分辨率-水平方向
    uint16_t y;                 //  输出分辨率-垂直方向
};
    
static const struct _FPGA_OUT_CONFIG OUT_2160P={
    .fre            =   148.5,
    .x              =   3840,
    .y              =   2160,
};
static const struct _FPGA_OUT_CONFIG OUT_1080P={
    .fre            =   37.125,
    .x              =   1920,
    .y              =   1080,
};
static const struct _FPGA_OUT_CONFIG OUT_3440P={
    .fre            =   104.98775,
    .x              =   3440,
    .y              =   1440,
};
static const struct _FPGA_OUT_CONFIG OUT_2560P={
    .fre            =   78.11375,
    .x              =   2560,
    .y              =   1440,
};

const uint16_t VideoTable1[] = {
    4096,2160,30,
    3840,2160,30,
    3440,1440,60,
    3440,1440,50,
    3440,1440,30,
    2560,1600,50,
    2560,1600,30,
    2560,1440,50,
    2560,1440,30,
    1920,1200,60,
    1920,1200,30,
    1600,1200,60,
    

    
};

const uint16_t VideoTable0[] = {
    4096,2160,60,
    4096,2160,50,
    3840,2160,60,
    3840,2160,50,
    2560,1600,60,
    2560,1440,60,
    2560,1080,60,
    2560,1080,50,
    2560,1080,30,
    1920,1200,60,
    1920,1080,60,
    1920,1080,50,
    1920,1080,30,

    

};

#define TABLE1_COUNT  (sizeof(VideoTable1)/sizeof(VideoTable1[0])/3)
#define TABLE0_COUNT  (sizeof(VideoTable1)/sizeof(VideoTable0[0])/3)
static int found1=255;
static int found2=255;
static int found3=255;
static int found4=255;
//static inline bool set_fpga_mode(uint8_t mode, uint16_t x, uint16_t y, struct _FPGA_OUT_CONFIG const * OUT)
static inline bool set_fpga_mode(uint8_t mode,uint8_t out, APP_GSV2011_DATA_TYPEDEFSTRUCT *gsv2011_1, APP_GSV2011_DATA_TYPEDEFSTRUCT *gsv2011_2, APP_GSV2011_DATA_TYPEDEFSTRUCT *gsv2011_3, APP_GSV2011_DATA_TYPEDEFSTRUCT *gsv2011_4, uint8_t channel)
{    
    bool retu=true;;
//    struct _FPGA_OUT_CONFIG const * OUT;
    static const struct{
        uint8_t mode;   //  模式
        uint8_t x1_drv;  //  X轴方向被分成几份
        uint8_t y1_drv;  //  Y轴方向被分成几份
        uint8_t drv1;  //  分成几份
        uint8_t x2_drv;  //  X轴方向被分成几份
        uint8_t y2_drv;
        uint8_t drv2;  //  分成几份
        uint8_t x3_drv;  //  X轴方向被分成几份
        uint8_t y3_drv;  //  Y轴方向被分成几份
        uint8_t drv3;  //  分成几份
        uint8_t x4_drv;  //  X轴方向被分成几份
        uint8_t y4_drv;  //  Y轴方向被分成几份
        uint8_t drv4;  //  分成几份
    }config[]={
        //  输出第一个输入画面，全屏显示
        {   .mode   =    0,
            .x1_drv  =   1,
            .y1_drv  =   1, 
            .drv1    =   1,   
            .x2_drv  =   0,
            .y2_drv  =   0,
            .drv2    =   1,             
            .x3_drv  =   0,
            .y3_drv  =   0,
            .drv3    =   1, 
            .x4_drv  =   0,
            .y4_drv  =   0,
            .drv4    =   1, 

            },
        
           //  输出第二个输入画面，全屏显示
        {   .mode   =   1,
            .x1_drv  =   0,
            .y1_drv  =   0,
            .drv1    =   1,             
            .x2_drv  =   1,
            .y2_drv  =   1, 
            .drv2    =   1, 
            .x3_drv  =   0,
            .y3_drv  =   0,
            .drv3    =   1, 
            .x4_drv  =   0,
            .y4_drv  =   0,
            .drv4    =   1, 
        },
        
        //   输出第三个输入画面，全屏显示
        {   .mode   =   2,
            .x1_drv  =   0,
            .y1_drv  =   0,
            .drv1    =   1,             
            .x2_drv  =   0,
            .y2_drv  =   0,
            .drv2    =   1, 
            .x3_drv  =   1,
            .y3_drv  =   1,
            .drv3    =   1, 
            .x4_drv  =   0,
            .y4_drv  =   0,
            .drv4    =   1,             
        },
        
        // 输出第四个输入画面，全屏显示
        {   .mode   =   3,
            .x1_drv  =   0,
            .y1_drv  =   0,
            .drv1    =   1,             
            .x2_drv  =   0,
            .y2_drv  =   0,
            .drv2    =   1, 
            .x3_drv  =   0,
            .y3_drv  =   0,
            .drv3    =   1, 
            .x4_drv  =   1,
            .y4_drv  =   1,
            .drv4    =   1, 
        },
        
        //  输出四宫格画面，左上为1号口输入，左下为2号口输入，右上为3号口输入，右下为4号口输入
        {   .mode   =   4,
            .x1_drv  =   2,
            .y1_drv  =   2,
            .drv1    =   1, 
            .x2_drv  =   2,
            .y2_drv  =   2,
            .drv2    =   1, 
            .x3_drv  =   2,
            .y3_drv  =   2,
            .drv3    =   1, 
            .x4_drv  =   2,
            .y4_drv  =   2,
            .drv4    =   1, 
        },
        //  预留
        {   .mode   =   5,
            .x1_drv  =   2,
            .y1_drv  =   2,
            .drv1    =   1, 
            .x2_drv  =   2,
            .y2_drv  =   2,
            .drv2    =   1, 
            .x3_drv  =   2,
            .y3_drv  =   2,
            .drv3    =   1, 
            .x4_drv  =   2,
            .y4_drv  =   2,
            .drv4    =   1, 
        },
        // 输出三大一小画面，左边为1号口输入，右上为2号口输入，右中间为3号口输入，右下为4号口输入
        {   .mode   =   6,
            .x1_drv  =   3,
            .y1_drv  =   3,
            .drv1    =   2,     
            .x2_drv  =   3,
            .y2_drv  =   3,
            .drv2    =   1, 
            .x3_drv  =   3,
            .y3_drv  =   3,
            .drv3    =   1, 
            .x4_drv  =   3,
            .y4_drv  =   3,
            .drv4    =   1, 
        },
        
        //  输出三大一小画面，左边为2号口输入，右上为3号口输入，右中间为4号口输入，右下为1号口输入
        {   .mode   =   7,
            .x1_drv  =   3,
            .y1_drv  =   3,
            .drv1    =   1,     
            .x2_drv  =   3,
            .y2_drv  =   3,
            .drv2    =   2,     
            .x3_drv  =   3,
            .y3_drv  =   3,
            .drv3    =   1, 
            .x4_drv  =   3,
            .y4_drv  =   3,
            .drv4    =   1, 
        },
        
        //  输出三大一小画面，左边为3号口输入，右上为4号口输入，右中间为1号口输入，右下为2号口输入
        {    .mode   =   8,
            .x1_drv  =   3,
            .y1_drv  =   3,
            .drv1    =   1, 
            .x2_drv  =   3,
            .y2_drv  =   3,
            .drv2    =   1, 
            .x3_drv  =   3,
            .y3_drv  =   3,
            .drv3    =   2, 
            .x4_drv  =   3,
            .y4_drv  =   3,
            .drv4    =   1, 
        },
        
        //  输出三大一小画面，左边为4号口输入，右上为1号口输入，右中间为2号口输入，右下为3号口输入
        {    .mode   =   9,
            .x1_drv  =   3,
            .y1_drv  =   3,
            .drv1    =   1,     
            .x2_drv  =   3,
            .y2_drv  =   3,
            .drv2    =   1,     
            .x3_drv  =   3,
            .y3_drv  =   3,
            .drv3    =   1, 
            .x4_drv  =   3,
            .y4_drv  =   3,
            .drv4    =   2, 
        },
        
        //  输出2个平分画面，左边为1号口，右边为2号口  对应遥控器的*键
        {   .mode   =   10,  
            .x1_drv  =   2,
            .y1_drv  =   2,
            .drv1    =   1,     
            .x2_drv  =   2,
            .y2_drv  =   2,
            .drv2    =   1,     
            .x3_drv  =   0,
            .y3_drv  =   0,
            .drv3    =   1, 
            .x4_drv  =   0,
            .y4_drv  =   0,
            .drv4    =   1,    
        },
        //  输出2个平分画面，左边为3号口，右边为4号口 对应遥控器的#键
        {   .mode   =   11,  
            .x1_drv  =   0,
            .y1_drv  =   0,  
            .drv1    =   1,     
            .x2_drv  =   0,
            .y2_drv  =   0,
            .drv2    =   1,     
            .x3_drv  =   2,
            .y3_drv  =   2,
            .drv3    =   1, 
            .x4_drv  =   2,
            .y4_drv  =   2,
            .drv4    =   1,
        },
         //  输出1大2小画面，左边大屏为1号口，右上小屏为2号口，右下小屏为3号口
         {   .mode   =   12,  
            .x1_drv  =   2,
            .y1_drv  =   1,  
            .drv1    =   1,     
            .x2_drv  =   2,
            .y2_drv  =   2,
            .drv2    =   1,     
            .x3_drv  =   2,
            .y3_drv  =   2,
            .drv3    =   1, 
            .x4_drv  =   0,
            .y4_drv  =   0,
            .drv4    =   1,
         },
         //  输出1大2小画面，左边大屏为2号口，右上小屏为1号口，右下小屏为3号口
         {   .mode   =   13,  
            .x1_drv  =   2,
            .y1_drv  =   2,  
            .drv1    =   1,     
            .x2_drv  =   2,
            .y2_drv  =   1,
            .drv2    =   1,     
            .x3_drv  =   2,
            .y3_drv  =   2,
            .drv3    =   1, 
            .x4_drv  =   0,
            .y4_drv  =   0,
            .drv4    =   1,
         },
         //  输出1大2小画面，左边大屏为3号口，右上小屏为1号口，右下小屏为2号口
         {   .mode   =   14,  
            .x1_drv  =   2,
            .y1_drv  =   2,  
            .drv1    =   1,     
            .x2_drv  =   2,
            .y2_drv  =   2,
            .drv2    =   1,     
            .x3_drv  =   2,
            .y3_drv  =   1,
            .drv3    =   1, 
            .x4_drv  =   0,
            .y4_drv  =   0,
            .drv4    =   1,
         },
    };
    
    
    uint32_t temp;   
    uint16_t drv_x1_3;//drv_x1_1
    uint16_t drv_y1_3;//drv_y1_1
    uint16_t drv_x2_3;//drv_x2_1
    uint16_t drv_y2_3;//drv_y2_1
    uint16_t drv_x3_3;//drv_x3_1
    uint16_t drv_y3_3;//drv_y3_1
    uint16_t drv_x4_3;//drv_x4_1
    uint16_t drv_y4_3;//drv_y4_1
    static uint16_t out_last1=0;
    static uint16_t out_last2=0;
//    uint8_t mode_last;
    
    struct _FPGA_OUT_CONFIG const * out_pixel;  //  输出像素点时的配置信息
    double offset_temp;
    const uint8_t   mode_max    =   sizeof(config)/sizeof(config[0]);
    char str[100];
    static const uint32_t FACTOR_FRACTIONAL_BITS = 16;  //  张工给的固定值
    
//    if((mode>=mode_max) || ((gsv2011_1->in.width<10) || (gsv2011_1->in.height<10) || (gsv2011_1->in.width>8192) || (gsv2011_1->in.height>8192) )
//                        && ((gsv2011_2->in.width<10) || (gsv2011_2->in.height<10) || (gsv2011_2->in.width>8192) || (gsv2011_2->in.height>8192))
//                        && ((gsv2011_3->in.width<10) || (gsv2011_3->in.height<10) || (gsv2011_3->in.width>8192) || (gsv2011_3->in.height>8192))
//                        && ((gsv2011_4->in.width<10) || (gsv2011_4->in.height<10) || (gsv2011_4->in.width>8192) || (gsv2011_4->in.height>8192))){
//        return true;
//    }
 sprintf(str, "\r\n gsv2011_1->in.rate  %d \r\n  ",gsv2011_1->in.rate);
        BSP_USART_FUN.Write(str, strlen(str));
     //修复出现绿边
   
    for(int i=0; i<TABLE1_COUNT; i++){
        if((gsv2011_1->in.width == VideoTable0[i*3]) && (gsv2011_1->in.height == VideoTable0[i*3+1]) &&(gsv2011_1->in.rate == VideoTable0[i*3+2])&&(channel==1)){        
            
            found1=0;
//        sprintf(str, "\r\n--- found1  %d  gsv2011_1->in.width %d  %d  %d",found1,gsv2011_1->in.width,gsv2011_1->in.height,gsv2011_1->in.rate);
//        BSP_USART_FUN.Write(str, strlen(str));
            break;
           
        }
            
        if((gsv2011_2->in.width == VideoTable0[i*3]) && (gsv2011_2->in.height == VideoTable0[i*3+1]) && (gsv2011_2->in.rate == VideoTable0[i*3+2])&&(channel==2)){        
            found2=0;
            break;
           
        }
        if((gsv2011_3->in.width == VideoTable0[i*3]) && (gsv2011_3->in.height == VideoTable0[i*3+1]) && (gsv2011_3->in.rate == VideoTable0[i*3+2])&&(channel==3)){        
            found3=0;
            break;
        }
        if((gsv2011_4->in.width == VideoTable0[i*3]) && (gsv2011_4->in.height == VideoTable0[i*3+1]) && (gsv2011_4->in.rate == VideoTable0[i*3+2])&&(channel==4)){        
            found4=0;
            break;
           
        }

    }
        
//     sprintf(str, "\r\n out  %d   %d  mode %d   ",out_pixel->x,out_pixel->y,config[mode].mode);
//        BSP_USART_FUN.Write(str, strlen(str));
    //  输出的分辨率,先用默认输出的分辨率填充，可自行更改
    if(out==0){
        out_pixel = &OUT_2160P;
        out_last2=0;
        out_last1=0;
        if(mode ==5)
            return 1;
      
    }else if(out==1){
        out_pixel = &OUT_1080P;
        out_last2=0;
        out_last1=0;
        if(mode ==5)
            return 1;
        
    }else if(out==2){
        out_pixel = &OUT_3440P;
        out_last2=0;        
        if(mode >=6 && mode <=9)
        {       
            mode = mode; 
            out_last1=0;
        }
        else {
            mode=6;
        }
        if(out_last1==0)
            out_last1=1;
        else 
            return 1;              
    }else if(out==3){
        out_pixel = &OUT_2560P;
        if(mode==4||(mode>=12 && mode<=14)){
            mode=mode;
            out_last2=0;
        }
        else
            mode=4;
        out_last1=0;
        if(out_last2==0)
            out_last2=1;
        else
           return 1; 
    }
       
//    sprintf(str, "\r\n++out_pixel->fre +++-%f  ",out_pixel->fre);
//    BSP_USART_FUN.Write(str, strlen(str));
//  最后设置时钟频率
    BSP_GM5351A_FUN.Set(out_pixel->fre, 50, 1);    
    //  写入模式
    retu &= write_fpga_mode(0x01, config[mode].mode, 0xFFFF);    
    retu &= write_fpga_mode(0x02, out, 0xFFFF);                   //  输入视频源1的像素宽  如3840
    retu &= write_fpga_mode(0x04, gsv2011_1->in.width, 0xFFFF);                   //  输入视频源1的像素宽  如3840
    retu &= write_fpga_mode(0x05, gsv2011_1->in.height, 0xFFFF);                   //  输入视频的像素高    如 2160
    retu &= write_fpga_mode(0x06, gsv2011_2->in.width, 0xFFFF);                   //  输入视频源2的像素宽  如3840
    retu &= write_fpga_mode(0x07, gsv2011_2->in.height, 0xFFFF);                   //  输入视频的像素高    如 2160
    retu &= write_fpga_mode(0x08, gsv2011_3->in.width, 0xFFFF);                   //  输入视频源3的像素宽  如3840
    retu &= write_fpga_mode(0x09, gsv2011_3->in.height, 0xFFFF);                   //  输入视频的像素高    如 2160
    retu &= write_fpga_mode(0x0A, gsv2011_4->in.width, 0xFFFF);                   //  输入视频源4的像素宽  如3840
    retu &= write_fpga_mode(0x0B, gsv2011_4->in.height, 0xFFFF);                   //  输入视频的像素高    如 2160
    //retu &= write_fpga_mode(0x35, mode==8, 0xFFFF);             //  是否为 1080P 输出
   
    //  计算分割后的分辨率

    if(out_pixel->x==3440 &&out_pixel->y==1440){          
       if(config[mode].mode==6){
            drv_x1_3 = 2560;
            drv_y1_3 = 1440;
               
            drv_x2_3 =drv_x3_3=drv_x4_3=880;
            drv_y2_3 =drv_y3_3=drv_y4_3= 480;
       }else if(config[mode].mode==7){
            drv_x2_3 = 2560;
            drv_y2_3 = 1440;

            drv_x1_3 =drv_x3_3=drv_x4_3=880;
            drv_y1_3 =drv_y3_3=drv_y4_3= 480;
       }else if(config[mode].mode==8){
            drv_x3_3 = 2560;
            drv_y3_3 = 1440;

            drv_x1_3 =drv_x2_3=drv_x4_3=880;
            drv_y1_3 =drv_y2_3=drv_y4_3= 480;
       }else if(config[mode].mode==9){
            drv_x4_3 = 2560;
            drv_y4_3 = 1440;

            drv_x1_3=drv_x2_3 =drv_x3_3=880;
            drv_y1_3=drv_y2_3 =drv_y3_3= 480;
      
        
        }
        
    }else if(config[mode].mode==12||config[mode].mode==13||config[mode].mode==14){
        drv_x1_3 = (out_pixel->x/config[mode].x1_drv)*config[mode].drv1;
        drv_y1_3 = (out_pixel->y/config[mode].y1_drv)*config[mode].drv1;

        drv_x2_3 =(out_pixel->x/config[mode].x2_drv)*config[mode].drv2;
        drv_y2_3 = out_pixel->y/config[mode].y2_drv*config[mode].drv2;
        
        drv_x3_3 = out_pixel->x/config[mode].x3_drv*config[mode].drv3;
        drv_y3_3 = out_pixel->y/config[mode].y3_drv*config[mode].drv3;

        drv_x4_3 = out_pixel->x/config[mode].x4_drv*config[mode].drv4;
        drv_y4_3 = out_pixel->y/config[mode].y4_drv*config[mode].drv4;
        
//        sprintf(str, "\r\n out  %d   %d  mode %d   ",out_pixel->x,out_pixel->y,mode);
//        BSP_USART_FUN.Write(str, strlen(str));

    }
    else{
        drv_x1_3 = (out_pixel->x/config[mode].x1_drv)*config[mode].drv1;
        drv_y1_3 = (out_pixel->y/config[mode].y1_drv)*config[mode].drv1;

        drv_x2_3 =(out_pixel->x/config[mode].x2_drv)*config[mode].drv2;
        drv_y2_3 = out_pixel->y/config[mode].y2_drv*config[mode].drv2;
        
        drv_x3_3 = out_pixel->x/config[mode].x3_drv*config[mode].drv3;
        drv_y3_3 = out_pixel->y/config[mode].y3_drv*config[mode].drv3;

        drv_x4_3 = out_pixel->x/config[mode].x4_drv*config[mode].drv4;
        drv_y4_3 = out_pixel->y/config[mode].y4_drv*config[mode].drv4;
        
//        sprintf(str, "\r\n out  %d   %d  mode %d   ",out_pixel->x,out_pixel->y,mode);
//        BSP_USART_FUN.Write(str, strlen(str));

    }
    

     

#if 1
    if(channel==1){
        
//        sprintf(str, "\r\n 1  %d   %d ",gsv2011_1->in.width,gsv2011_1->in.height);
//        BSP_USART_FUN.Write(str, strlen(str));

        if((gsv2011_1->in.width<=1280 )&&(gsv2011_1->in.height<=720)){
            retu &= write_fpga_mode(0x15, 0, 0xFFFF);   // 
        }else{
            retu &= write_fpga_mode(0x15, 1, 0xFFFF);
        }
        
        temp=(gsv2011_1->in.width<<FACTOR_FRACTIONAL_BITS)/drv_x1_3;
        retu &= write_fpga_mode(0x1C, temp/0x10000, 0xFFFF);            //  xstep_x1_hbit    video scale  x  step  高16bit
        retu &= write_fpga_mode(0x1D, temp%0x10000, 0xFFFF);            //  xstep_x1_Lbit    video scale  x  step  低16bit 
        offset_temp = ((double)gsv2011_1->in.width)/drv_x1_3;
        temp=((1<<(FACTOR_FRACTIONAL_BITS-1))*offset_temp);
        temp=temp-(1<<(FACTOR_FRACTIONAL_BITS-1));
        retu &= write_fpga_mode(0x1E, temp/0x10000, 0xFFFF);            //  xoffset_x1_hbit    video scale  xoffset  高16bit
        retu &= write_fpga_mode(0x1F, temp%0x10000, 0xFFFF);            //  xoffset_x1_Lbit    video scale  xoffset  低16bit

        temp=(gsv2011_1->in.height<<FACTOR_FRACTIONAL_BITS)/drv_y1_3;
        retu &= write_fpga_mode(0x20, temp/0x10000, 0xFFFF);            //  ystep_x1_hbit    video scale  y  step  高16bit
        retu &= write_fpga_mode(0x21, temp%0x10000, 0xFFFF);            //  ystep_x1_Lbit    video scale  y  step  低16bit 
        offset_temp = ((double)gsv2011_1->in.height)/drv_y1_3;
        temp=((1<<(FACTOR_FRACTIONAL_BITS-1))*offset_temp);
        temp=temp-(1<<(FACTOR_FRACTIONAL_BITS-1));
        retu &= write_fpga_mode(0x22, temp/0x10000, 0xFFFF);            //  yoffset_x1_hbit    video scale  yoffset  高16bit
        retu &= write_fpga_mode(0x23, temp%0x10000, 0xFFFF);            //  yoffset_x1_Lbit    video scale  yoffset  低16bit

        rt_thread_delay(10);
        if(gsv2011_1->in.height<=1080||found1!=0){
//            sprintf(str, "\r\n 45  1   ");
//            BSP_USART_FUN.Write(str, strlen(str));
            retu &= write_fpga_mode(0x45, 1, 0xFFFF);   // 绿边   右
           
        }
        else{
             
//            sprintf(str, "\r\n 45  0   ");
//            BSP_USART_FUN.Write(str, strlen(str));
            retu &= write_fpga_mode(0x45, 0, 0xFFFF);   // 绿边   
            found1=1;
        }
        
        retu &= write_fpga_mode(0x0D, 0, 0xFFFF);   //  port1 rst      mcu复位FPGA     高有效，
        rt_thread_delay(10);
        retu &= write_fpga_mode(0x0D, 1, 0xFFFF);   //  port1 rst      mcu复位FPGA     高有效， 
        
//        sprintf(str, "\r\n 1 drv_x_3 +++-%d  drv_y_3  %d config[%d].drv1 %d",drv_x1_3,drv_y1_3,config[mode].mode ,config[mode].drv1);
//        BSP_USART_FUN.Write(str, strlen(str));
       
    }  
    if(channel==2){
//        sprintf(str, "\r\n  2  %d   %d ",gsv2011_2->in.width,gsv2011_2->in.height);
//        BSP_USART_FUN.Write(str, strlen(str));
        if((gsv2011_2->in.width<=1280)&&(gsv2011_2->in.height<=720)){
            retu &= write_fpga_mode(0x16, 0, 0xFFFF);    
        }else{
            retu &= write_fpga_mode(0x16, 1, 0xFFFF);
        }
        temp=(gsv2011_2->in.width<<FACTOR_FRACTIONAL_BITS)/drv_x2_3;
        retu &= write_fpga_mode(0x24, temp/0x10000, 0xFFFF);            //  xstep_x2_hbit    video scale  x  step  高16bit
        retu &= write_fpga_mode(0x25, temp%0x10000, 0xFFFF);            //  xstep_x2_Lbit    video scale  x  step  低16bit
        offset_temp = ((double)gsv2011_2->in.width)/drv_x2_3;
        temp=((1<<(FACTOR_FRACTIONAL_BITS-1))*offset_temp);
        temp=temp-(1<<(FACTOR_FRACTIONAL_BITS-1));
        retu &= write_fpga_mode(0x26, temp/0x10000, 0xFFFF);            //  xoffset_x2_hbit    video scale  xoffset  高16bit
        retu &= write_fpga_mode(0x27, temp%0x10000, 0xFFFF);            //  xoffset_x2_Lbit    video scale  xoffset  低16bit

        temp=(gsv2011_2->in.height<<FACTOR_FRACTIONAL_BITS)/drv_y2_3;
        retu &= write_fpga_mode(0x28, temp/0x10000, 0xFFFF);            //  ystep_x2_hbit    video scale  y  step  高16bit
        retu &= write_fpga_mode(0x29, temp%0x10000, 0xFFFF);            //  ystep_x2_Lbit    video scale  y  step  低16bit  
        offset_temp = ((double)gsv2011_2->in.height)/drv_y2_3;
        temp=((1<<(FACTOR_FRACTIONAL_BITS-1))*offset_temp);
        temp=temp-(1<<(FACTOR_FRACTIONAL_BITS-1));  
        retu &= write_fpga_mode(0x2A, temp/0x10000, 0xFFFF);            //  yoffset_x2_hbit    video scale  yoffset  高16bit
        retu &= write_fpga_mode(0x2B, temp%0x10000, 0xFFFF);            //  yoffset_x2_Lbit    video scale  yoffset  低16bit
        
        rt_thread_delay(10);
        if(gsv2011_2->in.height<=1080||found2!=0){
            retu &= write_fpga_mode(0x46, 1, 0xFFFF);   // 绿边   
            
        }
        else{
            retu &= write_fpga_mode(0x46, 0, 0xFFFF);   // 绿边 
            found2=1;
        }
        
        retu &= write_fpga_mode(0x0E, 0, 0xFFFF);   //  port2rst      mcu复位FPGA     高有效，
        rt_thread_delay(10);
        retu &= write_fpga_mode(0x0E, 1, 0xFFFF);   //  port2rst      mcu复位FPGA     高有效，
//        sprintf(str, "\r\n 2 drv_x2_3 +++-%d  drv_y2_3  %d config[%d].drv2 %d",drv_x2_3,drv_y2_3,mode ,config[mode].drv2);
//        BSP_USART_FUN.Write(str, strlen(str));
    }
    
    if(channel==3){
//        sprintf(str, "\r\n 3  %d   %d ",gsv2011_3->in.width,gsv2011_3->in.height);
//        BSP_USART_FUN.Write(str, strlen(str));
        if((gsv2011_3->in.width<=1280)&&(gsv2011_3->in.height<=720)){
            retu &= write_fpga_mode(0x17, 0, 0xFFFF);   // 
        }else{
            retu &= write_fpga_mode(0x17, 1, 0xFFFF);
        }
        
        temp=(gsv2011_3->in.width<<FACTOR_FRACTIONAL_BITS)/drv_x3_3;
        retu &= write_fpga_mode(0x2C, temp/0x10000, 0xFFFF);            //  xstep_x3_hbit    video scale  x  step  高16bit
        retu &= write_fpga_mode(0x2D, temp%0x10000, 0xFFFF);            //  xstep_x3_Lbit    video scale  x  step  低16bit
        offset_temp = ((double)gsv2011_3->in.width)/drv_x3_3;
        temp=((1<<(FACTOR_FRACTIONAL_BITS-1))*offset_temp);
        temp=temp-(1<<(FACTOR_FRACTIONAL_BITS-1));
        retu &= write_fpga_mode(0x2E, temp/0x10000, 0xFFFF);            //  xoffset_x3_hbit    video scale  xoffset  高16bit
        retu &= write_fpga_mode(0x2F, temp%0x10000, 0xFFFF);            //  xoffset_x3_Lbit    video scale  xoffset  低16bit

        temp=(gsv2011_3->in.height<<FACTOR_FRACTIONAL_BITS)/drv_y3_3;
        retu &= write_fpga_mode(0x30, temp/0x10000, 0xFFFF);            //  ystep_x3_hbit    video scale  y  step  高16bit
        retu &= write_fpga_mode(0x31, temp%0x10000, 0xFFFF);            //  ystep_x3_Lbit    video scale  y  step  低16bit  
        offset_temp = ((double)gsv2011_3->in.height)/drv_y3_3;
        temp=((1<<(FACTOR_FRACTIONAL_BITS-1))*offset_temp);
        temp=temp-(1<<(FACTOR_FRACTIONAL_BITS-1));  
        retu &= write_fpga_mode(0x32, temp/0x10000, 0xFFFF);            //  yoffset_x3_hbit    video scale  yoffset  高16bit
        retu &= write_fpga_mode(0x33, temp%0x10000, 0xFFFF);            //  yoffset_x3_Lbit    video scale  yoffset  低16bit
        
        rt_thread_delay(10);
        if(gsv2011_3->in.height<=1080||found3!=0){
            retu &= write_fpga_mode(0x47, 1, 0xFFFF);   // 绿边   
            
        }
        else{
            retu &= write_fpga_mode(0x47, 0, 0xFFFF);   // 绿边 
            found3=1;
        }
        
        retu &= write_fpga_mode(0x0F, 0, 0xFFFF);   //  port3rst      mcu复位FPGA     高有效，
        rt_thread_delay(10);
        retu &= write_fpga_mode(0x0F, 1, 0xFFFF);   //  port3rst      mcu复位FPGA     高有效，
//        sprintf(str, "\r\n 3 drv_x3_3 +++-%d  drv_y3_3  %d config[%d].drv3 %d",drv_x3_3,drv_y3_3,mode ,config[mode].drv3);
//        BSP_USART_FUN.Write(str, strlen(str));
    
    }
    if(channel==4){
//        sprintf(str, "\r\n  4 %d   %d ",gsv2011_4->in.width,gsv2011_4->in.height);
//        BSP_USART_FUN.Write(str, strlen(str));
        
        if((gsv2011_4->in.width<=1280)&&(gsv2011_4->in.height<=720)){
            retu &= write_fpga_mode(0x18, 0, 0xFFFF);   // 
        }else{
            retu &= write_fpga_mode(0x18, 1, 0xFFFF);
        }
        
        temp=(gsv2011_4->in.width<<FACTOR_FRACTIONAL_BITS)/drv_x4_3;
        retu &= write_fpga_mode(0x34, temp/0x10000, 0xFFFF);            //  xstep_x4_hbit    video scale  x  step  高16bit
        retu &= write_fpga_mode(0x35, temp%0x10000, 0xFFFF);            //  xstep_x4_Lbit    video scale  x  step  低16bit
        offset_temp = ((double)gsv2011_4->in.width)/drv_x4_3;
        temp=((1<<(FACTOR_FRACTIONAL_BITS-1))*offset_temp);
        temp=temp-(1<<(FACTOR_FRACTIONAL_BITS-1));
        retu &= write_fpga_mode(0x36, temp/0x10000, 0xFFFF);            //  xoffset_x4_hbit    video scale  xoffset  高16bit
        retu &= write_fpga_mode(0x37, temp%0x10000, 0xFFFF);            //  xoffset_x4_Lbit    video scale  xoffset  低16bit

        temp=(gsv2011_4->in.height<<FACTOR_FRACTIONAL_BITS)/drv_y4_3;
        retu &= write_fpga_mode(0x38, temp/0x10000, 0xFFFF);            //  ystep_x4_hbit    video scale  y  step  高16bit
        retu &= write_fpga_mode(0x39, temp%0x10000, 0xFFFF);            //  ystep_x4_Lbit    video scale  y  step  低16bit  
        offset_temp = ((double)gsv2011_4->in.height)/drv_y4_3;
        temp=((1<<(FACTOR_FRACTIONAL_BITS-1))*offset_temp);
        temp=temp-(1<<(FACTOR_FRACTIONAL_BITS-1));  
        retu &= write_fpga_mode(0x3A, temp/0x10000, 0xFFFF);            //  yoffset_x4_hbit    video scale  yoffset  高16bit
        retu &= write_fpga_mode(0x3B, temp%0x10000, 0xFFFF);            //  yoffset_x4_Lbit    video scale  yoffset  低16bit
        
        rt_thread_delay(10);
        if(gsv2011_4->in.height<=1080||found4!=0){
            
            
            retu &= write_fpga_mode(0x48, 1, 0xFFFF);   // 绿边   
            
        }
        else{
            found4=1;
            retu &= write_fpga_mode(0x48, 0, 0xFFFF);   // 绿边 
        }
        
        retu &= write_fpga_mode(0x10, 0, 0xFFFF);   //  port4rst      mcu复位FPGA     高有效，
        rt_thread_delay(10);
        retu &= write_fpga_mode(0x10, 1, 0xFFFF);   //  port4rst      mcu复位FPGA     高有效，
        
//        sprintf(str, "\r\n 4 drv_x4_3 +++-%d  drv_y4_3  %d config[%d].drv4 %d",drv_x4_3,drv_y4_3,mode ,config[mode].drv4);
//        BSP_USART_FUN.Write(str, strlen(str));
    }
   
#endif

   //  配置结束，发送复位信号
    if((gsv2011_1->in.width==0&&gsv2011_1->in.height==0)||gsv2011_1->connect==0){
//        sprintf(str, "\r\n+++++++++++++++ 1 复位信号 +++-%d  height  %d ",gsv2011_1->in.width,gsv2011_1->in.height);
//        BSP_USART_FUN.Write(str, strlen(str));
        retu &= write_fpga_mode(0x0D, 0, 0xFFFF);   //  port1 rst      mcu复位FPGA     高有效，
    }   
    if((gsv2011_2->in.width==0&&gsv2011_2->in.height==0)||gsv2011_2->connect==0){
//        sprintf(str, "\r\n+++++++++++++++ 2 复位信号 +++-%d  height  %d ",gsv2011_2->in.width,gsv2011_2->in.height);
//        BSP_USART_FUN.Write(str, strlen(str));
        retu &= write_fpga_mode(0x0E, 0, 0xFFFF);   //  port1 rst      mcu复位FPGA     高有效，
    }  
    if((gsv2011_3->in.width==0&&gsv2011_3->in.height==0)||gsv2011_3->connect==0){
//        sprintf(str, "\r\n+++++++++++++++ 3 复位信号 +++-%d  height  %d ",gsv2011_3->in.width,gsv2011_3->in.height);
//        BSP_USART_FUN.Write(str, strlen(str));
        retu &= write_fpga_mode(0x0F, 0, 0xFFFF);   //  port1 rst      mcu复位FPGA     高有效，
    }
    if((gsv2011_4->in.width==0&&gsv2011_4->in.height==0)||gsv2011_4->connect==0){
//        sprintf(str, "\r\n+++++++++++++++ 4 复位信号 +++-%d  height  %d ",gsv2011_4->in.width,gsv2011_4->in.height);
//        BSP_USART_FUN.Write(str, strlen(str));
        retu &= write_fpga_mode(0x10, 0, 0xFFFF);   //  port1 rst      mcu复位FPGA     高有效，
    }
    return retu;
    
//    return retu;
}

static void function_thread(void * pragram)
{
     bool retu=true;;
    APP_FPGA_DATA_Typedefstruct app;
    uint32_t config_tick;
    uint32_t sate_tick;
    uint8_t FPGA_NO_CONFIG;
//    char str[100]; 
    APP_NEC_DATA_Typedefstruct nec;
    APP_KEY_DATA_Typedefstruct key;
    APP_USART_DATA_Typedefstruct usart_data;
    APP_SOFT_USART_DATA_Typedefstruct soft_usart_data;
    APP_FLASH_DATA_Typedefstruct flash;
    extern rt_mq_t APP_USART_QUEUE_RX;
    extern rt_mq_t APP_USART_QUEUE_TX;
    
    struct{
        APP_GSV2011_DATA_TYPEDEFSTRUCT gsv2011;
        
        APP_GSV2011_DATA_TYPEDEFSTRUCT gsv2011_1;
        APP_GSV2011_DATA_TYPEDEFSTRUCT gsv2011_2;
        APP_GSV2011_DATA_TYPEDEFSTRUCT gsv2011_3;
        APP_GSV2011_DATA_TYPEDEFSTRUCT gsv2011_4;
        
        uint8_t mode;
        uint8_t turn;
        uint8_t out;
    }config,last_config;
    
    BSP_SPI_FUN.Init();
    BSP_GM5351A_FUN.Init();
    BSP_GM5351A_FUN.Set(148.5, 50, 1);
    
    //  初始化参数
    memset(&app, 0, sizeof(app));
    memset(&config, 0, sizeof(config));
    memset(&last_config, 0, sizeof(last_config));
    FPGA_NO_CONFIG = 0;
    sate_tick = rt_tick_get();
    FUN_WRITE(&app);
    
    //  发布自身任务启动的信号
    rt_event_send(event_handle, APP_EVENT_START);
    
    //  等待别的任务启动
    APP_NEC_FUN.Event(APP_EVENT_START);
    APP_KEY_FUN.Event(APP_EVENT_START);
    APP_USART_FUN.Event(APP_EVENT_START);
    APP_FLASH_FUN.Event(APP_EVENT_START);
    APP_SOFT_USART_FUN.Event(APP_EVENT_START);
    
    //  尝试配置FPGA，直至FPGA成功运行（就是向FPGA写入固件，FPGA断电后丢失固件，所以每次上电后都要写入一次固件）
    FPGA_INIT_AGAIN:
    BSP_FPGA_FUN.Init();
    config_tick = rt_tick_get();
    while(BSP_FPGA_FUN.CFG_DONE() != true){
        if((rt_tick_get()-config_tick)>20000){
            BSP_FPGA_FUN.Init();
            config_tick = rt_tick_get();
            FPGA_NO_CONFIG=1;
        }
        rt_thread_delay(100);
    }
    retu &= write_fpga_mode(0x0C, 0, 0xFFFF);
    rt_thread_delay(10);
    retu &= write_fpga_mode(0x0C, 1, 0xFFFF);   //  rst      mcu复位FPGA     高有效，
    
    //  读取FLASH的状态，用来设置FPGA
    APP_FLASH_FUN.Read(&flash);
    app.nec.mode = flash.NEC_MODE;
    app.nec.turn = flash.NEC_TURN;
    app.nec.out = flash.NEC_OUT;
    app.usart.mode = flash.USART_MODE;
    app.usart.turn = flash.USART_TURN;
    config.mode = app.nec.mode;
    config.turn = app.nec.turn;
    config.out = app.nec.out;
    
    rt_kprintf("\r\n   nec.mode  %d   ", app.nec.mode);
    rt_kprintf("\r\n   nec.turn  %d   ", app.nec.turn);
    rt_kprintf("\r\n   nec.out   %d   ", app.nec.out);
    rt_kprintf("\r\n usart.mode  %d   ", app.usart.mode);
    rt_kprintf("\r\n usart.turn  %d   ", app.usart.turn);


    while(1){
         rt_thread_delay(10);
        //  检测到FPGA异常，重新配置FPGA的固件
//        if(BSP_FPGA_FUN.CFG_DONE() != true){
//            goto FPGA_INIT_AGAIN;
//        }
        
        APP_NEC_FUN.Read(&nec);
        APP_KEY_FUN.Read(&key);
        APP_USART_FUN.Read(&usart_data);
        APP_SOFT_USART_FUN.Read(&soft_usart_data);
        APP_GSV2011_FUN0.Read(&config.gsv2011);
        
        APP_GSV2011_FUN1.Read(&config.gsv2011_1);
        APP_GSV2011_FUN2.Read(&config.gsv2011_2);
        APP_GSV2011_FUN3.Read(&config.gsv2011_3);
        APP_GSV2011_FUN4.Read(&config.gsv2011_4);
        
        if((rt_tick_get()-sate_tick)>500){
            //  定时和FPGA通信，保障单片机和FPGA之间连接可靠
            sate_tick = rt_tick_get();
            
        }
        if(nec.activate){
            if(nec.mode != 255){
                config.mode = nec.mode;
                app.nec.mode = nec.mode;
                app.nec.rx_data = 1;
            }
            if(nec.turn != 255){
                config.turn = nec.turn;
                app.nec.turn = nec.turn;
                app.nec.rx_data = 1;
            }
            if(nec.out != 255){
                config.out = nec.out;
                app.nec.out = nec.out;
                app.nec.rx_data = 1;
            }
            APP_NEC_ClearActivate(&nec);
            // FUN_WRITE(&app);
        }
        /* 软件串口独立处理，始终可用 */
        if(soft_usart_data.activate)
        {
            if(soft_usart_data.mode != 255)
            {
                config.mode = soft_usart_data.mode;

                app.usart.mode = soft_usart_data.mode;
               // FUN_WRITE(&app);
            }
           APP_SOFT_USART_ClearActivate(&soft_usart_data);
        }
        if(key.mode <= 14){
            //  DIP switch directly selects mode; 1111 enables IR/USART control
            config.mode = key.mode;
            config.out = 0;
            config.turn = 0;
        }
            //  配置参数发生了变化
            if( (last_config.mode != config.mode)
            ||  (last_config.out != config.out)
//            ||  (last_config.gsv2011.in.width != config.gsv2011.in.width)
//            ||  (last_config.gsv2011.in.height!= config.gsv2011.in.height)
            ||  (last_config.gsv2011_1.connect  != config.gsv2011_1.connect)
            ||  (last_config.gsv2011_1.in.width  != config.gsv2011_1.in.width)
            ||  (last_config.gsv2011_1.in.height != config.gsv2011_1.in.height)   
            ||  (last_config.gsv2011_1.in.rate != config.gsv2011_1.in.rate)  
            ||  (FPGA_NO_CONFIG)    ){
                  if(false == set_fpga_mode(config.mode,config.out,&config.gsv2011_1,&config.gsv2011_2,&config.gsv2011_3,&config.gsv2011_4,1)){
                //if(false == set_fpga_mode(config.mode, config.gsv2011.in.width, config.gsv2011.in.height, config.out?&OUT_2160P:&OUT_1080P)){
                    goto FPGA_INIT_AGAIN;
                }
            }
            
               //  配置参数发生了变化
            if( (last_config.mode != config.mode)
            ||  (last_config.out != config.out)
            ||  (last_config.gsv2011_2.connect  != config.gsv2011_2.connect)
            ||  (last_config.gsv2011_2.in.width  != config.gsv2011_2.in.width)
            ||  (last_config.gsv2011_2.in.height != config.gsv2011_2.in.height)
            ||  (last_config.gsv2011_2.in.rate != config.gsv2011_2.in.rate)  
            ||  (FPGA_NO_CONFIG)    ){
                  if(false == set_fpga_mode(config.mode,config.out,&config.gsv2011_1,&config.gsv2011_2,&config.gsv2011_3,&config.gsv2011_4,2)){
                //if(false == set_fpga_mode(config.mode, config.gsv2011.in.width, config.gsv2011.in.height, config.out?&OUT_2160P:&OUT_1080P)){
                    goto FPGA_INIT_AGAIN;
                }
            }
            
               //  配置参数发生了变化
            if( (last_config.mode != config.mode)
            ||  (last_config.out != config.out)
            ||  (last_config.gsv2011_3.connect  != config.gsv2011_3.connect)
            ||  (last_config.gsv2011_3.in.width  != config.gsv2011_3.in.width)
            ||  (last_config.gsv2011_3.in.height != config.gsv2011_3.in.height)
            ||  (last_config.gsv2011_3.in.rate != config.gsv2011_3.in.rate)  
            ||  (FPGA_NO_CONFIG)    ){
                  if(false == set_fpga_mode(config.mode,config.out,&config.gsv2011_1,&config.gsv2011_2,&config.gsv2011_3,&config.gsv2011_4,3)){
                //if(false == set_fpga_mode(config.mode, config.gsv2011.in.width, config.gsv2011.in.height, config.out?&OUT_2160P:&OUT_1080P)){
                    goto FPGA_INIT_AGAIN;
                }
            }
            
               //  配置参数发生了变化
            if( (last_config.mode != config.mode)
            ||  (last_config.out != config.out)
            ||  (last_config.gsv2011_4.connect  != config.gsv2011_4.connect)
            ||  (last_config.gsv2011_4.in.width  != config.gsv2011_4.in.width)
            ||  (last_config.gsv2011_4.in.height != config.gsv2011_4.in.height)
            ||  (last_config.gsv2011_4.in.rate != config.gsv2011_4.in.rate)  
            ||  (FPGA_NO_CONFIG)    ){
                  if(false == set_fpga_mode(config.mode,config.out,&config.gsv2011_1,&config.gsv2011_2,&config.gsv2011_3,&config.gsv2011_4,4)){
                //if(false == set_fpga_mode(config.mode, config.gsv2011.in.width, config.gsv2011.in.height, config.out?&OUT_2160P:&OUT_1080P,4)){
                    goto FPGA_INIT_AGAIN;
                }
            }

//            if((last_config.turn != config.turn) || (FPGA_NO_CONFIG)){
//                write_fpga_mode(0x07, config.turn?1:0, 0xFFFF);
//                write_fpga_mode(0x08, config.turn?1:0, 0xFFFF);
//            }
            
            FPGA_NO_CONFIG=0;
            memcpy(&last_config, &config, sizeof(config));
        }
       
        //  串口向FPGA写入数据
        write_fpga_reg_from_usart();
        
   }


