
#include "APP_GSV2011.h"
//#include "rtthread.h"
#include "string.h"
#include "gsv2011_config.h"
#include "uapi.h"
#include "stdint.h"





/* Video Logic Bus Configuration */
static const uint8 Gsv2k11ParTxTable_0[] = {
    0x27,0x00, 
    0xC9,0x0F,
    0xCA,0xFF,
    0xCB,0x00,
#if Gsv2k11LaneCompensation
    0xC8,0x02,
    0xCC,0x11,
    0xCD,0x22,
    0xCE,0x22,
    0xCF,0x22,
    0xD0,0x22,
    0xD1,0x82,
    0xD2,0x83,
    0xD3,0x83,
#else
    0xC8,0x00,
    0xCC,0x00,
    0xCD,0x00,
    0xCE,0x00,
    0xCF,0x00,
    0xD0,0x00,
    0xD1,0x80,
    0xD2,0x80,
    0xD3,0x80,
#endif
    0xD4,0x80,
    0xD5,0x80,
    0xD6,0x80,
    0xD7,0x80,
    0xD8,0x80,
    0xE2,0x04,
    0xC6,0x3B,
    0xC4,0x00,
    0xC3,0x17,
#if Gsv2k11WeakParallelDrive
    0xC5,0x17,
    0xE7,0x50,
#else
    0xC5,0x19,
    0xE7,0x58,
#endif
    0x66,0x00, 
    0x69,0x09, 
	//	0xE5,0x01,
    0xFF,0xFF
};

static const uint8 Gsv2k11ParRxTable_0[] = {
    0x27,0x04, 
    0xC8,0x00,
#if Gsv2k11LaneCompensation
    0xC9,0x00,
    0xCA,0x00,
#else
    0xC9,0x0F,
    0xCA,0xFF,
#endif
    0xCB,0x00,
    0xCC,0x00,
    0xCD,0x00,
    0xCE,0x00,
    0xCF,0x00,
    0xD0,0x00,
    0xD1,0x80,
    0xD2,0x80,
    0xD3,0x80,
    0xD4,0x00,
    0xD5,0x00,
    0xD6,0x00,
    0xD7,0x00,
    0xD8,0x00,
    0xC0,0x43,
    0xC1,0x0F,
    0xC2,0x00,
    0xC6,0x49,
    0xC7,0x10,
    0xDF,0x18,
    0xDA,0x12,
    0xDB,0x80,
    0xDC,0x01,
    0xDD,0xF0,
    0xDE,0x04,
    0xE1,0x0F,     //0X0F
    0xE5,0x00,   //0X02
    0xE6,0x00,
    0xE7,0x80,
    0xFF,0xFF
};

/* Video Logic Bus Configuration */
static const uint8 Gsv2k11ParTxTable_1[] = {
    0x27,0x00, 
    0xC9,0x0F,
    0xCA,0xFF,
    0xCB,0x00,
#if Gsv2k11LaneCompensation
    0xC8,0x0b,
    0xCC,0x11,
    0xCD,0x22,
    0xCE,0x22,
    0xCF,0x22,
    0xD0,0x22,
    0xD1,0x82,
    0xD2,0x83,
    0xD3,0x83,
#else
    0xC8,0x00,
    0xCC,0x00,
    0xCD,0x00,
    0xCE,0x00,
    0xCF,0x00,
    0xD0,0x00,
    0xD1,0x80,
    0xD2,0x80,
    0xD3,0x80,
#endif
    0xD4,0x80,
    0xD5,0x80,
    0xD6,0x80,
    0xD7,0x80,
    0xD8,0x80,
    0xE2,0x04,
    0xC6,0x3B,
    0xC4,0x00,
    0xC3,0x17,
#if Gsv2k11WeakParallelDrive
    0xC5,0x17,
    0xE7,0x50,
#else
    0xC5,0x19,
    0xE7,0x58,
#endif
    0x66,0x00, 
    0x69,0x09, 
	//	0xE5,0x01,
    0xFF,0xFF
};

