#ifndef SPI_HPP
#define SPI_HPP

#include <stdint.h>

uint8_t spiInit();
uint8_t spiDeInit();
uint8_t spiTransmit(char *data);
uint8_t spiReceive(char *buffer);
uint8_t spiStart();
uint8_t spiStop();
void spiWritePoll_(uint8_t *data, uint32_t len);
void spiReadPoll_(uint8_t *buffer, uint16_t len);

#endif // MY_FILENAME_HPP