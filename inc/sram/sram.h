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
typedef struct{
    char buffer[100];
    uint8_t err_code;
}SRAM;
uint8_t sramWrite(uint8_t *data, uint16_t length, uint32_t address);
void sramWriteU16(uint16_t data, uint32_t address);
void sramWriteU32(uint32_t data, uint32_t address);

uint8_t sramRead(uint8_t *c, uint16_t length, uint32_t address);
void sramReadU16(uint16_t *data, uint32_t address);
void sramReadU32(uint32_t *data, uint32_t address);

uint8_t sramReadModeRegister();
void sramWriteModeRegister(uint8_t mode);

extern SRAM sram;
#endif