static const uint8 Gsv2k11ParRxTable_1[] = {
    0x27,0x04, 
    0xC8,0x00,
#if Gsv2k11LaneCompensation
    0xC9,0x00,
    0xCA,0x00,
#else
    0xC9,0x0F,
    0xCA,0xFF,
#endif
    0xCB,0x00,
    0xCC,0x00,
    0xCD,0x00,
    0xCE,0x00,
    0xCF,0x00,
    0xD0,0x00,
    0xD1,0x80,
    0xD2,0x80,
    0xD3,0x80,
    0xD4,0x00,
    0xD5,0x00,
    0xD6,0x00,
    0xD7,0x00,
    0xD8,0x00,
    0xC0,0x43,
    0xC1,0x0F,
    0xC2,0x00,
    0xC6,0x49,
    0xC7,0x10,
    0xDF,0x18,
    0xDA,0x12,
    0xDB,0x80,
    0xDC,0x01,
    0xDD,0xF0,
    0xDE,0x04,
    0xE1,0x0F,     //0X0F
    0xE5,0x00,   //0X02
    0xE6,0x00,
    0xE7,0x80,
    0xFF,0xFF
};

/* Video Logic Bus Configuration */
static const uint8 Gsv2k11ParTxTable_2[] = {
    0x27,0x00, 
    0xC9,0x0F,
    0xCA,0xFF,
    0xCB,0x00,
#if Gsv2k11LaneCompensation
    0xC8,0x02,
    0xCC,0x11,
    0xCD,0x22,
    0xCE,0x22,
    0xCF,0x22,
    0xD0,0x22,
    0xD1,0x82,
    0xD2,0x83,
    0xD3,0x83,
#else
    0xC8,0x00,
    0xCC,0x00,
    0xCD,0x00,
    0xCE,0x00,
    0xCF,0x00,
    0xD0,0x00,
    0xD1,0x80,
    0xD2,0x80,
    0xD3,0x80,
#endif
    0xD4,0x80,
    0xD5,0x80,
    0xD6,0x80,
    0xD7,0x80,
    0xD8,0x80,
    0xE2,0x04,
    0xC6,0x3B,
    0xC4,0x00,
    0xC3,0x17,
#if Gsv2k11WeakParallelDrive
    0xC5,0x17,
    0xE7,0x50,
#else
    0xC5,0x19,
    0xE7,0x58,
#endif
    0x66,0x00, 
    0x69,0x09, 
	//	0xE5,0x01,
    0xFF,0xFF
};

static const uint8 Gsv2k11ParRxTable_2[] = {
    0x27,0x04, 
    0xC8,0x00,
#if Gsv2k11LaneCompensation
    0xC9,0x00,
    0xCA,0x00,
#else
    0xC9,0x0F,
    0xCA,0xFF,
#endif
    0xCB,0x00,
    0xCC,0x00,
    0xCD,0x00,
    0xCE,0x00,
    0xCF,0x00,
    0xD0,0x00,
    0xD1,0x80,
    0xD2,0x80,
    0xD3,0x80,
    0xD4,0x00,
    0xD5,0x00,
    0xD6,0x00,
    0xD7,0x00,
    0xD8,0x00,
    0xC0,0x43,
    0xC1,0x0F,
    0xC2,0x00,
    0xC6,0x49,
    0xC7,0x10,
    0xDF,0x18,
    0xDA,0x12,
    0xDB,0x80,
    0xDC,0x01,
    0xDD,0xF0,
    0xDE,0x04,
    0xE1,0x0F,     //0X0F
    0xE5,0x00,   //0X02
    0xE6,0x00,
    0xE7,0x80,
    0xFF,0xFF
};

