
#include "APP_LED.h"
#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "BSP_LED.h"


/*******************任务配置开始*************************************************/
static APP_LED_DATA_Typedefstruct app_data={0}; //  本任务与其它任务共享的数据
static rt_thread_t handle = RT_NULL;            //  任务句柄
static rt_event_t event_handle = RT_NULL;       //  任务对应的事件句柄，事件已经全部定义完成，不要更改
static rt_mutex_t mutex_handle = RT_NULL;       //  任务共享数据用到的互斥量句柄
static void function_thread(void * pragram);    //  任务对应的函数

//  向共享内存，写入数据，本任务专用
static inline void FUN_WRITE(APP_LED_DATA_Typedefstruct * data)
{
    rt_mutex_take(mutex_handle, RT_TICK_MAX);
    memcpy(&app_data, data, sizeof(APP_LED_DATA_Typedefstruct));
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
    memcpy(data, &app_data, sizeof(APP_LED_DATA_Typedefstruct));
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
        handle = rt_thread_create(  "LED",             /*  线程名字*/                    
                                    function_thread,    /*  线程入口函数*/
                                    NULL,               /*  线程入口函数参数*/
                                    4096,               /*  线程栈大小*/
                                    3 ,                 /*  线程优先级*/
                                    20);                /*  线程时间片*/
    }
    if(event_handle == RT_NULL){
        event_handle = rt_event_create( "LED", RT_IPC_FLAG_PRIO );
    }
    if(mutex_handle == RT_NULL){
        mutex_handle = rt_mutex_create( "LED", RT_IPC_FLAG_PRIO);														
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

const APP_HANDLE_typedefstruct APP_LED_FUN={
    .Create = FUN_CREATE,
    .Read = FUN_READ,
    .Event = FUN_EVENT,
};
/*******************任务配置结束*************************************************/

/*******************以下全部是个性化自定义任务逻辑*************************************************/
static void function_thread(void * pragram)
{
//    uint8_t LED;
    APP_LED_DATA_Typedefstruct app;
    APP_GSV2011_DATA_TYPEDEFSTRUCT gsv_0;
    APP_GSV2011_DATA_TYPEDEFSTRUCT gsv_1;
    APP_GSV2011_DATA_TYPEDEFSTRUCT gsv_2;
    APP_GSV2011_DATA_TYPEDEFSTRUCT gsv_3;
    APP_GSV2011_DATA_TYPEDEFSTRUCT gsv_4;
    
       static uint32_t tick;  
    static uint32_t const delay_tick = 2;
    
//    BSP_LED_FUN.Init();
//    LED=0;
//    BSP_LED_FUN.Write(LED);
    FUN_WRITE(&app);
    
    //  发布自身任务启动的信号
    rt_event_send(event_handle, APP_EVENT_START);
    
    while(1){
        APP_GSV2011_FUN0.Read(&gsv_0);
        APP_GSV2011_FUN1.Read(&gsv_1);
        APP_GSV2011_FUN2.Read(&gsv_2);
        APP_GSV2011_FUN3.Read(&gsv_3);
        APP_GSV2011_FUN4.Read(&gsv_4);
        memset(&app, 0, sizeof(APP_LED_DATA_Typedefstruct));
        app.tick = rt_tick_get();
        app.activate = true;
        if((app.tick-tick)>delay_tick){
            tick = app.tick;
            BSP_LED_FUN.off(BSP_LED_NUM_TX1);
            BSP_LED_FUN.off(BSP_LED_NUM_TX2);
            BSP_LED_FUN.off(BSP_LED_NUM_TX3);
            BSP_LED_FUN.off(BSP_LED_NUM_TX4);
           
            if(gsv_0.connect){
                    BSP_LED_FUN.ledrxon();
            }else {
                  BSP_LED_FUN.ledrxoff();
            }        
            if(gsv_1.connect){
                    BSP_LED_FUN.on(BSP_LED_NUM_TX1);
            } if(gsv_2.connect){
                BSP_LED_FUN.on(BSP_LED_NUM_TX2);
            } if(gsv_3.connect){
                BSP_LED_FUN.on(BSP_LED_NUM_TX3);
            } if(gsv_4.connect){
                BSP_LED_FUN.on(BSP_LED_NUM_TX4);
            }
     
     }
    
      BSP_LED_FUN.refresh();
     rt_thread_delay(10);
//        LED=0;
//        if(gsv_0.connect){
//            app.value[0] = true;
//            LED |= BSP_LED_NUM_RX;
//        }
//        if(gsv_1.connect){
//            app.value[1] = true;
//            LED |= BSP_LED_NUM_TX1;
//        }
//        if(gsv_2.connect){
//            app.value[2] = true;
//            LED |= BSP_LED_NUM_TX2;
//        }
//        if(gsv_3.connect){
//            app.value[3] = true;
//            LED |= BSP_LED_NUM_TX3;
//        }
//        if(gsv_4.connect){
//            app.value[4] = true;
//            LED |= BSP_LED_NUM_TX4;
//        }
//        BSP_LED_FUN.Write(LED);
//        FUN_WRITE(&app);
        
    }
}

