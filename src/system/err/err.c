#include "err.h"
#include "uart.h"

void systemErrDecode(uint8_t err){
    switch (err){
        case ERR_INIT_FAIL: uartWrite_("ERR_INIT_FAIL\n"); break;
        case ERR_TIMEOUT: uartWrite_("ERR_TIMEOUT\n"); break;
        case ERR_INVALID_PARAM: uartWrite_("ERR_INVALID_PARAM\n"); break;
        case ERR_BUFFER_OVERFLOW: uartWrite_("ERR_BUFFER_OVERFLOW\n"); break;
        case ERR_NULL_POINTER: uartWrite_("ERR_NULL_POINTER\n"); break;
        case ERR_UART_INVALID_MODE: uartWrite_("ERR_INVALID_MODE\n"); break;
        case ERR_GPIO_INIT_FAIL: uartWrite_("ERR_GPIO_INIT_FAIL\n"); break;
        case ERR_CS_ENABLE_FAIL: uartWrite_("ERR_CS_ENABLE_FAIL\n"); break;
        case ERR_EMPTY_BUFFER: uartWrite_("ERR_EMPTY_BUFFER\n"); break;
        case SPI_INVALID_STATE: uartWrite_("ERR_SPI_INVALID_STATE\n"); break;
        default: uartWrite_("ERR_UNKNOWN\n"); break;
    }
}