/* Video Logic Bus Configuration */
static const uint8 Gsv2k11ParTxTable_3[] = {
    0x27,0x00, 
    0xC9,0x0F,
    0xCA,0xFF,
    0xCB,0x00,
#if Gsv2k11LaneCompensation
    0xC8,0x0e,
    0xCC,0x11,
    0xCD,0x22,
    0xCE,0x22,
    0xCF,0x22,
    0xD0,0x22,
    0xD1,0x82,
    0xD2,0x83,
    0xD3,0x83,
#else
    0xC8,0x00,
    0xCC,0x00,
    0xCD,0x00,
    0xCE,0x00,
    0xCF,0x00,
    0xD0,0x00,
    0xD1,0x80,
    0xD2,0x80,
    0xD3,0x80,
#endif
    0xD4,0x80,
    0xD5,0x80,
    0xD6,0x80,
    0xD7,0x80,
    0xD8,0x80,
    0xE2,0x04,
    0xC6,0x3B,
    0xC4,0x00,
    0xC3,0x17,
#if Gsv2k11WeakParallelDrive
    0xC5,0x17,
    0xE7,0x50,
#else
    0xC5,0x19,
    0xE7,0x58,
#endif
    0x66,0x00, 
    0x69,0x09, 
	//	0xE5,0x01,
    0xFF,0xFF
};

static const uint8 Gsv2k11ParRxTable_3[] = {
    0x27,0x04, 
    0xC8,0x00,
#if Gsv2k11LaneCompensation
    0xC9,0x00,
    0xCA,0x00,
#else
    0xC9,0x0F,
    0xCA,0xFF,
#endif
    0xCB,0x00,
    0xCC,0x00,
    0xCD,0x00,
    0xCE,0x00,
    0xCF,0x00,
    0xD0,0x00,
    0xD1,0x80,
    0xD2,0x80,
    0xD3,0x80,
    0xD4,0x00,
    0xD5,0x00,
    0xD6,0x00,
    0xD7,0x00,
    0xD8,0x00,
    0xC0,0x43,
    0xC1,0x0F,
    0xC2,0x00,
    0xC6,0x49,
    0xC7,0x10,
    0xDF,0x18,
    0xDA,0x12,
    0xDB,0x80,
    0xDC,0x01,
    0xDD,0xF0,
    0xDE,0x04,
    0xE1,0x0F,     //0X0F
    0xE5,0x00,   //0X00
    0xE6,0x00,
    0xE7,0x80,
    0xFF,0xFF
};

/* Video Logic Bus Configuration */
static const uint8 Gsv2k11ParTxTable_4[] = {
    0x27,0x00, 
    0xC9,0x0F,
    0xCA,0xFF,
    0xCB,0x00,
#if Gsv2k11LaneCompensation
    0xC8,0x0b,//0x02
    0xCC,0x11,
    0xCD,0x22,
    0xCE,0x22,
    0xCF,0x22,
    0xD0,0x22,
    0xD1,0x82,
    0xD2,0x83,
    0xD3,0x83,
#else
    0xC8,0x00,
    0xCC,0x00,
    0xCD,0x00,
    0xCE,0x00,
    0xCF,0x00,
    0xD0,0x00,
    0xD1,0x80,
    0xD2,0x80,
    0xD3,0x80,
#endif
    0xD4,0x80,
    0xD5,0x80,
    0xD6,0x80,
    0xD7,0x80,
    0xD8,0x80,
    0xE2,0x04,
    0xC6,0x3B,
    0xC4,0x00,
    0xC3,0x17,
#if Gsv2k11WeakParallelDrive
    0xC5,0x17,
    0xE7,0x50,
#else
    0xC5,0x19,
    0xE7,0x58,
#endif
    0x66,0x00, 
    0x69,0x09, 
	//	0xE5,0x01,
    0xFF,0xFF
};

static const uint8 Gsv2k11ParRxTable_4[] = {
    0x27,0x04, 
    0xC8,0x00,
#if Gsv2k11LaneCompensation
    0xC9,0x00,
    0xCA,0x00,
#else
    0xC9,0x0F,
    0xCA,0xFF,
#endif
    0xCB,0x00,
    0xCC,0x00,
    0xCD,0x00,
    0xCE,0x00,
    0xCF,0x00,
    0xD0,0x00,
    0xD1,0x80,
    0xD2,0x80,
    0xD3,0x80,
    0xD4,0x00,
    0xD5,0x00,
    0xD6,0x00,
    0xD7,0x00,
    0xD8,0x00,
    0xC0,0x43,
    0xC1,0x0F,
    0xC2,0x00,
    0xC6,0x49,
    0xC7,0x10,
    0xDF,0x18,
    0xDA,0x12,
    0xDB,0x80,
    0xDC,0x01,
    0xDD,0xF0,
    0xDE,0x04,
    0xE1,0x0F,     //0X0F
    0xE5,0x00,   //0X02
    0xE6,0x00,
    0xE7,0x80,
    0xFF,0xFF
};



