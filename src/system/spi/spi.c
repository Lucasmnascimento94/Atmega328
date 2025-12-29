#include "system.h"

uint16_t timeout;
uint16_t max;
uint8_t lastByte;
char *buffer;
volatile uint8_t  *CS_DDR;
volatile uint8_t  *CS_PORT;
volatile uint8_t  *CS_RPIN;
uint8_t            CS_PIN;

uint8_t spiGPIO();
uint8_t spiConf();
uint8_t spitTransmitPoll(char *data, uint16_t length);
uint8_t spiTransmitInt(uint16_t length);
uint8_t spiReceivePoll(char *data, uint16_t length);
uint8_t spiReceiveInt(uint16_t length);

uint8_t spiInit(){

    uint8_t status;
    status = spiGPIO();
    if(status != ERR_OK){return status;}

    status = spiConf();
    if(status != ERR_OK){return status;}

    SPI_CONF *spi = &systemConfig.spi.conf;
    uint8_t mstr = spi->mode_conf.mstr;
    mstr = (mstr == SPI_MASTER || mstr == SPI_MASTER_SLAVE) ? 1 : 0;
    SPCR =  (SPCR & ~_BV(SPIE)) | ((spi->mode_conf.irq& 1u) << SPIE);        // Enable/Disable Interrupt Mode
    SPCR =  (SPCR & ~_BV(DORD)) | ((spi->mode_conf.lsbfirst & 1u)<< DORD);   // Set Data Orientation
    SPCR =  (SPCR & ~_BV(MSTR)) | ((mstr & 1u)<< MSTR);       // Enable/Disable Master Mode
    SPCR =  (SPCR & ~_BV(CPOL)) | ((spi->cpol & 1u)<< CPOL);                  // Set Orientation for clock indle
    SPCR =  (SPCR & ~_BV(CPHA)) | ((spi->cpha & 1u)<< CPHA);                  // Set clock phase
    SPCR =  (SPCR & ~_BV(SPR1)) | ((spi->spr1 & 1u)<< SPR1);                  // Adjust prescaler 
    SPCR =  (SPCR & ~_BV(SPR0)) | ((spi->spr0 & 1u)<< SPR0);                  // Adjust prescaler 
    SPSR =  (SPSR & ~_BV(SPI2X)) | ((spi->spr2x & 1u) << SPI2X);              // Adjust prescaler 

    SPCR =  (SPCR & ~_BV(SPE))  | ((spi->mode_conf.en & 1u)<< SPE);          // Enable/Disable SPI

    return ERR_OK;
}

uint8_t spiGPIO(){
    SPI_CONF spi = systemConfig.spi.conf;

    switch (spi.mode_conf.mstr){
        case SPI_MASTER:
            DDRB |= (1<<PB5) | (1<<PB3) | (1<<PB2);
            DDRB &= ~(1<<PB4);
            PORTB |= (1<<PB5) | (1<<PB4) | (1<<PB3) | (1<<PB2);
            _delay_us(1);
            if((PINB & (1<<PB5)) == 0 || (PINB & (1<<PB3)) == 0 || (PINB & (1<<PB2)) == 0){return ERR_SPI_GPIO_INIT_FAIL;}
            _delay_us(10);
            break;

        case SPI_MASTER_SLAVE:
            DDRB |= (1<<PB5) | (1<<PB3);
            DDRB &= ~(1<<PB4) & ~(1<<PB2);
            PORTB |= (1<<PB5) | (1<<PB4) | (1<<PB3) | (1<<PB2);
            _delay_us(1);
            if((PINB & (1<<PB5)) == 0 || (PINB & (1<<PB3)) == 0 || (PINB & (1<<PB2)) == 0){return ERR_SPI_GPIO_INIT_FAIL;}
            break;

        case SPI_SLAVE:
            DDRB |= (1<<PB4);
            DDRB &= ~(1<<PB5) & ~(1<<PB3) & ~(1<<PB2);
            PORTB |= (1<<PB5) | (1<<PB4) | (1<<PB3) | (1<<PB2);
            _delay_us(1);
            if((PINB & (1<<PB4)) == 0){return ERR_SPI_GPIO_INIT_FAIL;}
            break;
            
        default:
            return ERR_SPI_INVALID_MODE;
    }
    return ERR_OK;
}

