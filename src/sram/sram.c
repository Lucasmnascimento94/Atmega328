#include "sram.h"


void sendInstruction(uint32_t address){
    uint8_t byte[4] = {};
     for(int i=0; i<4; i++){
        byte[i] = (uint8_t)((address >> ((3 - i) * 8)) & 0xFF);
     }
     spiWritePoll_(byte, 4);
}


/* Writing One Byte Process:
>> 1byte: command Instruction [SRAM_WRITE]
>> 3bytes: 24 bit address
>> 1byte: Data out
*/
void sramWriteByte(uint8_t c, uint32_t address){
    address = (((uint32_t)SRAM_WRITE) << 24) | address; // Combine Command instruction to the address data
    spiStart();
    sendInstruction(address); 
    spiWritePoll_(&c, 1);
    spiStop(); 
}

void sramWriteU16(uint16_t data, uint32_t address){
    sramWriteByte((uint8_t)(data>>8), address);
    sramWriteByte((uint8_t)data, address  + 1);
}

void sramWriteU32(uint32_t data, uint32_t address){
    sramWriteByte((uint8_t)(data>>24), address);
    sramWriteByte((uint8_t)(data >>16), address + 1);
    sramWriteByte((uint8_t)(data>>8), address + 2);
    sramWriteByte((uint8_t)data, address + 3);
}


/* Writing One Byte Process:
>> 1byte: command Instruction [SRAM_WRITE]
>> 3bytes: 24 bit address
>> 1byte: Data Stream
*/
void sramWriteStringPoll(char *data, uint32_t address, uint16_t size){
    address = (((uint32_t)SRAM_WRITE) << 24) | address;
    spiStart();
    sendInstruction(address); 
    spiWritePoll_((uint8_t *)data, size);
    spiStop(); 
}


/* Writing One Byte Process:spiWritePoll
>> 1byte: command Instruction [SRAM_READ]
>> 3bytes: 24 bit address
>> 1byte: Data out
*/
void sramReadByte(uint8_t *c, uint32_t address){
    address = (((uint32_t)SRAM_READ) << 24) | address; // Combine Command instruction to the address data
    *c = 0x00;

    spiStart();                        // CS low
    sendInstruction(address);          // Send instruction + address
    SPDR = 0x00;
    while(!(SPSR & (1<<SPIF))){}       // Check flag to confirm the data is ready to be read.
    *c = SPDR;                       // Get data from buffer
    spiStop();   
}

void sramReadU16(uint16_t *data, uint32_t address){
    *data = 0x00;
    uint8_t byte = 0x00;
    sramReadByte(&byte, address);
    *data |= ((uint16_t) byte) << 8;
    sramReadByte(&byte, address  + 1);
    *data |= ((uint16_t) byte);
}

void sramReadU32(uint32_t *data, uint32_t address){
    *data = 0x00;
    uint8_t byte = 0x00;
    sramReadByte(&byte, address);
    *data |= ((uint32_t) byte) << 24;
    sramReadByte(&byte, address  + 1);
    *data |= ((uint32_t) byte) << 16;
    sramReadByte(&byte, address  + 2);
    *data |= ((uint32_t) byte) << 8;
    sramReadByte(&byte, address  + 3);
    *data |= ((uint32_t) byte);
}

void sramReadString(uint8_t *buffer, size_t len, uint32_t address){
    address = (((uint32_t)SRAM_READ) << 24) | address;
    spiStart();
    sendInstruction(address); 
    spiReadPoll_((uint8_t *)buffer, len);
    spiStop(); 
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