extern int GsvMain(GSV_CONFIG *gsvconfig);
extern void rt_thread_exit(void);

static GSV_CONFIG gsv_config0;
static GSV_CONFIG gsv_config1;
static GSV_CONFIG gsv_config2;
static GSV_CONFIG gsv_config3;
static GSV_CONFIG gsv_config4;
static rt_thread_t thread0=NULL;
static rt_thread_t thread1=NULL;
static rt_thread_t thread2=NULL;
static rt_thread_t thread3=NULL;
static rt_thread_t thread4=NULL;
void function_thread0(void * pragram)
{
    
    GsvMain(pragram);
    rt_thread_exit();
}


#include "SEGGER_RTT.h"

static rt_sem_t sem_id=NULL;

void GSV2011_LOG(uint8_t id, const char * str)
{
    if(sem_id){
        rt_sem_take(sem_id, RT_WAITING_FOREVER);
        SEGGER_RTT_TerminalOut(id, str);
        rt_sem_release(sem_id);
    }
}

static void GSV2011_WRITE_MEMORY(GSV_CONFIG * gsv_config)
{
    rt_mutex_take(gsv_config->mutex, RT_TICK_MAX);
    memcpy(&gsv_config->make, &gsv_config->read, sizeof(gsv_config->make));
    rt_mutex_release(gsv_config->mutex);
}

static inline void GSV2011_READ_MEMORY(GSV_CONFIG * gsv_config , void * da)
{
    rt_mutex_take(gsv_config->mutex, RT_TICK_MAX);
    memcpy(da, &gsv_config->make, sizeof(gsv_config->make));
    rt_mutex_release(gsv_config->mutex);
}

static int GSV2011_READ_MEMORY_0(void * da)
{
    GSV2011_READ_MEMORY(&gsv_config0, da);
    return true;
}
static int GSV2011_READ_MEMORY_1(void * da)
{
    GSV2011_READ_MEMORY(&gsv_config1, da);
    return true;
}
static int GSV2011_READ_MEMORY_2(void * da)
{
    GSV2011_READ_MEMORY(&gsv_config2, da);
    return true;
}
static int GSV2011_READ_MEMORY_3(void * da)
{
    GSV2011_READ_MEMORY(&gsv_config3, da);
    return true;
}
static int GSV2011_READ_MEMORY_4(void * da)
{
    GSV2011_READ_MEMORY(&gsv_config4, da);
    return true;
}

