#include "system.h"
#include "tim.h"
#include "err.h"

static uint8_t init = false;
uint8_t *ptr_holder;
uint8_t mstrResolve(uint8_t action);
uint8_t spiGPIO();
uint8_t spiConf();
uint8_t spitTransmitPoll(char *data, uint16_t length);
uint8_t spiReceivePoll(char *data, uint16_t length);

uint8_t spiInit(){
    if(init){spiDeInit();}

    uint8_t status = spiConf();
    if(status != ERR_OK){systemErrDecode(status); return status;}

    status = spiGPIO();
    if(status != ERR_OK){systemErrDecode(status); return status;}

    SPI_CONF *spi = &systemConfig.spi.conf;
    uint8_t mstr = spi->spi_mode;
    mstr = (spi->spi_mode == SPI_MASTER || mstr == SPI_MASTER_SLAVE) ? 1 : 0;
    SPCR =  (SPCR & ~_BV(SPIE)) | ((spi->irq& 1u) << SPIE);        // Enable/Disable Interrupt Mode
    SPCR =  (SPCR & ~_BV(DORD)) | ((spi->lsbfirst & 1u)<< DORD);   // Set Data Orientation
    SPCR =  (SPCR & ~_BV(MSTR)) | ((mstr & 1u)<< MSTR);       // Enable/Disable Master Mode
    SPCR =  (SPCR & ~_BV(SPE))  | ((1u)<< SPE);          // Enable SPI

    if(spi->irq){sei();}
    init = true;
    return ERR_OK;
}

uint8_t spiDeInit(){
    SPCR = 0X00;
    PORTB |=  (1<<PB5) | (1<<PB4) | (1<<PB3) | (1<<PB2);
    DDRB &= ~(1<<PB5) & ~(1<<PB4) & ~(1<<PB3) & ~(1<<PB2);
    *systemConfig.spi.cs_reg.CS_PORT &= ~(1<<systemConfig.spi.cs_reg.CS_PIN);
    *systemConfig.spi.cs_reg.CS_DDR &= ~(1<<systemConfig.spi.cs_reg.CS_PIN);
    init = false;
    return ERR_OK;
}

uint8_t spiGPIO(){
    SPI_CONF spi = systemConfig.spi.conf;
    volatile uint8_t *CS_DDR = systemConfig.spi.cs_reg.CS_DDR;
    volatile uint8_t *CS_PORT = systemConfig.spi.cs_reg.CS_PORT;
    volatile uint8_t *CS_RPIN = systemConfig.spi.cs_reg.CS_RPIN;
    uint8_t CS_PIN = systemConfig.spi.cs_reg.CS_PIN;

    switch (spi.spi_mode){
        case SPI_MASTER:
            DDRB |= (1<<PB5) | (1<<PB3) | (1<<PB2);
            DDRB &= ~(1<<PB4);
            PORTB |= (1<<PB5) | (1<<PB4) | (1<<PB3) | (1<<PB2);
            *CS_DDR |= (1<<CS_PIN);
            *CS_PORT |= (1<<CS_PIN);
            __builtin_avr_delay_cycles(3);
            if((PINB & (1<<PB5)) == 0 || (PINB & (1<<PB3)) == 0 || (PINB & (1<<PB2)) == 0 || (*CS_RPIN & (1<<CS_PIN)) == 0){return ERR_GPIO_INIT_FAIL;}
            __builtin_avr_delay_cycles(3);
            break;

        case SPI_MASTER_SLAVE:
            DDRB |= (1<<PB5) | (1<<PB3);
            DDRB &= ~(1<<PB4) & ~(1<<PB2);
            PORTB |= (1<<PB5) | (1<<PB4) | (1<<PB3) | (1<<PB2);
            *CS_DDR |= (1<<CS_PIN);
            *CS_PORT |= (1<<CS_PIN);
            __builtin_avr_delay_cycles(3);
            if((PINB & (1<<PB5)) == 0 || (PINB & (1<<PB3)) == 0 || (PINB & (1<<PB2)) == 0 || (*CS_RPIN & (1<<CS_PIN)) == 0){return ERR_GPIO_INIT_FAIL;}
            break;

        case SPI_SLAVE:
            DDRB |= (1<<PB4);
            DDRB &= ~(1<<PB5) & ~(1<<PB3) & ~(1<<PB2);
            PORTB |= (1<<PB5) | (1<<PB4) | (1<<PB3) | (1<<PB2);
            __builtin_avr_delay_cycles(3);
            if((PINB & (1<<PB4)) == 0){return ERR_GPIO_INIT_FAIL;}
            break;
            
        default:
            return ERR_INVALID_MODE;
    }
    return ERR_OK;
}

