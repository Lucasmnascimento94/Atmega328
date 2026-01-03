#include "system.h"

SYSTEM systemConfig = {
    .FOSC = 16000000UL,

    // SPI DEFAULT CONFIGURATION
    .spi.conf.max_length = 256,
    .spi.conf.timeout = 1000,
    .spi.conf.irq = true,
    .spi.conf.lsbfirst = SPI_MSB,
    .spi.conf.spi_mode = SPI_SLAVE,
    .spi.conf.prescaler = 2,
    .spi.conf.clk_mode = 0,
    .spi.cs_reg.CS_DDR = &DDRB,
    .spi.cs_reg.CS_PORT = &PORTB,
    .spi.cs_reg.CS_RPIN = &PINB,
    .spi.cs_reg.CS_PIN = PB1,
    .spi.buffer = {'\0'},
    .spi.interruptFLag = SPI_IT_DONE,
    .spi.buffer_length = 100,
    .spi.index = 0,

    // I2C DEFAULT CONFIGURATION
    .i2c.frequency = 400000UL,
    .i2c.mode = MODE_MASTER_POL,
    .i2c.prescaler = 1,

    // UART DEFAULT CONFIGURATION
    .uart.baudrate = 9600,
    .uart.frame_size = 8,
    .uart.irq_enable = false,
    .uart.parity = 1,
    .uart.stop_bits = 0,


    // TIM
    .tim.tim0.frequency = 100,
    .tim.tim0.prescaler = 1024
};

uint8_t systemInit() {
    uint8_t status;
    char err[30] = {0};
    status = uartInit();
    if (status != ERR_OK) return status;
    _delay_ms(100);
    status = spiInit();
    if (status != ERR_OK) {
        sprintf(err,"SPI INIT_FAIL <%x>\n", status);
        uartWrite_(err); 
        return status;}
    _delay_ms(100);
    status = i2cInit();
    if (status != ERR_OK) {
        sprintf(err,"I2C INIT_FAIL <%x>\n", status);
        uartWrite_(err); 
        return status;}
    _delay_ms(100);
    return ERR_OK;
}


#if SYSTEMDEBUG == 1

void systemDisplay(){
    char s[300];
    sprintf(s, "SYSTEM.\n FOSC: %lu\n\n", systemConfig.FOSC);
    uartWrite_(s);

    sprintf(s, "SPI CONF.\n \
                 max_length: %d\n \
                 timeout: %d\n \
                 irq: %d\n \
                 lsbfirst: %d\n \
                 spi_mode: %d\n \
                 prescaler: %d\n \
                 clk_mode: %d\n\n", 
                 systemConfig.spi.conf.max_length,
                 systemConfig.spi.conf.timeout,
                 systemConfig.spi.conf.irq,
                 systemConfig.spi.conf.lsbfirst,
                 systemConfig.spi.conf.spi_mode,
                 systemConfig.spi.conf.prescaler,
                systemConfig.spi.conf.clk_mode);
    uartWrite_(s);

    sprintf(s, "I2C CONF.\n \
                 TWBR_VAL: %lu\n \
                 frequency: %lu\n \
                 prescaler: %d\n \
                 mode: %d\n\n",
                 systemConfig.i2c.TWBR_VAL,
                 systemConfig.i2c.frequency,
                 systemConfig.i2c.prescaler,
                 systemConfig.i2c.mode);
    uartWrite_(s);


    sprintf(s, "TIM0 CONF.\n \
                 prescaler: %u\n \
                 frequency: %u\n \
                 mode: %d\n \
                 errFLag: %d\n \
                 OCR0A: %u\n\n",
                 systemConfig.tim.tim0.prescaler,
                 systemConfig.tim.tim0.frequency,
                 systemConfig.tim.tim0.mode,
                 systemConfig.tim.errFlag,
                 systemConfig.tim.tim0.OCR0A_VAL);
    uartWrite_(s);

}



#endif