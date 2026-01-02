#ifndef SPI_HPP
#define SPI_HPP

#include <stdint.h>

uint8_t spiInit();
uint8_t spiDeInit();
uint8_t spiTransmit(char *data, uint16_t length);
uint8_t spiTransmitInt();
uint8_t spiReceive(char *data, uint16_t len);
uint8_t spiReceiveInt();
uint8_t spiStart();
uint8_t spiStop();
void __spiWritePoll__(uint8_t *data, uint32_t len);
void __spiReadPoll__(uint8_t *buffer, uint16_t len);

#endif // MY_FILENAME_HPP