uint8_t spiConf(){
    SPI_CONF *spi = &systemConfig.spi.conf;
    uint8_t cpol = 0;
    uint8_t cpha = 0;
    switch (spi->clk_mode){
        case 0: cpol = 0; cpha = 0; break;
        case 1: cpol = 0; cpha = 1; break;
        case 2: cpol = 1; cpha = 0;break;
        case 3: cpol = 1; cpha = 1; break;
        default: return ERR_INVALID_MODE;
    }
    SPCR =  (SPCR & ~_BV(CPOL)) | ((cpol & 1u)<< CPOL);                  // Set Orientation for clock indle
    SPCR =  (SPCR & ~_BV(CPHA)) | ((cpha & 1u)<< CPHA);                  // Set clock phase

    uint8_t spr0 = 0;
    uint8_t spr1 = 0;
    uint8_t spr2x = 0;
    switch (spi->prescaler){
        case 2: spr1 = 0; spr0 = 0; spr2x = 1; break;
        case 4: spr1 = 0; spr0 = 0; spr2x = 0; break;
        case 8: spr1 = 0; spr0 = 1; spr2x = 1; break;
        case 16: spr1 = 0; spr0 = 1; spr2x = 0; break;
        case 32: spr1 = 1; spr0 = 0; spr2x = 1; break;
        case 64: spr1 = 1; spr0 = 0; spr2x = 0; break;
        case 128: spr1 = 1; spr0 = 1; spr2x = 0; break;
        default: return ERR_INVALID_MODE;
    }

    SPCR =  (SPCR & ~_BV(SPR1)) | ((spr1 & 1u)<< SPR1);                  // Adjust prescaler 
    SPCR =  (SPCR & ~_BV(SPR0)) | ((spr0 & 1u)<< SPR0);                  // Adjust prescaler 
    SPSR =  (SPSR & ~_BV(SPI2X)) | ((spr2x & 1u) << SPI2X);              // Adjust prescaler 
    return ERR_OK;
}

uint8_t spiTransmit(char *data, uint16_t len){
    //__ Safe Guard__
    if(data == NULL){return ERR_NULL_POINTER;}

    //__Safe Buffer Length Calc__
    uint16_t max = systemConfig.spi.conf.max_length;  
    uint16_t length = (len > max) ? max : len;
    systemConfig.spi.buffer_length = length;
    
    uint8_t status = ERR_OK;
    if(systemConfig.spi.conf.irq){
        //__Store&Send Data in SPI buffer if data is sent in Interrupt Mode__
        memcpy(systemConfig.spi.buffer, data, length); 
        status = spiTransmitInt();

    }else{status = spitTransmitPoll(data, length);}
    return (status == ERR_OK || status == SPI_IT_RUNNING_TRANSMIT)? ERR_OK: status;
}

void __spiWritePoll__(uint8_t *data, uint32_t len){
    for(uint16_t i=0; i< len; i++){
        SPDR = (uint8_t)data[i];        
        while(!(SPSR & (1<<SPIF))){}
    }     
}

uint8_t spitTransmitPoll(char *data, uint16_t length){
    uint8_t status = ERR_OK;
    //__Disable Interrupt__
    SPCR &= ~(1 << SPIE);
    
    if ((SPCR & (1 << MSTR)) != 0) {status = spiStart();}
    if(status != ERR_OK) {return status;}

    for(uint16_t i=0; i<length; i++){
        uint16_t count = 0;
        SPDR = (uint8_t)(data[i]);

        while(!(SPSR & (1<<SPIF))){
            __builtin_avr_delay_cycles(1);
            if(count++ > systemConfig.spi.conf.timeout){
                if ((SPCR & (1 << MSTR)) != 0){spiStop();}
                return ERR_TIMEOUT;
            }
        }
    }
    if ((SPCR & (1 << MSTR)) != 0) {status = spiStop();}
    return status;
}

uint8_t spiTransmitInt(){
    static uint8_t status = ERR_OK;

    //__Safe Guard__
    if(systemConfig.spi.buffer == NULL) {return ERR_NULL_POINTER;}
    if(systemConfig.spi.buffer_length == 0) {return ERR_SPI_EMPTY_BUFFER;}

    //__Pull CS Low if Master__
    if ((SPCR & (1 << MSTR)) != 0) {status = spiStart();}
    if(status != ERR_OK) {return status;}

    //__Start Interrupt__
    systemConfig.spi.index = 0;
    systemConfig.spi.interruptFLag = SPI_IT_RUNNING_TRANSMIT;
    SPCR |= (1 << SPIE);
    sei();
}

void __spiReadPoll__(uint8_t *data, uint16_t len){
    for(uint16_t i=0; i<len; i++){
        SPDR = 0x00;
        while(!(SPSR & (1<<SPIF))){}       // Check flag to confirm the data is ready to be read.
        data[i] = SPDR;                       // Get data from buffer
    }
}

