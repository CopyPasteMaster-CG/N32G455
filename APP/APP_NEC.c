


#include "APP_NEC.h"
#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "BSP_NEC.h"


/*******************任务配置开始*************************************************/
static APP_NEC_DATA_Typedefstruct app_data;    //  本任务与其它任务共享的数据
static rt_thread_t handle = RT_NULL;            //  任务句柄
static rt_event_t event_handle = RT_NULL;       //  任务对应的事件句柄，事件已经全部定义完成，不要更改
static rt_mutex_t mutex_handle = RT_NULL;       //  任务共享数据用到的互斥量句柄
static void function_thread(void * pragram);    //  任务对应的函数

//  向共享内存，写入数据，本任务专用
static inline void FUN_WRITE(APP_NEC_DATA_Typedefstruct * data)
{
    rt_mutex_take(mutex_handle, RT_TICK_MAX);
    memcpy(&app_data, data, sizeof(APP_NEC_DATA_Typedefstruct));
    rt_mutex_release(mutex_handle);
}


void APP_NEC_ClearActivate(APP_NEC_DATA_Typedefstruct *data)
{
    if(data == RT_NULL){
        return;
    }

    data->activate = false;
    FUN_WRITE(data);
}

static int FUN_READ(void * data)
{
    //  判断互斥量的句柄是否被定义
    while(mutex_handle == RT_NULL){
        rt_thread_delay(1000);
    }
    rt_mutex_take(mutex_handle, RT_TICK_MAX);
    memcpy(data, &app_data, sizeof(APP_NEC_DATA_Typedefstruct));
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
        handle = rt_thread_create(  "NEC",            /*  线程名字*/                    
                                    function_thread,    /*  线程入口函数*/
                                    NULL,               /*  线程入口函数参数*/
                                    4096,               /*  线程栈大小*/
                                    3 ,                 /*  线程优先级*/
                                    20);                /*  线程时间片*/
    }
    if(event_handle == RT_NULL){
        event_handle = rt_event_create( "NEC", RT_IPC_FLAG_PRIO );
    }
    if(mutex_handle == RT_NULL){
        mutex_handle = rt_mutex_create( "NEC", RT_IPC_FLAG_PRIO);														
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

const APP_HANDLE_typedefstruct APP_NEC_FUN={
    .Create = FUN_CREATE,
    .Read = FUN_READ,
    .Event = FUN_EVENT,
};
/*******************任务配置结束*************************************************/

/*******************以下全部是个性化自定义任务逻辑*************************************************/
#define NEC_RECEIVE_BIT_MAX  (34)      //  NEC一次性最多接受34个载波信号，NEC通信时，以是否发送载波作为信号调制的基本方式，单次载波发送的时间长短和两次载波之间的时间间隔作为基本单位，所以载波信号的个数至关重要


static volatile struct{
    uint16_t buf[NEC_RECEIVE_BIT_MAX*2];    //  发出一次载波信号，需要记录两次时间，开始时间和结束时间，记录时间时，按照时间轴记录
    
    uint8_t cnt;
    rt_tick_t tick;
}NEC_DATA;
static void NEC_RECEIVE_DATA(uint16_t start, uint16_t stop)
{
    if(NEC_DATA.cnt<NEC_RECEIVE_BIT_MAX){
        NEC_DATA.buf[NEC_DATA.cnt*2+0] = start;
        NEC_DATA.buf[NEC_DATA.cnt*2+1] = stop;
    }else{
        NEC_DATA.cnt=NEC_RECEIVE_BIT_MAX;
    }
    NEC_DATA.cnt++;
    NEC_DATA.tick=rt_tick_get();
}

static const double tolerance = 0.2;
static const uint32_t NEC_AGAIN         =   110*4;  //  重复码的发送周期为110ms，超过周期接收不到重复码，判定重复码无效
static const uint32_t NEC_TICK_MAX      =   65536;  //  最大时间计数
static const uint32_t NEC_AGC_PULSE     =   9000;   //  每帧报文的起始位置（数据帧、重复码 共用）
static const uint32_t NEC_LONG_PAUSE    =   4500;   //  数据帧报文标志（数据帧专用）
static const uint32_t NEC_SHORT_PAUSE   =   2250;   //  重复码报文标志（重复码专用）
static const uint32_t NEC_BIT_START     =   560;    //  bit开始标志
static const uint32_t NEC_BIT_1         =   1680;   //  bit1结束标志
static const uint32_t NEC_BIT_0         =   560;    //  bit0结束标志
static const uint32_t NEC_STOP          =   560;    //  结束标志
/*
    报文分为两种，一种是数据帧，另一种是重复码

    数据帧
    1、AGC_PULSE，    发送载波，反映到信号总线上是低电平
    2、LONG_PAUSE，   暂停载波，反映到信号总线上是高电平
    3、bit，          连续发送32个bit，对应4个字节，LSB的bit顺序
                        byte-1是地址
                        byte-2是地址反码
                        byte-3是数据
                        byte-4是数据反码
    4、STOP，         发送载波，反映到信号总线上是低电平
    
    其中，bit的发送方式分为两种
    bit0：先发BIT_START（发送载波），然后发送BIT_0（暂停载波）
    bit1：先发BIT_START（发送载波），然后发送BIT_1（暂停载波）
    
    重复码
    1、AGC_PULSE，    发送载波，反映到信号总线上是低电平
    2、SHORT_PAUSE   暂停载波，反映到信号总线上是高电平
    3、STOP，         发送载波，反映到信号总线上是低电平

*/


static inline char nec_get_byte(volatile uint16_t *buf, uint8_t *data)
{
    uint8_t code=0;
    uint16_t temp;
    for(uint8_t i=0; i<8; i++){
        temp=(NEC_TICK_MAX + buf[i*2+1] - buf[i*2])%NEC_TICK_MAX;
        if(((NEC_BIT_START*(1-tolerance))<temp) && (temp<(NEC_BIT_START*(1+tolerance)))){
            //  起始校验通过，然后校验bit类型
            temp=(NEC_TICK_MAX + buf[i*2+2] - buf[i*2+1])%NEC_TICK_MAX;
            if(((NEC_BIT_1*(1-tolerance))<temp) && (temp<(NEC_BIT_1*(1+tolerance)))){
                code|=(0x01<<i);
            }else if(((NEC_BIT_0*(1-tolerance))<temp) && (temp<(NEC_BIT_0*(1+tolerance)))){
                code&=(~(0x01<<i));
            }else {
                return 0;
            }
        }else{
            return 0;
        }
    }
    *data=code;
    return 1;
}

rt_mq_t APP_NEC_QUEUE_RX = RT_NULL;
static void function_thread(void * pragram)
{
    APP_NEC_DATA_Typedefstruct app;
    uint32_t temp;
    uint32_t value;
    uint8_t  have_data=0;
    rt_tick_t rx_tick;
    uint8_t buf[4];
    
    BSP_NEC_FUN.Init(NEC_RECEIVE_DATA);
    
    buf[0]=0;
    buf[1]=0;
    buf[2]=0;
    buf[3]=0;
    
    while(APP_NEC_QUEUE_RX == RT_NULL){
        rt_thread_delay(1);
        APP_NEC_QUEUE_RX = rt_mq_create(  "NEC_RX"
                                        ,   sizeof(APP_NEC_DATA_Typedefstruct)
                                        ,   1
                                        ,   RT_IPC_FLAG_FIFO);
    }
    
    
    //  发布自身任务启动的信号
    memset(&app, 0, sizeof(app));
    app.mode=255;
    app.turn=255;
    FUN_WRITE(&app);
    rt_event_send(event_handle, APP_EVENT_START);
    
    //  等待别的任务启动
    
    
    
    while(1){
        
        
        if(NEC_DATA.cnt){
            if((rt_tick_get()-NEC_DATA.tick)>15){
                if(NEC_DATA.cnt==2){
                    //  重复码
                    temp=(NEC_TICK_MAX+NEC_DATA.buf[1]-NEC_DATA.buf[0])%NEC_TICK_MAX;
                    value=NEC_AGC_PULSE;
                    if((temp>((1-tolerance)*value)) && (temp<((1+tolerance)*value))){
                        //  AGC_PULSE校验通过
                        temp=(NEC_TICK_MAX+NEC_DATA.buf[2]-NEC_DATA.buf[1])%NEC_TICK_MAX;
                        value=NEC_SHORT_PAUSE;
                        if((temp>((1-tolerance)*value)) && (temp<((1+tolerance)*value))){
                            //  SHORT_PAUSE校验通过
                            temp=(NEC_TICK_MAX+NEC_DATA.buf[3]-NEC_DATA.buf[2])%NEC_TICK_MAX;
                            value=NEC_STOP;
                            if((temp>((1-tolerance)*value)) && (temp<((1+tolerance)*value))){
                                //  STOP校验通过
                                
                                //  重复码校验通过，根据上一次是否接收到键值决定是否发出
                                if(have_data){
//                                    FUN_WRITE(&app);
                                }
                            }
                        }
                    }
                }else 
                if(NEC_DATA.cnt==34){
                    //  数据帧解码
                    temp=(NEC_TICK_MAX+NEC_DATA.buf[1]-NEC_DATA.buf[0])%NEC_TICK_MAX;
                    value=NEC_AGC_PULSE;
                    if((temp>((1-tolerance)*value)) && (temp<((1+tolerance)*value))){
                        //  AGC_PULSE校验通过
                        temp=(NEC_TICK_MAX+NEC_DATA.buf[2]-NEC_DATA.buf[1])%NEC_TICK_MAX;
                        value=NEC_LONG_PAUSE;
                        if((temp>((1-tolerance)*value)) && (temp<((1+tolerance)*value))){
                            //  LONG_PAUSE校验通过
                            if( nec_get_byte(&NEC_DATA.buf[2+16*0], &buf[0])
                            &&  nec_get_byte(&NEC_DATA.buf[2+16*1], &buf[1])
                            &&  nec_get_byte(&NEC_DATA.buf[2+16*2], &buf[2])
                            &&  nec_get_byte(&NEC_DATA.buf[2+16*3], &buf[3])){
                                //  数据接收校验通过
                                temp=(NEC_TICK_MAX+NEC_DATA.buf[3]-NEC_DATA.buf[2])%NEC_TICK_MAX;
                                value=NEC_STOP;
                                if((temp>((1-tolerance)*value)) && (temp<((1+tolerance)*value))){
                                    //  STOP校验通过
                                    
                                    rt_kprintf("\r\nNEC receive data %02X   ", buf[2]);
                                    app.mode=255;
                                    app.turn=255;
                                    app.out=255;
                                    if(buf[2]==0x19){
                                        rt_kprintf("0");
                                        app.mode=0;
                                    }else if(buf[2]==0x45){
                                        rt_kprintf("1");
                                        app.mode=1;
                                    }else if(buf[2]==0x46){
                                        rt_kprintf("2");
                                        app.mode=2;
                                    }else if(buf[2]==0x47){
                                        rt_kprintf("3");
                                        app.mode=3;
                                    }else if(buf[2]==0x44){
                                        rt_kprintf("4");
                                        app.mode=4;
                                    }else if(buf[2]==0x40){
                                        rt_kprintf("5");
                                        //app.mode=5;
                                        app.mode=12;
                                    }else if(buf[2]==0x43){
                                        rt_kprintf("6");
                                        app.mode=6;
                                    }else if(buf[2]==0x07){
                                        rt_kprintf("7");
                                        app.mode=7;
                                    }else if(buf[2]==0x15){
                                        rt_kprintf("8");
                                        app.mode=8;
                                    }else if(buf[2]==0x09){
                                        rt_kprintf("9");
                                        app.mode=9;
                                    }else if(buf[2]==0x16){
                                        //app.turn=1;
                                        app.mode=10;
                                        rt_kprintf("*");
                                    }else if(buf[2]==0x0D){
//                                        app.turn=0;
                                        app.mode=11;
                                        rt_kprintf("#");
                                    }else if(buf[2]==0x18){
                                        rt_kprintf("up");
                                        
                                         app.out=3;
                                    }else if(buf[2]==0x52){
                                        rt_kprintf("down");
                                        app.mode=13;
//                                         app.out=2;
                                    }else if(buf[2]==0x08){
                                        rt_kprintf("left");
                                        app.out=1;
                                    }else if(buf[2]==0x5A){
                                        app.out=0;
                                        rt_kprintf("right");
                                    }else if(buf[2]==0x1C){
                                        rt_kprintf("ok");
                                        app.mode=14;
                                    }else{
                                        rt_kprintf("UnKnow-key");
                                    }
                                    
                                    if((app.turn!=255) || (app.mode!=255) || (app.out!=255)){
                                        app.activate=true;
                                        FUN_WRITE(&app);
                                        if(app.mode != 255){
                                            APP_SOFT_USART_SendSetMode(app.mode);
                                        }
                                        have_data=1;
                                        rx_tick=rt_tick_get();
                                    }else{
                                        have_data=0;
                                    }
                                }
                            }
                        }
                    }
                
                }
                NEC_DATA.cnt=0;
            }
        }
        
        if((rt_tick_get()-rx_tick)>NEC_AGAIN){
            have_data=0;
            rx_tick=rt_tick_get();  
        }
        
        rt_thread_delay(10);
    }
}
