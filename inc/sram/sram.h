#ifndef SRAM_H
#define SRAM_H

#include "system.h"

/* Data Sample TIming

Data Sample -> Rising
SetUp       -> Falling

*/


/*#########################-- READING FROM SRAM --############################
############################################################################*/

/* Read Byte

1 - Instruction: SRAM_READ
2 - ADDRESS: 3 Bytes Address
3-  Read Data

*/


/*#########################-- WRITING ON SRAM --############################
############################################################################*/

/* Write Byte

1 - Instruction: SRAM_WRITE
2 - ADDRESS: 3 Bytes Address
3 - Write Byte

*/

void sendInstruction(uint32_t address);

void sramWriteByte(uint8_t data, uint32_t address);
void sramWriteU16(uint16_t data, uint32_t address);
void sramWriteU32(uint32_t data, uint32_t address);
void sramWriteStringPoll(char *data, uint32_t address, uint16_t size);

void sramReadByte(uint8_t *data, uint32_t address);
void sramReadU16(uint16_t *data, uint32_t address);
void sramReadU32(uint32_t *data, uint32_t address);
void sramReadString(uint8_t *data, size_t len, uint32_t address);
void sramReadBuffer(uint32_t size, uint32_t addr_start);

uint8_t sramReadModeRegister();
void sramWriteModeRegister(uint8_t mode);
#endif