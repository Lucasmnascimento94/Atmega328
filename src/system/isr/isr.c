#include "types.h"
#include "spi.h"
#include "i2c.h"
#include "uart.h"
#include "sram.h"
#include "err.h"
#include "tim.h"
// SPI Transfer Complete ISR
static index = 0;
ISR(SPI_STC_vect){
    uint8_t data = SPDR;
    systemConfig.spi.buffer[index++] = data;
    startTimer();
}


// I2C Transfer Complete ISR
ISR(TWI_vect){
    
}

// USART RX Transfer Complete ISR
ISR(USART_RX_vect){
    
}
// USART TX Transfer Complete ISR
ISR(USART_TX_vect){
    
}
// USART DATA REGISTER EMPTY Transfer Complete ISR
ISR(USART_UDRE_vect){
    
}
// Pin Change Interrupt Request 0 ISR
ISR(PCINT0_vect){
    
}
// Pin Change Interrupt Request 1 ISR
ISR(PCINT1_vect){
    
}
// Pin Change Interrupt Request 2 ISR
ISR(PCINT2_vect){
    
}


ISR(TIMER0_COMPA_vect){
    static uint16_t counter = 0;
    if(counter ++ == 20){
        stopTimer();
        cli();
        uartWrite_("buffer : ");
        uartWrite_(systemConfig.spi.buffer);
        uartWrite_("\n");
        systemConfig.spi.interruptFLag = ERR_TIMEOUT;
        index = 0;
        memset(systemConfig.spi.buffer, 0, sizeof(systemConfig.spi.buffer));
        counter = 0;

    }
}
