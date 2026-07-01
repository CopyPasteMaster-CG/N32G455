
#include "APP_KEY.h"
#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "BSP_KEY.h"


/*******************任务配置开始*************************************************/
static APP_KEY_DATA_Typedefstruct app_data={0}; //  本任务与其它任务共享的数据
static rt_thread_t handle = RT_NULL;            //  任务句柄
static rt_event_t event_handle = RT_NULL;       //  任务对应的事件句柄，事件已经全部定义完成，不要更改
static rt_mutex_t mutex_handle = RT_NULL;       //  任务共享数据用到的互斥量句柄
static void function_thread(void * pragram);    //  任务对应的函数

//  向共享内存，写入数据，本任务专用
static inline void FUN_WRITE(APP_KEY_DATA_Typedefstruct * data)
{
    rt_mutex_take(mutex_handle, RT_TICK_MAX);
    memcpy(&app_data, data, sizeof(APP_KEY_DATA_Typedefstruct));
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
    memcpy(data, &app_data, sizeof(APP_KEY_DATA_Typedefstruct));
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
        handle = rt_thread_create(  "KEY",             /*  线程名字*/                    
                                    function_thread,    /*  线程入口函数*/
                                    NULL,               /*  线程入口函数参数*/
                                    4096,               /*  线程栈大小*/
                                    3 ,                 /*  线程优先级*/
                                    20);                /*  线程时间片*/
    }
    if(event_handle == RT_NULL){
        event_handle = rt_event_create( "KEY", RT_IPC_FLAG_PRIO );
    }
    if(mutex_handle == RT_NULL){
        mutex_handle = rt_mutex_create( "KEY", RT_IPC_FLAG_PRIO);														
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

const APP_HANDLE_typedefstruct APP_KEY_FUN={
    .Create = FUN_CREATE,
    .Read = FUN_READ,
    .Event = FUN_EVENT,
};
/*******************任务配置结束*************************************************/

/*******************以下全部是个性化自定义任务逻辑*************************************************/
static void function_thread(void * pragram)
{
    uint8_t key;
    APP_KEY_DATA_Typedefstruct app;
    
    BSP_KEY_FUN.Init();
    
    //  发布自身任务启动的信号
    rt_event_send(event_handle, APP_EVENT_START);
    
    while(1){
        app.tick=1;
        app.tick = rt_tick_get();
        BSP_KEY_FUN.Read(&key);
        app.activate = true;
        app.value[0] = key&BSP_KEY_NUM_1;
        app.value[1] = key&BSP_KEY_NUM_2;
        app.value[2] = key&BSP_KEY_NUM_3;
        app.value[3] = key&BSP_KEY_NUM_4;
        app.mode = key;
        FUN_WRITE(&app);
        rt_thread_delay(10);
    }
}

