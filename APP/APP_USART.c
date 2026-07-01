
#include "APP_USART.h"
#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "BSP_USART.h"


/*******************任务配置开始*************************************************/
static APP_USART_DATA_Typedefstruct app_data;    //  本任务与其它任务共享的数据
static rt_thread_t handle = RT_NULL;            //  任务句柄
static rt_event_t event_handle = RT_NULL;       //  任务对应的事件句柄，事件已经全部定义完成，不要更改
static rt_mutex_t mutex_handle = RT_NULL;       //  任务共享数据用到的互斥量句柄
static void function_thread(void * pragram);    //  任务对应的函数

//  向共享内存，写入数据，本任务专用
static inline void FUN_WRITE(APP_USART_DATA_Typedefstruct * data)
{
    rt_mutex_take(mutex_handle, RT_TICK_MAX);
    memcpy(&app_data, data, sizeof(APP_USART_DATA_Typedefstruct));
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
    memcpy(data, &app_data, sizeof(APP_USART_DATA_Typedefstruct));
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
        handle = rt_thread_create(  "USART",            /*  线程名字*/                    
                                    function_thread,    /*  线程入口函数*/
                                    NULL,               /*  线程入口函数参数*/
                                    4096,               /*  线程栈大小*/
                                    3 ,                 /*  线程优先级*/
                                    20);                /*  线程时间片*/
    }
    if(event_handle == RT_NULL){
        event_handle = rt_event_create( "USART", RT_IPC_FLAG_PRIO );
    }
    if(mutex_handle == RT_NULL){
        mutex_handle = rt_mutex_create( "USART", RT_IPC_FLAG_PRIO);														
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

const APP_HANDLE_typedefstruct APP_USART_FUN={
    .Create = FUN_CREATE,
    .Read = FUN_READ,
    .Event = FUN_EVENT,
};
/*******************任务配置结束*************************************************/

/*******************以下全部是个性化自定义任务逻辑*************************************************/
static inline uint8_t char_to_hex(char c)
{
    if(('0'<=c) && (c<='9')){
        return c-'0';
    }else if(('a'<=c) && (c<='f')){
        return 10+c-'a';
    }else if(('A'<=c) && (c<='F')){
        return 10+c-'A';
    }else{
        return 255;
    }
    
}
static inline uint8_t GET_BYTE(void * source, uint8_t * back_data)
{
    char * p = source;
    uint8_t MSB,LSB;
    MSB=char_to_hex(p[0]);
    LSB=char_to_hex(p[1]);
    if((MSB>0x0F) || (LSB>0x0F)){
        return 0;
    }else{
        *back_data=MSB*0x10+LSB;
        return 1;
    }
}
static inline uint8_t GET_WORD(void * source, uint16_t * back_data)
{
    char * p = source;
    uint8_t bit0_7,bit8_15,bit16_23,bit24_31;
    bit24_31    =   char_to_hex(p[0]);
    bit16_23    =   char_to_hex(p[1]);
    bit8_15     =   char_to_hex(p[2]);
    bit0_7      =   char_to_hex(p[3]);
    if((bit24_31>0x0F) || (bit16_23>0x0F) || (bit8_15>0x0F) || (bit0_7>0x0F)){
        return 0;
    }else{
        *back_data=bit24_31*0x1000+bit16_23*0x0100+bit8_15*0x0010+bit0_7*0x0001;
        return 1;
    }
}

static struct{
    uint8_t buf[100];
    uint8_t len;
    rt_tick_t tick;
}UART_RX;
static void USART_RECEIVE_DATA(uint8_t data)
{
    if(UART_RX.len<sizeof(UART_RX.buf)){
        UART_RX.buf[UART_RX.len]=data;
        UART_RX.len++;
        UART_RX.tick=rt_tick_get();
    }
}
rt_mq_t APP_USART_QUEUE_RX = RT_NULL;
rt_mq_t APP_USART_QUEUE_TX = RT_NULL;
static void function_thread(void * pragram)
{
    APP_USART_DATA_Typedefstruct app;
    APP_USART_QURUR_DATA_Typedefstruct queue_data;
    uint8_t read;
    uint8_t ack;
    uint8_t address;
    uint16_t VALUE;
    
    char str[100];
    
    BSP_USART_FUN.Init(115200, USART_RECEIVE_DATA);
    
    while(APP_USART_QUEUE_RX == RT_NULL){
        rt_thread_delay(1);
        APP_USART_QUEUE_RX = rt_mq_create(  "USART_RX"
                                        ,   sizeof(APP_USART_DATA_Typedefstruct)
                                        ,   1
                                        ,   RT_IPC_FLAG_FIFO);
    }
    
    while(APP_USART_QUEUE_TX == RT_NULL){
        rt_thread_delay(1);
        APP_USART_QUEUE_TX = rt_mq_create(  "USART_TX"
                                        ,   sizeof(APP_USART_DATA_Typedefstruct)
                                        ,   1
                                        ,   RT_IPC_FLAG_FIFO);
    }

    
    //  发布自身任务启动的信号
    memset(&app, 0, sizeof(app));
    memset(&queue_data, 0, sizeof(queue_data));
    FUN_WRITE(&app);
    rt_event_send(event_handle, APP_EVENT_START);
    
    //  等待别的任务启动
    
     
    
    while(1){
        if(UART_RX.len){
            if((rt_tick_get()-UART_RX.tick)>50){
                if(UART_RX.len>=8){
                    if(UART_RX.buf[3]==' '){
                        if((UART_RX.buf[0]=='W') || (UART_RX.buf[0]=='w')){
                            read=0;
                            if(GET_BYTE(&(UART_RX.buf[1]), &address) && GET_WORD(&(UART_RX.buf[4]),&VALUE)){
                                ack=2;  //  成功
                            }else{
                                ack=1;  //  失败，错误信息已经准备好打印
                                sprintf(str, "\r\n 错误，十六进制数据错误，当前输入-%d  :::%s", UART_RX.len,((char*)(UART_RX.buf)));
                            }
                        }else if((UART_RX.buf[0]=='R') || (UART_RX.buf[0]=='r')){
                            read=0x80;
                            if(GET_BYTE(&(UART_RX.buf[1]), &address) && GET_WORD(&(UART_RX.buf[4]),&VALUE)){
                                ack=2;  //  
                            }else{
                                ack=1;  //  失败，错误信息已经准备好打印
                                sprintf(str, "\r\n 错误，十六进制数据错误，当前输入-%d  :::%s", UART_RX.len,((char*)(UART_RX.buf)));
                            }
                        }else{
                            ack=1;  //  失败，错误信息已经准备好打印
                            sprintf(str, "\r\n 错误，读写指令开头错误，当前输入-%d :::%s", UART_RX.len,((char*)(UART_RX.buf)));
                        }
                    }else{
                        ack=1;  //  失败，错误信息已经准备好打印
                        sprintf(str, "\r\n 错误，空格分隔符错误，当前输入-%d  :::%s", UART_RX.len,((char*)(UART_RX.buf)));
                    }
                }else{
                    ack=1;  //  失败，错误信息已经准备好打印
                    sprintf(str, "\r\n 错误，输入指令长度必须是8个字符，当前输入-%d  :::%s", UART_RX.len,((char*)(UART_RX.buf)));
                }
                
                UART_RX.len=0;
            }
        }
        if(ack==1){
            BSP_USART_FUN.Write(str, strlen(str));
        }else if(ack==2){
            queue_data.buf[0] = read|address;
            queue_data.buf[1] = VALUE/0x100;
            queue_data.buf[2] = VALUE%0x100;
            rt_mq_send(APP_USART_QUEUE_RX, &queue_data, sizeof(queue_data));
            rt_mq_recv(APP_USART_QUEUE_TX, &queue_data, sizeof(queue_data), RT_TICK_MAX);
            sprintf(str, "\r\n 正确-%c%02x %04x    back：0x%02X%02X", (read?'R':'W'),address, VALUE, queue_data.buf[1],queue_data.buf[2] );
            BSP_USART_FUN.Write(str, strlen(str));
        }
        
        ack=0;
        rt_thread_delay(10);
        
    }
}


