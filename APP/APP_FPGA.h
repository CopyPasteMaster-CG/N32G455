
#ifndef __APP_FPGA_H__
#define __APP_FPGA_H__

#include "APP.h"
#include "rtthread.h"

typedef struct{
    struct{
        char mode;
        char turn;
        char out;
        char rx_data;
    }nec,usart;
}APP_FPGA_DATA_Typedefstruct;

extern const APP_HANDLE_typedefstruct APP_FPGA_FUN;



#endif
