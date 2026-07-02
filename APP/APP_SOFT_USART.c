#include "APP_SOFT_USART.h"
#include "BSP_SOFT_USART.h"
#include "BSP_USART.h"
#include "stdio.h"
#include "string.h"

#define KVM_FRAME_HEAD1  0x55
#define KVM_FRAME_HEAD2  0xAA

typedef enum {
    KVM_RX_HEAD1 = 0,
    KVM_RX_HEAD2,
    KVM_RX_CMD,
    KVM_RX_MODE,
    KVM_RX_CRC
} KVM_RX_STATE;

typedef struct {
    KVM_RX_STATE state;
    uint8_t cmd;
    uint8_t mode;
} KVM_RX_DATA;

static KVM_RX_DATA kvm_rx;
static APP_SOFT_USART_DATA_Typedefstruct app_data;
static uint8_t current_mode = 0x01;
static rt_thread_t handle = RT_NULL; 
static rt_event_t event_handle = RT_NULL;
static rt_mutex_t mutex_handle = RT_NULL;
static rt_sem_t rx_sem = RT_NULL;

static void function_thread(void *parameter);

static inline void FUN_WRITE(APP_SOFT_USART_DATA_Typedefstruct *data)
{
    rt_mutex_take(mutex_handle, RT_TICK_MAX);
    memcpy(&app_data, data, sizeof(APP_SOFT_USART_DATA_Typedefstruct));
    rt_mutex_release(mutex_handle);
}


void APP_SOFT_USART_ClearActivate(APP_SOFT_USART_DATA_Typedefstruct *data)
{
    if(data == RT_NULL)
    {
        return;
    }

    data->activate = false;
    FUN_WRITE(data);
}

static uint8_t KVM_ModeToFpgaMode(uint8_t kvm_mode, uint8_t *fpga_mode)
{
    if(fpga_mode == RT_NULL)
    {
        return 0;
    }

    switch(kvm_mode)
    {
        case KVM_MODE_PORT1_ONLY:
            *fpga_mode = 0;
            break;

        case KVM_MODE_PORT2_ONLY:
            *fpga_mode = 1;
            break;

        case KVM_MODE_PORT3_ONLY:
            *fpga_mode = 2;
            break;

        case KVM_MODE_PORT4_ONLY:
            *fpga_mode = 3;
            break;

        case KVM_MODE_TWO_SMALL_MAIN_PORT1:
            *fpga_mode = 12;
            break;

        case KVM_MODE_TWO_SMALL_MAIN_PORT2:
            *fpga_mode = 13;
            break;

        case KVM_MODE_TWO_SMALL_MAIN_PORT3:
            *fpga_mode = 14;
            break;

        case KVM_MODE_PORT12_ONLY:
            *fpga_mode = 10;
            break;

        case KVM_MODE_PORT34_ONLY:
            *fpga_mode = 11;
            break;

        case KVM_MODE_PORT_ALL:
            *fpga_mode = 4;
            break;

        case KVM_MODE_PORT_ALL_SYNC:
            *fpga_mode = 4;
            break;

        case KVM_MODE_THREE_SMALL_MAIN_PORT4:
            *fpga_mode = 9;
            break;

        case KVM_MODE_THREE_SMALL_MAIN_PORT3:
            *fpga_mode = 8;
            break;

        case KVM_MODE_THREE_SMALL_MAIN_PORT2:
            *fpga_mode = 7;
            break;

        case KVM_MODE_THREE_SMALL_MAIN_PORT1:
            *fpga_mode = 6;
            break;

        default:
            return 0;
    }

    return 1;
}


