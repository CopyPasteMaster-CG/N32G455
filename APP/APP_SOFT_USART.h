#ifndef __APP_SOFT_USART_H__
#define __APP_SOFT_USART_H__

#include "APP.h"
#include "rtthread.h"

#define KVM_CMD_SET_MODE     0x01
#define KVM_CMD_GET_MODE     0x02
#define KVM_CMD_REPORT_MODE  0x03
#define KVM_CMD_ACK          0x06

#define KVM_MODE_PORT1_ONLY              0x01
#define KVM_MODE_PORT2_ONLY              0x02
#define KVM_MODE_PORT3_ONLY              0x03
#define KVM_MODE_PORT4_ONLY              0x04
#define KVM_MODE_TWO_SMALL_MAIN_PORT1    0x05
#define KVM_MODE_TWO_SMALL_MAIN_PORT2    0x06
#define KVM_MODE_TWO_SMALL_MAIN_PORT3    0x07
#define KVM_MODE_PORT12_ONLY             0x08
#define KVM_MODE_PORT34_ONLY             0x09
#define KVM_MODE_PORT_ALL                0x0A
#define KVM_MODE_PORT_ALL_SYNC           0x0B
#define KVM_MODE_THREE_SMALL_MAIN_PORT1  0x0C
#define KVM_MODE_THREE_SMALL_MAIN_PORT2  0x0D
#define KVM_MODE_THREE_SMALL_MAIN_PORT3  0x0E
#define KVM_MODE_THREE_SMALL_MAIN_PORT4  0x0F

typedef struct{
    uint8_t mode;
    bool activate;
}APP_SOFT_USART_DATA_Typedefstruct;

extern const APP_HANDLE_typedefstruct APP_SOFT_USART_FUN;


void APP_SOFT_USART_ClearActivate(APP_SOFT_USART_DATA_Typedefstruct *data);
uint8_t APP_SOFT_USART_SendSetMode(uint8_t fpga_mode);

#endif