uint8_t spiConf(){
    SPI_CONF *spi = &systemConfig.spi.conf;
    switch (spi->mode_conf.mode){
        case 0: spi->cpol = 0; spi->cpha = 0; break;
        case 1: spi->cpol = 0; spi->cpha = 1; break;
        case 2: spi->cpol = 1; spi->cpha = 0;break;
        case 3: spi->cpol = 1; spi->cpha = 1; break;
        default: return ERR_SPI_INVALID_MODE;
    }
    switch (spi->mode_conf.prescaler){
        case 2: spi->spr1 = 0; spi->spr0 = 0; spi->spr2x = 1; break;
        case 4: spi->spr1 = 0; spi->spr0 = 0; spi->spr2x = 0; break;
        case 8: spi->spr1 = 0; spi->spr0 = 1; spi->spr2x = 1; break;
        case 16: spi->spr1 = 0; spi->spr0 = 1; spi->spr2x = 0; break;
        case 32: spi->spr1 = 1; spi->spr0 = 0; spi->spr2x = 1; break;
        case 64: spi->spr1 = 1; spi->spr0 = 0; spi->spr2x = 0; break;
        case 128: spi->spr1 = 1; spi->spr0 = 1; spi->spr2x = 0; break;
        default: return ERR_SPI_INVALID_MODE;
    }

    timeout = systemConfig.spi.conf.timeout;
    max = systemConfig.spi.conf.max_length;
    buffer = systemConfig.spi.buffer;
    CS_DDR = systemConfig.spi.cs_reg.CS_DDR;
    CS_PORT = systemConfig.spi.cs_reg.CS_PORT;
    CS_RPIN = systemConfig.spi.cs_reg.CS_RPIN;
    CS_PIN = systemConfig.spi.cs_reg.CS_PIN;
    return ERR_OK;
}

uint8_t spiTransmit(char *data){
    if(data == NULL){return ERR_NULL_POINTER;}
    uint16_t max = systemConfig.spi.conf.max_length;  
    uint16_t length = (strlen(data) > max) ? max : strlen(data);
    uint8_t status = ERR_OK;

    if(systemConfig.spi.conf.mode_conf.irq){
        buffer = data;
        status = spiTransmitInt(length);
    }else{
        status = spitTransmitPoll(data, length);
    }
    return status;
}

void spiWritePoll_(uint8_t *data, uint32_t len){
    for(uint16_t i=0; i< len; i++){
        SPDR = (uint8_t)data[i];        
        while(!(SPSR & (1<<SPIF))){}
    }     
}

uint8_t spitTransmitPoll(char *data, uint16_t length){
    uint8_t status = ERR_OK;
    
    if ((SPCR & (1 << MSTR)) != 0) {status = spiStart();}
    if(status != ERR_OK) {return status;}

    for(uint16_t i=0; i<length; i++){
        uint16_t count = 0;
        SPDR = (uint8_t)(data[i]);

        while(!(SPSR & (1<<SPIF))){
            __builtin_avr_delay_cycles(1);
            if(count++ > timeout){
                if ((SPCR & (1 << MSTR)) != 0){spiStop();}
                return ERR_TIMEOUT;
            }
        }
        (void)SPDR;
        (void)SPSR;
    }
    if ((SPCR & (1 << MSTR)) != 0) {status = spiStop();}
    return status;
}


uint8_t spiTransmitInt(uint16_t length){
    static uint8_t state = SPI_STATE_START;
    static uint8_t status = ERR_OK;
    static uint16_t tx_length = 0;
    static uint16_t tx_index = 0;

    if(state == SPI_STATE_START){
        if ((SPCR & (1 << MSTR)) != 0) {status = spiStart();}
        if(status != ERR_OK) {return status;}
        tx_length = length;
        if(buffer == NULL) {return ERR_NULL_POINTER;}
        if(tx_length == 0) {return ERR_SPI_EMPTY_BUFFER;}
        tx_index = 0;
        state = SPI_STATE_RUNNING;
        SPCR |= (1 << SPIE);
    }

    if(state == SPI_STATE_RUNNING){
        if(tx_index < tx_length){
            systemConfig.spi.conf.interruptFLag = SPI_IT_RUNNING;
            SPDR = (uint8_t)(buffer[tx_index++]);
            return SPI_IT_RUNNING;
        }
        else{
            SPCR &= ~(1 << SPIE);
            if ((SPCR & (1 << MSTR)) != 0) {status = spiStop();}
            systemConfig.spi.conf.interruptFLag = SPI_IT_DONE;
            tx_index = 0;
            state = SPI_STATE_START;
            return SPI_IT_DONE;
        }
    }
    return ERR_SPI_INVALID_STATE;
}

