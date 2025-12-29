#ifndef I2C_H
#define I2C_H
#include "types.h"

/*___________CLOCK & GPIO ____________*/
 uint8_t i2cInit();

/*______________HELPERS_______________*/
uint8_t i2cStartPol(uint8_t address, uint8_t mode);
uint8_t i2cStop();

/*_________Writing Polling___________*/
uint8_t i2cWritePol(char *buffer, size_t size, uint8_t address);
uint8_t i2cWritePol_(char *buffer, size_t size);

/*_________Reading Polling___________*/
uint8_t i2cReadPol(char *data, size_t size, uint8_t address);
uint8_t i2cReadPol_(char *buffer, size_t size);
#endif