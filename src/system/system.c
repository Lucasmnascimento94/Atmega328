#include "system.h"

SYSTEM systemConfig = {
    .FOSC = 16000000UL,

    // SPI DEFAULT CONFIGURATION
    .spi.conf.max_length = 256,
    .spi.conf.timeout = 1000,
    .spi.conf.mode_conf.en = SPI_ENABLE,
    .spi.conf.mode_conf.irq = false,
    .spi.conf.mode_conf.lsbfirst = SPI_MSB,
    .spi.conf.mode_conf.mstr = SPI_MASTER,
    .spi.conf.mode_conf.prescaler = 2,
    .spi.cs_reg.CS_DDR = &DDRB,
    .spi.cs_reg.CS_PORT = &PORTB,
    .spi.cs_reg.CS_RPIN = &PINB,
    .spi.cs_reg.CS_PIN = PB1,
    .spi.buffer = {'\0'},

    // I2C DEFAULT CONFIGURATION
    .i2c.frequency = 4000000UL,
    .i2c.mode = MODE_MASTER_POL,
    .i2c.prescaler = 64,

    // UART DEFAULT CONFIGURATION
    .uart.baudrate = 9600,
    .uart.frame_size = 8,
    .uart.irq_enable = false,
    .uart.parity = 1,
    .uart.stop_bits = 0
};

uint8_t systemInit() {
    uint8_t status;
    char err[30] = {0};
    status = uartInit();
    if (status != ERR_OK) return status;

    status = spiInit();
    if (status != ERR_OK) {
        sprintf(err,"SPI INIT_FAIL <%x>\n", status);
        uartWrite_(err); 
        return status;}

    status = i2cInit();
    if (status != ERR_OK) {
        sprintf(err,"I2C INIT_FAIL <%x>\n", status);
        uartWrite_(err); 
        return status;}

    return ERR_OK;
}