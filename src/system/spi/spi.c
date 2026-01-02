#include "system.h"

static uint8_t init = false;
uint8_t *ptr_holder;

uint8_t spiGPIO();
uint8_t spiConf();
uint8_t spitTransmitPoll(char *data, uint16_t length);
uint8_t spiReceivePoll(char *data, uint16_t length);

uint8_t spiInit(){
    if(init){spiDeInit();}

    uint8_t status = spiConf();
    if(status != ERR_OK){uartWrite_("SPI_CONF_FAIL\n"); return status;}

    status = spiGPIO();
    if(status != ERR_OK){uartWrite_("SPI_GPIO_FAIL\n");  return status;}

    SPI_CONF *spi = &systemConfig.spi.conf;
    uint8_t mstr = spi->mode_conf.mstr;
    mstr = (spi->mode_conf.mstr == SPI_MASTER || mstr == SPI_MASTER_SLAVE) ? 1 : 0;
    SPCR =  (SPCR & ~_BV(SPIE)) | ((spi->mode_conf.irq& 1u) << SPIE);        // Enable/Disable Interrupt Mode
    SPCR =  (SPCR & ~_BV(DORD)) | ((spi->mode_conf.lsbfirst & 1u)<< DORD);   // Set Data Orientation
    SPCR =  (SPCR & ~_BV(MSTR)) | ((mstr & 1u)<< MSTR);       // Enable/Disable Master Mode
    SPCR =  (SPCR & ~_BV(CPOL)) | ((spi->cpol & 1u)<< CPOL);                  // Set Orientation for clock indle
    SPCR =  (SPCR & ~_BV(CPHA)) | ((spi->cpha & 1u)<< CPHA);                  // Set clock phase
    SPCR =  (SPCR & ~_BV(SPR1)) | ((spi->spr1 & 1u)<< SPR1);                  // Adjust prescaler 
    SPCR =  (SPCR & ~_BV(SPR0)) | ((spi->spr0 & 1u)<< SPR0);                  // Adjust prescaler 
    SPSR =  (SPSR & ~_BV(SPI2X)) | ((spi->spr2x & 1u) << SPI2X);              // Adjust prescaler 

    SPCR =  (SPCR & ~_BV(SPE))  | ((spi->mode_conf.en & 1u)<< SPE);          // Enable/Disable SPI

    if(spi->mode_conf.irq){sei();}
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

    switch (spi.mode_conf.mstr){
        case SPI_MASTER:
            DDRB |= (1<<PB5) | (1<<PB3) | (1<<PB2);
            DDRB &= ~(1<<PB4);
            PORTB |= (1<<PB5) | (1<<PB4) | (1<<PB3) | (1<<PB2);
            *CS_DDR |= (1<<CS_PIN);
            *CS_PORT |= (1<<CS_PIN);
            __builtin_avr_delay_cycles(3);
            if((PINB & (1<<PB5)) == 0 || (PINB & (1<<PB3)) == 0 || (PINB & (1<<PB2)) == 0 || (*CS_RPIN & (1<<CS_PIN)) == 0){return ERR_SPI_GPIO_INIT_FAIL;}
            __builtin_avr_delay_cycles(3);
            break;

        case SPI_MASTER_SLAVE:
            DDRB |= (1<<PB5) | (1<<PB3);
            DDRB &= ~(1<<PB4) & ~(1<<PB2);
            PORTB |= (1<<PB5) | (1<<PB4) | (1<<PB3) | (1<<PB2);
            *CS_DDR |= (1<<CS_PIN);
            *CS_PORT |= (1<<CS_PIN);
            __builtin_avr_delay_cycles(3);
            if((PINB & (1<<PB5)) == 0 || (PINB & (1<<PB3)) == 0 || (PINB & (1<<PB2)) == 0 || (*CS_RPIN & (1<<CS_PIN)) == 0){return ERR_SPI_GPIO_INIT_FAIL;}
            break;

        case SPI_SLAVE:
            DDRB |= (1<<PB4);
            DDRB &= ~(1<<PB5) & ~(1<<PB3) & ~(1<<PB2);
            PORTB |= (1<<PB5) | (1<<PB4) | (1<<PB3) | (1<<PB2);
            __builtin_avr_delay_cycles(3);
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
    return ERR_OK;
}

uint8_t spiTransmit(char *data, uint16_t len){
    //__ Sanity Check__
    if(data == NULL){return ERR_NULL_POINTER;}

    //__Safe Buffer Length Calc__
    uint16_t max = systemConfig.spi.conf.max_length;  
    uint16_t length = (len > max) ? max : len;
    systemConfig.spi.buffer_length = length;
    
    uint8_t status = ERR_OK;
    if(systemConfig.spi.conf.mode_conf.irq){
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
    static uint8_t state = SPI_STATE_START;
    static uint8_t status = ERR_OK;
    static uint16_t tx_index = 0;

    if(state == SPI_STATE_START){
        if ((SPCR & (1 << MSTR)) != 0) {status = spiStart();}
        if(status != ERR_OK) {return status;}
        if(systemConfig.spi.buffer == NULL) {return ERR_NULL_POINTER;}
        if(systemConfig.spi.buffer_length == 0) {return ERR_SPI_EMPTY_BUFFER;}
        tx_index = 0;
        state = SPI_STATE_RUNNING;
        SPCR |= (1 << SPIE);
        sei();
    }

    if(state == SPI_STATE_RUNNING){
        if(tx_index < systemConfig.spi.buffer_length){
            systemConfig.spi.interruptFLag = SPI_IT_RUNNING_TRANSMIT;
            SPDR = (uint8_t)(systemConfig.spi.buffer[tx_index++]);
            return SPI_IT_RUNNING_TRANSMIT;
        }
        else{
            if ((SPCR & (1 << MSTR)) != 0) {status = spiStop();}
            systemConfig.spi.interruptFLag = SPI_IT_DONE;
            tx_index = 0;
            state = SPI_STATE_START;
            return SPI_IT_DONE;
        }
    }
    return ERR_SPI_INVALID_STATE;
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
    
    if(systemConfig.spi.conf.mode_conf.irq){ptr_holder = buffer; status = spiReceiveInt();
    }else{status = spiReceivePoll(buffer, length);}
    return (status == ERR_OK || status == SPI_IT_RUNNING_RECEIVE)? ERR_OK: status;
}

uint8_t spiReceivePoll(char *data, uint16_t length){
    uint8_t status = ERR_OK;
    SPCR &= ~(1 << SPIE);
    
    if ((SPCR & (1 << MSTR)) != 0) {status = spiStart();}
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
    if ((SPCR & (1 << MSTR)) != 0) {status = spiStop();}
    return status;
}

uint8_t spiReceiveInt(){
    static uint8_t state = SPI_STATE_START;
    static uint8_t status = ERR_OK;
    static uint16_t tx_index = 0;

    if(state == SPI_STATE_START){
        if ((SPCR & (1 << MSTR)) != 0) {status = spiStart();}
        if(status != ERR_OK) {return status;}
        if(systemConfig.spi.buffer == NULL) {return ERR_NULL_POINTER;}
        tx_index = 0;
        state = SPI_STATE_RUNNING;
        SPCR |= (1 << SPIE);
        sei();

        if ((SPCR & (1 << MSTR)) != 0){
            SPDR = 0XFF;
            systemConfig.spi.interruptFLag = SPI_IT_RUNNING_RECEIVE;
            return SPI_IT_RUNNING_RECEIVE;
        }
    }

    if(state == SPI_STATE_RUNNING){
        systemConfig.spi.buffer [tx_index++] = SPDR; 

        if(tx_index < systemConfig.spi.buffer_length){
            systemConfig.spi.interruptFLag = SPI_IT_RUNNING_RECEIVE;
            if ((SPCR & (1 << MSTR)) != 0){SPDR = 0xFF;}
            return SPI_IT_RUNNING_RECEIVE;
        }
        else{
            if ((SPCR & (1 << MSTR)) != 0) {status = spiStop();}
            tx_index = 0;
            state = SPI_STATE_START;
            memcpy(ptr_holder, systemConfig.spi.buffer, systemConfig.spi.buffer_length);
            systemConfig.spi.buffer_length = 0;
            (void)SPSR;
            (void)SPDR;
            systemConfig.spi.interruptFLag = SPI_IT_DONE;
            return SPI_IT_DONE;
        }
    }
    return ERR_SPI_INVALID_STATE;
}

uint8_t spiStart(){
    *systemConfig.spi.cs_reg.CS_PORT &= ~(1<<systemConfig.spi.cs_reg.CS_PIN);
    __builtin_avr_delay_cycles(1);
    uint8_t status = ((*systemConfig.spi.cs_reg.CS_RPIN & (1<<systemConfig.spi.cs_reg.CS_PIN)) == 0)? ERR_OK : ERR_SPI_CS_ENABLE_FAIL;
    if(status == ERR_SPI_CS_ENABLE_FAIL){uartWrite_("ERR_SPI_CS_ENABLE_FAIL\n");}
    return ERR_OK;
}

uint8_t spiStop(){
    *systemConfig.spi.cs_reg.CS_PORT |= (1<<systemConfig.spi.cs_reg.CS_PIN);
    __builtin_avr_delay_cycles(1);
    uint8_t status = ((*systemConfig.spi.cs_reg.CS_RPIN & (1<<systemConfig.spi.cs_reg.CS_PIN)) == 0)? ERR_SPI_CS_ENABLE_FAIL : ERR_OK;
    if(status == ERR_SPI_CS_ENABLE_FAIL){uartWrite_("ERR_SPI_CS_ENABLE_FAIL\n");}
    return ERR_OK;
}