void spiReadPoll_(uint8_t *data, uint16_t len){
    for(uint16_t i=0; i<len; i++){
        SPDR = 0x00;
        while(!(SPSR & (1<<SPIF))){}       // Check flag to confirm the data is ready to be read.
        data[i] = SPDR;                       // Get data from buffer
    }
}

uint8_t spiReceive(char *data){
    if(data == NULL){return ERR_NULL_POINTER;}
    uint16_t max = systemConfig.spi.conf.max_length;  
    uint16_t length = (strlen(data) > max) ? max : strlen(data);
    uint8_t status = ERR_OK;
    
    if(systemConfig.spi.conf.mode_conf.irq){
        buffer = data;
        status = spiReceiveInt(length);
    }else{
        status = spiReceivePoll(data, length);
    }
    return status;
}

uint8_t spiReceivePoll(char *data, uint16_t length){
    uint8_t status = ERR_OK;
    
    if ((SPCR & (1 << MSTR)) != 0) {status = spiStart();}
    if(status != ERR_OK) {return status;}

    for(uint16_t i=0; i<length; i++){
        uint16_t count = 0;
         if ((SPCR & (1 << MSTR)) != 0) {SPDR = 0xff;}

        while(!(SPSR & (1<<SPIF))){
            __builtin_avr_delay_cycles(1);
            if(count++ > timeout){
                if ((SPCR & (1 << MSTR)) != 0){spiStop();}
                return ERR_TIMEOUT;
            }
        }
        data[i] = SPDR;
    }
    if ((SPCR & (1 << MSTR)) != 0) {status = spiStop();}
    return status;
}

uint8_t spiReceiveInt(uint16_t length){
    static uint8_t state = SPI_STATE_START;
    static uint8_t status = ERR_OK;
    static uint16_t tx_length = 0;
    static uint16_t tx_index = 0;

    if(state == SPI_STATE_START){
        tx_length = length;
        
        if(buffer == NULL) { return ERR_NULL_POINTER; }
        if(tx_length == 0) { return ERR_SPI_EMPTY_BUFFER; }
        
        tx_index = 0;
        state = SPI_STATE_RUNNING;
        
        SPCR |= (1 << SPIE); 

        if ((SPCR & (1 << MSTR)) != 0) {
            status = spiStart();
            if(status != ERR_OK) { return status; }
            
            SPDR = 0xFF; 
            return SPI_STATE_RUNNING;
        }
        else {
            return SPI_STATE_RUNNING;
        }
    }

    if(state == SPI_STATE_RUNNING){
        
        buffer[tx_index++] = SPDR; 

        if(tx_index < tx_length){
            systemConfig.spi.conf.interruptFLag = SPI_IT_RUNNING;
            
            if ((SPCR & (1 << MSTR)) != 0){
                SPDR = 0xFF;
            }
            return SPI_IT_RUNNING;
        }
        else{
            SPCR &= ~(1 << SPIE); 
            
            if ((SPCR & (1 << MSTR)) != 0) {
                status = spiStop();
            }
            
            systemConfig.spi.conf.interruptFLag = SPI_IT_DONE;
            tx_index = 0;
            state = SPI_STATE_START;
            
            return SPI_IT_DONE;
        }
    }
    return ERR_SPI_INVALID_STATE;
}

uint8_t spiStart(){
    *CS_PORT &= ~(1<<CS_PIN);
    return ((*CS_RPIN & (1<<CS_PIN)) == 0)? ERR_OK : ERR_SPI_CS_ENABLE_FAIL;
}

uint8_t spiStop(){
    *CS_PORT |= (1<<CS_PIN);
    return ((*CS_RPIN & (1<<CS_PIN)) == 0)? ERR_SPI_CS_ENABLE_FAIL : ERR_OK;
}