int FUN_CREATE0(void)
{
    memset(&gsv_config0, 0, sizeof(GSV_CONFIG));
    memset(&gsv_config1, 0, sizeof(GSV_CONFIG));
    memset(&gsv_config2, 0, sizeof(GSV_CONFIG));
    memset(&gsv_config3, 0, sizeof(GSV_CONFIG));
    memset(&gsv_config4, 0, sizeof(GSV_CONFIG));
    
    
    sem_id = rt_sem_create("rtt_terminal", 1, RT_IPC_FLAG_FIFO);

    gsv_config0.WRITE_MEMORY = GSV2011_WRITE_MEMORY;
    gsv_config1.WRITE_MEMORY = GSV2011_WRITE_MEMORY;
    gsv_config2.WRITE_MEMORY = GSV2011_WRITE_MEMORY;
    gsv_config3.WRITE_MEMORY = GSV2011_WRITE_MEMORY;
    gsv_config4.WRITE_MEMORY = GSV2011_WRITE_MEMORY;
    
    
    gsv_config0.index = 0;
    gsv_config0.FirstPort = NULL;
    gsv_config0.PreviousPort = NULL;
    gsv_config0.LogicOutputSel = 0;
    gsv_config0.currentPoint=0;
//    gsv_config0.write_reg_e7=0x50;
//    gsv_config0.write_reg_c8=0x00;
//    gsv_config0.write_reg_e2 =0x04; 
//    gsv_config0.write_reg_rxe5=0x01; 
    gsv_config1.write_reg_e5=0x00;
    gsv_config1.write_reg_e1=0x0f;
    gsv_config0.Gsv2k11ParTxTable = Gsv2k11ParTxTable_0;
    gsv_config0.Gsv2k11ParRxTable = Gsv2k11ParRxTable_0;
    gsv_config0.bsp = &BSP_GSV2011_FUN0;

    gsv_config1.index = 1;
    gsv_config1.FirstPort = NULL;
    gsv_config1.PreviousPort = NULL;
    gsv_config1.LogicOutputSel = 1;
    gsv_config1.currentPoint=0;
    gsv_config0.write_reg_e7=0x50;
    gsv_config0.write_reg_c8=0x00;
    gsv_config0.write_reg_e2 =0x04; 
    gsv_config0.write_reg_rxe5=0x01;
//    gsv_config1.write_reg_e5=0x00;
//    gsv_config1.write_reg_e1=0x0f;
    gsv_config1.Gsv2k11ParTxTable = Gsv2k11ParTxTable_1;
    gsv_config1.Gsv2k11ParRxTable = Gsv2k11ParRxTable_1;
    gsv_config1.bsp = &BSP_GSV2011_FUN1;


    gsv_config2.index = 2;
    gsv_config2.FirstPort = NULL;
    gsv_config2.PreviousPort = NULL;
    gsv_config2.LogicOutputSel = 1;
    gsv_config2.currentPoint=0;
    gsv_config0.write_reg_e7=0x50;
    gsv_config0.write_reg_c8=0x00;
    gsv_config0.write_reg_e2 =0x04; 
    gsv_config0.write_reg_rxe5=0x01;
//    gsv_config2.write_reg_e5=0x00;
//    gsv_config2.write_reg_e1=0x0f;
    gsv_config2.Gsv2k11ParTxTable = Gsv2k11ParTxTable_2;
    gsv_config2.Gsv2k11ParRxTable = Gsv2k11ParRxTable_2;
    gsv_config2.bsp = &BSP_GSV2011_FUN2;

    gsv_config3.index = 3;
    gsv_config3.FirstPort = NULL;
    gsv_config3.PreviousPort = NULL;
    gsv_config3.LogicOutputSel = 1;
    gsv_config3.currentPoint=0;
    gsv_config0.write_reg_e7=0x50;
    gsv_config0.write_reg_c8=0x00;
    gsv_config0.write_reg_e2 =0x04; 
    gsv_config0.write_reg_rxe5=0x01;
//    gsv_config3.write_reg_e5=0x00;
//    gsv_config3.write_reg_e1=0x0f;
    gsv_config3.Gsv2k11ParTxTable = Gsv2k11ParTxTable_3;
    gsv_config3.Gsv2k11ParRxTable = Gsv2k11ParRxTable_3;
    gsv_config3.bsp = &BSP_GSV2011_FUN3;

    gsv_config4.index = 4;
    gsv_config4.FirstPort = NULL;
    gsv_config4.PreviousPort = NULL;
    gsv_config4.LogicOutputSel = 1;
    gsv_config4.currentPoint=0;
    gsv_config0.write_reg_e7=0x50;
    gsv_config0.write_reg_c8=0x00;
    gsv_config0.write_reg_e2 =0x04; 
    gsv_config0.write_reg_rxe5=0x01;
//	  gsv_config4.write_reg_e5=0x00;
//    gsv_config4.write_reg_e1=0x0e;
    gsv_config4.Gsv2k11ParTxTable = Gsv2k11ParTxTable_4;
    gsv_config4.Gsv2k11ParRxTable = Gsv2k11ParRxTable_4;
    gsv_config4.bsp = &BSP_GSV2011_FUN4;
	
    gsv_config0.mutex = rt_mutex_create(    "gsv_0",          /*互斥量名字*/
                                RT_IPC_FLAG_PRIO);  /**/
    thread0 = rt_thread_create(   "gsv_0",          /*线程名字*/                    
                                function_thread0,    /*线程入口函数*/
                                &gsv_config0,            /*线程入口函数参数*/
                                4096,               /*线程栈大小*/
                                10 ,                 /*线程优先级*/
                                20);                /*线程时间片*/

    if(thread0 !=RT_NULL)
        rt_thread_startup (thread0);
    else 
        return -1;


	///////////////////TX1		
    rt_thread_delay(200);
    gsv_config1.mutex = rt_mutex_create(    "gsv_1",          /*互斥量名字*/
                                RT_IPC_FLAG_PRIO);  /**/
	thread1 = rt_thread_create(   "gsv_1",          /*线程名字*/                    
                                function_thread0,    /*线程入口函数*/
                                &gsv_config1,            /*线程入口函数参数*/
                                4096,               /*线程栈大小*/
                                10 ,                 /*线程优先级*/
                                20);                /*线程时间片*/
                                        
	
		
	if(thread1 !=RT_NULL)
        rt_thread_startup (thread1);
	else 
        return -1;
	
	///////////////////TX2	
	rt_thread_delay(200);
    gsv_config2.mutex = rt_mutex_create(    "gsv_2",          /*互斥量名字*/
                                RT_IPC_FLAG_PRIO);  /**/
    thread2 = rt_thread_create(    "gsv_2",          /*线程名字*/                    
                                function_thread0,    /*线程入口函数*/
                                &gsv_config2,            /*线程入口函数参数*/
                                4096,               /*线程栈大小*/
                                10 ,                 /*线程优先级*/
                                20);                /*线程时间片*/																		
	
			
	if(thread2 !=RT_NULL)
        rt_thread_startup (thread2);	
    else 
        return -1;

	///////////////////TX3	
	rt_thread_delay(200);
    gsv_config3.mutex = rt_mutex_create(    "gsv_3",          /*互斥量名字*/
                                RT_IPC_FLAG_PRIO);  /**/
    thread3 = rt_thread_create(    "gsv_3",          /*线程名字*/                    
                                function_thread0,    /*线程入口函数*/
                                &gsv_config3,            /*线程入口函数参数*/
                                4096,               /*线程栈大小*/
                                10 ,                 /*线程优先级*/
                                20);                /*线程时间片*/																		

			
	if(thread3 !=RT_NULL)
        rt_thread_startup (thread3);	
	else 
        return -1;
	
	///////////////////TX4	
	rt_thread_delay(200);
    gsv_config4.mutex = rt_mutex_create(    "gsv_4",          /*互斥量名字*/
                                RT_IPC_FLAG_PRIO);  /**/
    thread4 = rt_thread_create(    "gsv_4",          /*线程名字*/                    
                                function_thread0,    /*线程入口函数*/
                                &gsv_config4,            /*线程入口函数参数*/
                                4096,               /*线程栈大小*/
                                10 ,                 /*线程优先级*/
                                20);                /*线程时间片*/
	
			
	if(thread4 !=RT_NULL)
        rt_thread_startup (thread4);	
	else 
        return -1;
		
    return 0;
}

const APP_HANDLE_typedefstruct APP_GSV2011_FUN0={
    .Create = FUN_CREATE0,
    .Read = GSV2011_READ_MEMORY_0,
};

const APP_HANDLE_typedefstruct APP_GSV2011_FUN1={
    .Read = GSV2011_READ_MEMORY_1,
};

const APP_HANDLE_typedefstruct APP_GSV2011_FUN2={
    .Read = GSV2011_READ_MEMORY_2,
};

const APP_HANDLE_typedefstruct APP_GSV2011_FUN3={
    .Read = GSV2011_READ_MEMORY_3,
};

const APP_HANDLE_typedefstruct APP_GSV2011_FUN4={
    .Read = GSV2011_READ_MEMORY_4,
};


