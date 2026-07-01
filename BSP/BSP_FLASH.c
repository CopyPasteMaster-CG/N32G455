
#include "BSP_FLASH.h"

#include "BSP.h"

#include "string.h"




typedef uint32_t flash_write_once_size_type;    //  向flash单次写入数据的大小





static const flash_write_once_size_type FLASH_FLAG_ACTIVATE =   0xFFFFFFFF; //  当前数据块激活
static const flash_write_once_size_type FLASH_FLAG_ABANDON  =   0xFF55AA00; //  当前数据块禁用
static const uint8_t magic_flag = 0xF5;

static volatile uint32_t address;
static const uint32_t start_address = FLASH_BASE + 2048 * 245;
static const uint32_t flash_size = (255-245+1)*2048;
static const uint32_t flash_page_size = 2048;
static BSP_FLASH_DATA_Typedefstruct buffer;
struct __BSP_FLASH_DATA{
    BSP_FLASH_DATA_Typedefstruct data;
    uint8_t magic_data;
    uint8_t check;
};
static uint32_t const write_data_len =  ((sizeof(struct __BSP_FLASH_DATA)) % sizeof(flash_write_once_size_type)) ?
                                        ((uint32_t)(((sizeof(struct __BSP_FLASH_DATA)) + sizeof(flash_write_once_size_type))  /  sizeof(flash_write_once_size_type) ) * sizeof(flash_write_once_size_type)):
                                        (sizeof(struct __BSP_FLASH_DATA)) ;





static inline uint8_t SUM_MAKE(void * da, uint32_t len)
{
    uint8_t sum=0;
    uint32_t i=0; 
    for(;i<len;i++){
        sum+=((uint8_t*)da)[i];
    }
    return sum;
}

static inline void FUN_FLASH_ERASE_PAGE(uint32_t addr)
{
    static uint8_t i=0;
    i++;
    if(i>3){
        while(1);
    }
    
    FLASH_Unlock();
    while(FLASH_GetSTS()==FLASH_BUSY){}     //  等到FLASH不繁忙
    FLASH_EraseOnePage(((uint8_t)(addr/flash_page_size)) * flash_page_size);
    FLASH_Lock();
}

static inline void FUN_FLASH_WRITE_FLAG(uint32_t addr)
{
    FLASH_Unlock();
    while(FLASH_GetSTS()==FLASH_BUSY){}     //  等到FLASH不繁忙
    FLASH_ProgramWord(addr, FLASH_FLAG_ABANDON);
    FLASH_Lock();
}

static inline void FUN_FLASH_WRITE_DATA(uint32_t addr, struct __BSP_FLASH_DATA * buf)
{
    FLASH_Unlock();
    addr+=sizeof(flash_write_once_size_type);
    for(uint32_t i=0; i<sizeof(struct __BSP_FLASH_DATA); i+=sizeof(flash_write_once_size_type)){
        while(FLASH_GetSTS()==FLASH_BUSY){}     //  等到FLASH不繁忙
        FLASH_ProgramWord(addr+i, *((flash_write_once_size_type*)(((uint32_t)(buf))+i)));
    }
    FLASH_Lock();
}

static inline void FUN_FLASH_RESET(BSP_FLASH_RESET_DATA_Callback rx_callback)
{
    address = start_address;
    struct __BSP_FLASH_DATA buf;
    
    memset(&buf, 0, sizeof(struct __BSP_FLASH_DATA));
    rx_callback(&buf.data);
    buf.check = SUM_MAKE(&buf.data, sizeof(BSP_FLASH_DATA_Typedefstruct));
    buf.magic_data = magic_flag;
    FUN_FLASH_ERASE_PAGE(address);
    
    FLASH_Unlock();
    for(uint32_t i=0; i<sizeof(struct __BSP_FLASH_DATA); i+=sizeof(flash_write_once_size_type)){
        while(FLASH_GetSTS()==FLASH_BUSY){}     //  等到FLASH不繁忙
        FLASH_ProgramWord(address+sizeof(flash_write_once_size_type)+i, *((flash_write_once_size_type*)(((uint32_t)(&buf))+i)));
    }
    FLASH_Lock();
    
}