uint8_t spiReceive(char *buffer, uint16_t len){
    if(buffer == NULL){return ERR_NULL_POINTER;}
    uint16_t max = systemConfig.spi.conf.max_length;  
    uint16_t length = (len > max) ? max : len;
    systemConfig.spi.buffer_length = length;
    uint8_t status = ERR_OK;
    
    if(systemConfig.spi.conf.irq){ptr_holder = buffer; status = spiReceiveInt();
    }else{status = spiReceivePoll(buffer, length);}
    return (status == ERR_OK || status == SPI_IT_RUNNING_RECEIVE)? ERR_OK: status;
}

uint8_t spiReceivePoll(char *data, uint16_t length){
    uint8_t status = ERR_OK;
    SPCR &= ~(1 << SPIE);
    
    mstrResolve(SPI_START);
    if(status != ERR_OK) {return status;}

    for(uint16_t i=0; i<length; i++){
        uint16_t count = 0;
         if ((SPCR & (1 << MSTR)) != 0) {SPDR = 0xff;}

        while(!(SPSR & (1<<SPIF))){
            __builtin_avr_delay_cycles(1);
            if(count++ > systemConfig.spi.conf.timeout){
                if ((SPCR & (1 << MSTR)) != 0){spiStop();}
                return ERR_TIMEOUT;
            }
        }
        data[i] = SPDR;
    }
    mstrResolve(SPI_STOP);
    return status;
}

uint8_t mstrResolve(uint8_t action){
    if(systemConfig.spi.conf.spi_mode == SPI_MASTER){
        if((SPCR & (1 << MSTR)) != 0){spiInit();}
        if(action == SPI_START){return spiStart();}
        else{return spiStop();}}
        else{return ERR_OK;}
}

uint8_t spiReceiveInt(){
    static uint8_t state = SPI_STATE_START;
    static uint8_t status = ERR_OK;
    static uint16_t tx_index = 0;
    if(state == SPI_STATE_START){
        if ((SPCR & (1 << MSTR)) != 0) {status = spiStart();}
        if(status != ERR_OK) {return status;}
        if(systemConfig.spi.buffer == NULL) {systemErrDecode(ERR_NULL_POINTER); return ERR_NULL_POINTER;}
        tx_index = 0;
        state = SPI_STATE_RUNNING;
        SPCR |= (1 << SPIE);
        sei();

        if ((SPCR & (1 << MSTR)) != 0){
            SPDR = 0XFF;
            systemConfig.spi.interruptFLag = SPI_IT_RUNNING_RECEIVE;
            return SPI_IT_RUNNING_RECEIVE;
        }
        startTimer();
    }

    if(state == SPI_STATE_RUNNING){
        uint8_t data = SPDR;
        if(data != 0x00){systemConfig.spi.buffer [tx_index++] = data;} 

        if(tx_index < systemConfig.spi.buffer_length && systemConfig.spi.interruptFLag != ERR_TIMEOUT){
            systemConfig.spi.interruptFLag = SPI_IT_RUNNING_RECEIVE;

            if ((SPCR & (1 << MSTR)) != 0){SPDR = 0xFF;}
            resetTimer();
            return SPI_IT_RUNNING_RECEIVE;
        }
        else{
            stopTimer();
            if ((SPCR & (1 << MSTR)) != 0) {status = spiStop();}
            tx_index = 0;
            state = SPI_STATE_START;
            memcpy(ptr_holder, systemConfig.spi.buffer, systemConfig.spi.buffer_length);
            systemConfig.spi.buffer_length = 100;
            systemConfig.spi.interruptFLag = SPI_IT_DONE;
            uartWrite_(systemConfig.spi.buffer);
            return SPI_IT_DONE;
        }
    }
    return ERR_SPI_INVALID_STATE;
}

uint8_t spiStart(){
    *systemConfig.spi.cs_reg.CS_PORT &= ~(1<<systemConfig.spi.cs_reg.CS_PIN);
    __builtin_avr_delay_cycles(1);
    uint8_t status = ((*systemConfig.spi.cs_reg.CS_RPIN & (1<<systemConfig.spi.cs_reg.CS_PIN)) == 0)? ERR_OK : ERR_CS_ENABLE_FAIL;
    if(status == ERR_CS_ENABLE_FAIL){uartWrite_("ERR_SPI_CS_ENABLE_FAIL\n");}
    return ERR_OK;
}

uint8_t spiStop(){
    *systemConfig.spi.cs_reg.CS_PORT |= (1<<systemConfig.spi.cs_reg.CS_PIN);
    __builtin_avr_delay_cycles(1);
    uint8_t status = ((*systemConfig.spi.cs_reg.CS_RPIN & (1<<systemConfig.spi.cs_reg.CS_PIN)) == 0)? ERR_CS_ENABLE_FAIL : ERR_OK;
    if(status == ERR_CS_ENABLE_FAIL){uartWrite_("ERR_SPI_CS_ENABLE_FAIL\n");}
    return ERR_OK;
}