#include "sram.h"
SRAM sram = {
    .buffer = {0},
    .err_code = 0,
};
#define SRAM_INSTRUCTION_SIZE 4

void buildInstruction(uint32_t address, uint8_t cmd){
    uint32_t instruction = (((uint32_t)cmd) << 24) | address;
    int i=0;
     for(;i<SRAM_INSTRUCTION_SIZE; i++){
        sram.buffer[i] = (uint8_t)((instruction >> ((3 - i) * 8)) & 0xFF);
     }
}

uint8_t sramWrite(uint8_t *c, uint16_t length, uint32_t address){
    while(systemConfig.spi.interruptFLag == SPI_IT_RUNNING_RECEIVE || systemConfig.spi.interruptFLag == SPI_IT_RUNNING_TRANSMIT){__builtin_avr_delay_cycles(1);}
    memset(sram.buffer, 0, sizeof(sram.buffer));

    buildInstruction(address, SRAM_WRITE);
    uint8_t data_len = (length > (sizeof(sram.buffer) - SRAM_INSTRUCTION_SIZE))? sizeof(sram.buffer) - SRAM_INSTRUCTION_SIZE : length;
    memcpy(&sram.buffer[SRAM_INSTRUCTION_SIZE], c, data_len);

    return spiTransmit((char *)sram.buffer, data_len + SRAM_INSTRUCTION_SIZE);
}

void sramWriteU16(uint16_t data, uint32_t address){

    uint8_t data_[2];
    data_[0] = (uint8_t)(data>>8);
    data_[1] = (uint8_t)(data);
    sramWrite(data_, 2,address);
}

void sramWriteU32(uint32_t data, uint32_t address){
    uint8_t data_[4];

    data_[0] = (uint8_t)(data>>24);
    data_[1] = (uint8_t)(data>>16);
    data_[2] = (uint8_t)(data>>8);
    data_[3] = (uint8_t)(data);
    sramWrite(data_, 4,address);
}

/* Writing One Byte Process:spiWritePoll
>> 1byte: command Instruction [SRAM_READ]
>> 3bytes: 24 bit address
>> 1byte: Data out
*/

uint8_t sramRead(uint8_t *buffer, uint16_t length, uint32_t address){
    while(systemConfig.spi.interruptFLag == SPI_IT_RUNNING_RECEIVE || systemConfig.spi.interruptFLag == SPI_IT_RUNNING_TRANSMIT){__builtin_avr_delay_cycles(1);}
    memset(sram.buffer, 0, sizeof(sram.buffer));
    buildInstruction(address, SRAM_READ);

    cli();
    spiStart();
    __spiWritePoll__((uint8_t *)sram.buffer, SRAM_INSTRUCTION_SIZE);

    uint8_t data_len = (length > (sizeof(sram.buffer) - SRAM_INSTRUCTION_SIZE))? sizeof(sram.buffer) : length;
    memcpy(&sram.buffer[0], buffer, data_len);
    return spiReceive((char *)buffer, data_len); 
}

void sramReadU16(uint16_t *buffer, uint32_t address){
    uint8_t isr = systemConfig.spi.conf.irq;
    systemConfig.spi.conf.irq = 0;
    cli();
    *buffer = 0x00;
    uint8_t data[2] = {0};
    uint8_t status = sramRead(data, 2, address);
    *buffer = ((uint16_t) data[0]) << 8;
    *buffer |= ((uint16_t) data[1]);
    if(isr){systemConfig.spi.conf.irq = 1; sei();}
}

void sramReadU32(uint32_t *buffer, uint32_t address){
    uint8_t isr = systemConfig.spi.conf.irq;
    systemConfig.spi.conf.irq = 0;
    cli();
    *buffer = 0x00;
    uint8_t data[4] = {0};
    uint8_t status = sramRead(data, 4,address);
    if(status == SPI_IT_RUNNING_RECEIVE) while(systemConfig.spi.interruptFLag != SPI_IT_DONE){_delay_us(1);}
    *buffer = ((uint32_t) data[0]) << 24;
    *buffer |= ((uint32_t) data[1]) << 16;
    *buffer |= ((uint32_t) data[2]) << 8;
    *buffer |= ((uint32_t) data[3]);    
    if(isr){systemConfig.spi.conf.irq = 1; sei();}
}

uint8_t sramReadModeRegister(){
   uint8_t data = 0x00;
    spiStart();                         // CS low
    SPDR = (uint8_t)(SRAM_RDMR & 0xFF);
    while(!(SPSR & (1<<SPIF))){}       // Check flag to confirm the data is ready to be read.
    SPDR = 0x00;
    while(!(SPSR & (1<<SPIF))){}
    data = SPDR;                       // Get data from buffer
    spiStop();  
    return data;  
}

void sramWriteModeRegister(uint8_t mode){
    //uint8_t data = 0x00;

    if(mode != SRAM_MODE_BYTE && mode != SRAM_MODE_PAGE \
       && mode != SRAM_MODE_SEQU && mode != SRAM_MODE_RESE){
        return;
    }
    spiStart();                         // CS low
    SPDR = (uint8_t)SRAM_WRMR;
    while(!(SPSR & (1<<SPIF))){}       // Check flag to confirm the data is ready to be read.
    SPDR = mode;
    while(!(SPSR & (1<<SPIF))){}
    //data = SPDR;                       // Get data from buffer
    spiStop();   
}