static uint8_t KVM_FpgaModeToKvmMode(uint8_t fpga_mode, uint8_t *kvm_mode)
{
    if(kvm_mode == RT_NULL)
    {
        return 0;
    }

    switch(fpga_mode)
    {
        case 0:
            *kvm_mode = KVM_MODE_PORT1_ONLY;
            break;

        case 1:
            *kvm_mode = KVM_MODE_PORT2_ONLY;
            break;

        case 2:
            *kvm_mode = KVM_MODE_PORT3_ONLY;
            break;

        case 3:
            *kvm_mode = KVM_MODE_PORT4_ONLY;
            break;

        case 4:
            *kvm_mode = KVM_MODE_PORT_ALL;
						*kvm_mode = KVM_MODE_PORT_ALL_SYNC;
            break;

        case 6:
            *kvm_mode = KVM_MODE_THREE_SMALL_MAIN_PORT1;
            break;

        case 7:
            *kvm_mode = KVM_MODE_THREE_SMALL_MAIN_PORT2;
            break;

        case 8:
            *kvm_mode = KVM_MODE_THREE_SMALL_MAIN_PORT3;
            break;

        case 9:
            *kvm_mode = KVM_MODE_THREE_SMALL_MAIN_PORT4;
            break;

        case 10:
            *kvm_mode = KVM_MODE_PORT12_ONLY;
            break;

        case 11:
            *kvm_mode = KVM_MODE_PORT34_ONLY;
            break;

        case 12:
            *kvm_mode = KVM_MODE_TWO_SMALL_MAIN_PORT1;
            break;

        case 13:
            *kvm_mode = KVM_MODE_TWO_SMALL_MAIN_PORT2;
            break;

        case 14:
            *kvm_mode = KVM_MODE_TWO_SMALL_MAIN_PORT3;
            break;

        default:
            return 0;
    }

    return 1;
}

static uint8_t KVM_CRC8_Calculate(const uint8_t *data, uint8_t length)
{
    uint8_t crc = 0x00;
    uint8_t i;

    while(length--)
    {
        crc ^= *data++;

        for(i = 0; i < 8; i++)
        {
            if(crc & 0x80)
            {
                crc = (uint8_t)((crc << 1) ^ 0x07);
            }
            else
            {
                crc <<= 1;
            }
        }
    }

    return crc;
}

static void KVM_UART_SendFrame(uint8_t cmd, uint8_t mode)
{
    uint8_t crc_data[2];
    uint8_t frame[5];

    crc_data[0] = cmd;
    crc_data[1] = mode;

    frame[0] = KVM_FRAME_HEAD1;
    frame[1] = KVM_FRAME_HEAD2;
    frame[2] = cmd;
    frame[3] = mode;
    frame[4] = KVM_CRC8_Calculate(crc_data, 2);

    BSP_SFOT_USART_FUN.Write(frame, sizeof(frame));
}


uint8_t APP_SOFT_USART_SendSetMode(uint8_t fpga_mode)
{
    uint8_t kvm_mode;

    if(KVM_FpgaModeToKvmMode(fpga_mode, &kvm_mode) == 0)
    {
        return 0;
    }

    KVM_UART_SendFrame(KVM_CMD_SET_MODE, kvm_mode);
    return 1;
}

static void KVM_UART_HandleFrame(uint8_t cmd, uint8_t mode)
{
    APP_SOFT_USART_DATA_Typedefstruct app;
    uint8_t fpga_mode;
    char str[100];

    switch(cmd)
    {
        case KVM_CMD_SET_MODE:
            if(KVM_ModeToFpgaMode(mode, &fpga_mode))
            {
                current_mode = mode;
                app.mode = fpga_mode;
                app.activate = true;
							
                FUN_WRITE(&app);
                rt_kprintf(str, "\r\n KVM SET_MODE kvm_mode 0x%02X fpga_mode %d activate %d ",
                        current_mode,
                        app.mode,
                        app.activate);
                //KVM_UART_SendFrame(KVM_CMD_ACK, current_mode);
            }
            break;

        case KVM_CMD_GET_MODE:
            KVM_UART_SendFrame(KVM_CMD_REPORT_MODE, current_mode);
            break;

        default:
            break;
    }
}

