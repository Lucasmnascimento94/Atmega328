#include "types.h"
#include "spi.h"
#include "i2c.h"
#include "uart.h"
#include "sram.h"

// SPI Transfer Complete ISR
ISR(SPI_STC_vect){
    uint8_t flow = systemConfig.spi.interruptFLag;
    if(flow == SPI_IT_RUNNING_TRANSMIT){
        spiTransmitInt();
    }else if(SPI_IT_RUNNING_RECEIVE){
        spiReceiveInt();
    }else if(SPI_IT_DONE){
        systemConfig.spi.conf.mode_conf.mode = SPI_SLAVE;
        systemConfig.spi.interruptFLag = SPI_IT_RUNNING_RECEIVE;
        spiInit();
        // Triggered by SS pulled low.
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