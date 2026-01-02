#include "types.h"
#include "spi.h"
#include "i2c.h"
#include "uart.h"
#include "sram.h"

// SPI Transfer Complete ISR
ISR(SPI_STC_vect){
    switch (systemConfig.spi.interruptFLag){
        case SPI_IT_RUNNING_TRANSMIT:
            spiTransmitInt();
            return;
        default:
            spiReceiveInt();
            uartWrite_("S\n");
            return;
    }
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