static void KVM_UART_ReceiveByte(uint8_t byte)
{
    uint8_t crc_data[2];
    uint8_t crc;

    switch(kvm_rx.state)
    {
        case KVM_RX_HEAD1:
            if(byte == KVM_FRAME_HEAD1)
            {
                kvm_rx.state = KVM_RX_HEAD2;
            }
            break;

        case KVM_RX_HEAD2:
            if(byte == KVM_FRAME_HEAD2)
            {
                kvm_rx.state = KVM_RX_CMD;
            }
            else if(byte == KVM_FRAME_HEAD1)
            {
                kvm_rx.state = KVM_RX_HEAD2;
            }
            else
            {
                kvm_rx.state = KVM_RX_HEAD1;
            }
            break;

        case KVM_RX_CMD:
            kvm_rx.cmd = byte;
            kvm_rx.state = KVM_RX_MODE;
            break;

        case KVM_RX_MODE:
            kvm_rx.mode = byte;
            kvm_rx.state = KVM_RX_CRC;
            break;

        case KVM_RX_CRC:
            crc_data[0] = kvm_rx.cmd;
            crc_data[1] = kvm_rx.mode;
            crc = KVM_CRC8_Calculate(crc_data, 2);

            if(crc == byte)
            {
                KVM_UART_HandleFrame(kvm_rx.cmd, kvm_rx.mode);
            }

            kvm_rx.state = KVM_RX_HEAD1;
            break;

        default:
            kvm_rx.state = KVM_RX_HEAD1;
            break;
    }
}

static void APP_SOFT_USART_RxNotify(void)
{
    if(rx_sem != RT_NULL)
    {
        rt_sem_release(rx_sem);
    }
}

static int FUN_CREATE(void)
{
    
		BSP_SFOT_USART_FUN.Init();
	if(rx_sem == RT_NULL)
    {
        rx_sem = rt_sem_create("SUART_RX", 0, RT_IPC_FLAG_FIFO);
    }

    if(event_handle == RT_NULL)
    {
        event_handle = rt_event_create("SUART", RT_IPC_FLAG_PRIO);
    }

    if(mutex_handle == RT_NULL)
    {
        mutex_handle = rt_mutex_create("SUART", RT_IPC_FLAG_PRIO);
    }

    if(handle == RT_NULL)
    {
        handle = rt_thread_create("SUART",
                                  function_thread,
                                  RT_NULL,
                                  2048,
                                  3,
                                  20);
    }

    if(event_handle == RT_NULL)
    {
        return APP_CREATE_ERR_EVENT;
    }
    else if(handle == RT_NULL)
    {
        return APP_CREATE_ERR_HANDLE;
    }
    else if(rx_sem == RT_NULL)
    {
        return APP_CREATE_ERR_MUTEX;
    }
    else if(mutex_handle == RT_NULL)
    {
        return APP_CREATE_ERR_MUTEX;
    }
    else
    {
        BSP_SFOT_USART_FUN.SetRxNotify(APP_SOFT_USART_RxNotify);
        rt_thread_startup(handle);
        return APP_CREATE_OK;
    }
}

static int FUN_READ(void *data)
{
    if(data == RT_NULL)
    {
        return 0;
    }

    while(mutex_handle == RT_NULL)
    {
        rt_thread_delay(1);
    }

    rt_mutex_take(mutex_handle, RT_TICK_MAX);
    memcpy(data, &app_data, sizeof(APP_SOFT_USART_DATA_Typedefstruct));
    rt_mutex_release(mutex_handle);
    return 1;
}

static void FUN_EVENT(int event)
{
    while(event_handle == RT_NULL)
    {
        rt_thread_delay(1);
    }

    rt_event_recv(event_handle,
                  event,
                  RT_EVENT_FLAG_AND,
                  RT_TICK_MAX,
                  RT_NULL);
}

const APP_HANDLE_typedefstruct APP_SOFT_USART_FUN =
{
    .Create = FUN_CREATE,
    .Read = FUN_READ,
    .Event = FUN_EVENT,
};

static void function_thread(void *parameter)
{
    APP_SOFT_USART_DATA_Typedefstruct app;
    uint8_t data;

    (void)parameter;
    memset(&kvm_rx, 0, sizeof(kvm_rx));
    memset(&app, 0, sizeof(app));
    app.mode = 255;
    app.activate = false;
    FUN_WRITE(&app);
    rt_event_send(event_handle, APP_EVENT_START);

    while(1)
    {
        rt_sem_take(rx_sem, RT_TICK_MAX);

        while(BSP_SFOT_USART_FUN.Read(&data))
        {
            KVM_UART_ReceiveByte(data);
        }
    }
}
