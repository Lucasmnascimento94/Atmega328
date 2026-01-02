#include "system.h"

SYSTEM systemConfig = {
    .FOSC = 16000000UL,

    // SPI DEFAULT CONFIGURATION
    .spi.conf.max_length = 256,
    .spi.conf.timeout = 1000,
    .spi.conf.mode_conf.en = SPI_ENABLE,
    .spi.conf.mode_conf.irq = true,
    .spi.conf.mode_conf.lsbfirst = SPI_MSB,
    .spi.conf.mode_conf.mstr = SPI_MASTER_SLAVE,
    .spi.conf.mode_conf.prescaler = 4,
    .spi.conf.mode_conf.mode = 0,
    .spi.cs_reg.CS_DDR = &DDRB,
    .spi.cs_reg.CS_PORT = &PORTB,
    .spi.cs_reg.CS_RPIN = &PINB,
    .spi.cs_reg.CS_PIN = PB1,
    .spi.buffer = {'\0'},
    .spi.interruptFLag = SPI_IT_DONE,
    .spi.buffer_length = 1,

    // I2C DEFAULT CONFIGURATION
    .i2c.frequency = 400000UL,
    .i2c.mode = MODE_MASTER_POL,
    .i2c.prescaler = 1,

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
                 enable: %d\n \
                 irq: %d\n \
                 lsbfirst: %d\n \
                 mstr: %d\n \
                 prescaler: %d\n \
                 mode: %d\n\n", 
                 systemConfig.spi.conf.max_length,
                 systemConfig.spi.conf.timeout,
                 systemConfig.spi.conf.mode_conf.en,
                 systemConfig.spi.conf.mode_conf.irq,
                 systemConfig.spi.conf.mode_conf.lsbfirst,
                 systemConfig.spi.conf.mode_conf.mstr,
                 systemConfig.spi.conf.mode_conf.prescaler,
                systemConfig.spi.conf.mode_conf.mode);
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

}



#endif