static void FUN_FLASH_Init(BSP_FLASH_RESET_DATA_Callback rx_callback)
{
    flash_write_once_size_type flag;
    struct __BSP_FLASH_DATA buf;
    
    if(((write_data_len+sizeof(flash_write_once_size_type))>2048)  ||  (rx_callback==NULL)){
        while(1){;} //  不支持的长度
    }
    
    again:
    address = start_address;
    memset(&buf, 0, sizeof(struct __BSP_FLASH_DATA));
    do{
        flag = *((flash_write_once_size_type *)address);
        if(flag == FLASH_FLAG_ACTIVATE){
            //  从标志位上来说，数据块有效
            memcpy(&buf, (void*)(address+sizeof(flash_write_once_size_type)), sizeof(buf));
            if( (buf.magic_data == magic_flag)
            &&  (buf.check == SUM_MAKE(&buf.data, sizeof(BSP_FLASH_DATA_Typedefstruct)))    ){
                //  校验通过，数据正确，可以读取
                memcpy(&buffer, &buf.data, sizeof(BSP_FLASH_DATA_Typedefstruct));
                return;
            }else{
                //  数据混乱，需要复位
                FUN_FLASH_RESET(rx_callback);
                goto again;
            }
            
        }else if(flag == FLASH_FLAG_ABANDON){
            //  从标志位上来说，数据块无效，切换到下一个数据块
            address+=write_data_len+sizeof(flash_write_once_size_type);
            if( (address > (start_address+flash_size-1))
            ||  (address < start_address)
            ||  ((address+write_data_len-1) > (start_address+flash_size-1))
            ||  ((address+write_data_len-1) < start_address) ){
                //  溢出，找不到数据，重新配置
                FUN_FLASH_RESET(rx_callback);
                goto again;
            }
        }else{
            //  异常数据块，不能确定能否读取到正确的数据，进行复位操作
            FUN_FLASH_RESET(rx_callback);
            goto again;
        }    
    }while(1);
}

static void FUN_FLASH_Write( BSP_FLASH_DATA_Typedefstruct * data )
{
    struct __BSP_FLASH_DATA buf;
    if(memcmp(data, &buffer, sizeof(BSP_FLASH_DATA_Typedefstruct)) != 0){
        //  需要写入新的数据
        
        //  首先注销原来的数据
        FUN_FLASH_WRITE_FLAG(address);
        
        //  然后判断是否需要擦除新的页
        address+=sizeof(flash_write_once_size_type)+write_data_len;
        if( (address > (start_address+flash_size-1))
        ||  (address < start_address)
        ||  ((address+write_data_len-1) > (start_address+flash_size-1))
        ||  ((address+write_data_len-1) < start_address) ){
            //  溢出，需要从头计算地址
            address = start_address;
            FUN_FLASH_ERASE_PAGE(address);
        }else if(((address+write_data_len+sizeof(flash_write_once_size_type)-1)/flash_page_size) != ((address-1)/flash_page_size)){
            //  上一页的尾地址和本次的尾地址，不在同一页
            FUN_FLASH_ERASE_PAGE(address+write_data_len+sizeof(flash_write_once_size_type));
        }
        
        //  最后写入数据
        memset(&buf, 0, sizeof(struct __BSP_FLASH_DATA));
        memcpy(&buf.data, data, sizeof(BSP_FLASH_DATA_Typedefstruct));
        buf.magic_data = magic_flag;
        buf.check = SUM_MAKE(&buf.data, sizeof(BSP_FLASH_DATA_Typedefstruct));
        memcpy(&buffer, data, sizeof(BSP_FLASH_DATA_Typedefstruct));
        FUN_FLASH_WRITE_DATA(address, &buf);
    }
}

static bool FUN_FLASH_Read( BSP_FLASH_DATA_Typedefstruct * data )
{
    if(address==0){
        return false;
    }
    memcpy(data, &buffer, sizeof(BSP_FLASH_DATA_Typedefstruct));
    return true;
}



BSP_FLASH_FUN_typestruct BSP_FLASH_FUN =
{
    .Init = FUN_FLASH_Init,
    .Write = FUN_FLASH_Write,
    .Read = FUN_FLASH_Read,
} ;
