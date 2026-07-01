
#include "APP_FLASH.h"
#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "BSP_FLASH.h"

static rt_thread_t handle = NULL;
static rt_event_t event_handle = RT_NULL;
static rt_mutex_t mutex=NULL;


static APP_FLASH_DATA_Typedefstruct app_data;

static void RESET_DATA_Callback(BSP_FLASH_DATA_Typedefstruct*data)
{
    data->NEC_MODE = 6;
    data->NEC_TURN = 0;
    data->NEC_OUT = 0;
    data->USART_MODE = 6;
    data->USART_TURN = 0;
}

static inline void FUN_WRITE(APP_FLASH_DATA_Typedefstruct * data)
{
    rt_mutex_take(mutex, RT_TICK_MAX);
    memcpy(&app_data, data, sizeof(APP_FLASH_DATA_Typedefstruct));
    rt_mutex_release(mutex);
}

static void function_thread(void * pragram)
{
    BSP_FLASH_DATA_Typedefstruct buf;
    APP_FLASH_DATA_Typedefstruct app;
    APP_FPGA_DATA_Typedefstruct fpga;
    
    BSP_FLASH_FUN.Init(RESET_DATA_Callback);
    BSP_FLASH_FUN.Read(&buf);
    app.NEC_MODE = buf.NEC_MODE;
    app.NEC_TURN = buf.NEC_TURN;
    app.NEC_OUT = buf.NEC_OUT;
    FUN_WRITE(&app);
    
    //  发布自身任务启动的信号
    rt_event_send(event_handle, APP_EVENT_START);
    
    //  等待别的任务启动
    APP_FPGA_FUN.Event(APP_EVENT_START);
    
    while(1){
        rt_thread_delay(100);
        
        APP_FPGA_FUN.Read(&fpga);
        
        if(fpga.nec.rx_data){
            buf.NEC_MODE = fpga.nec.mode;
            buf.NEC_TURN = fpga.nec.turn;
            buf.NEC_OUT = fpga.nec.out;
            
            app.NEC_MODE = fpga.nec.mode;
            app.NEC_TURN = fpga.nec.turn;
            app.NEC_OUT = fpga.nec.out;
        }
        
        FUN_WRITE(&app);
        BSP_FLASH_FUN.Write(&buf);
    }
}


static void FUN_EVENT(int event)
{
    while(event_handle == NULL){
        rt_thread_delay(1000);
    }
    
    rt_event_recv(  event_handle
                ,   event
                ,   RT_EVENT_FLAG_AND
                ,   RT_TICK_MAX
                ,   RT_NULL );
}


static int FUN_READ(void * data)
{
    rt_mutex_take(mutex, RT_TICK_MAX);
    memcpy(data, &app_data, sizeof(APP_FLASH_DATA_Typedefstruct));
    rt_mutex_release(mutex);
    return 1;
}

static int FUN_CREATE(void)
{
    handle = rt_thread_create(  "FLASH",            /*线程名字*/                    
                                function_thread,    /*线程入口函数*/
                                NULL,               /*线程入口函数参数*/
                                4096,               /*线程栈大小*/
                                3 ,                 /*线程优先级*/
                                20);                /*线程时间片*/																		
	event_handle = rt_event_create( "FLASH_EVENT", RT_IPC_FLAG_PRIO );
    
    mutex = rt_mutex_create(    "flash",          /*互斥量名字*/
                                RT_IPC_FLAG_PRIO);  /**/
			
	if(handle !=RT_NULL)
        rt_thread_startup (handle);	
	else 
        return -1;
		
    return 0;
}

const APP_HANDLE_typedefstruct APP_FLASH_FUN={
    .Create = FUN_CREATE,
    .Event = FUN_EVENT,
    .Read = FUN_READ,
};


