
#ifndef TYPES_H
#define TYPES_H
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "string.h"
#include "systemCodes.h"


/*==============================================================================
 *  SPI MODULE CONFIGURATION
 *==============================================================================
 *  Reference:
 *    - SPI Application Note : ./documentation/protocols/SPI.md
 *    - SRAM Application Note: ./documentation/grid/SRAM.md
 *
 *  Description:
 *    This section contains data types for SPI configuration.
 *
 *============================================================================*/
typedef struct {
  volatile uint8_t  *CS_DDR;
  volatile uint8_t  *CS_PORT;
  volatile uint8_t  *CS_RPIN;
  uint8_t            CS_PIN;
}SPI_CS_TARGET;

typedef struct{
  uint8_t mode; // 0,1,2,3  (SRAM likes 0)
  uint8_t lsbfirst; // 1=MSB first, 0=LSB first
  uint8_t prescaler; // 2,4,8,16,32,64,128
  uint8_t irq; // 1=use SPI interrupt, 0=poll 
  uint8_t en;  // SPI Enable(1) Disable(0)
  uint8_t mstr; // SPI MODES
}SPI_MODE;

typedef struct{
  uint8_t cpol;
  uint8_t cpha;
  uint8_t spr2x;
  uint8_t spr1;
  uint8_t spr0;
  uint16_t timeout;
  uint16_t max_length;
  SPI_MODE mode_conf;
  uint8_t interruptFLag;
}SPI_CONF;

typedef struct {
  SPI_CONF conf;
  SPI_CS_TARGET  cs_reg;
  char buffer[100];
}SPI;


/*==============================================================================
 *  I2C MODULE CONFIGURATION
 *==============================================================================
    Reference:
        - I2C Application Note : ./documentation/protocols/I2C.md
    
    Description:
        This section contains data types for I2C configuration.
 *
 *============================================================================*/
/*──────────── Macros ────────────*/
#define I2C_WRITE 0
#define I2C_READ  1
#define I2C_TWSR_FLAG_MASK 0XF8

#define MODE_MASTER_POL 0X00
#define MODE_MASTER_INT 0X01
#define MODE_SLAVE_POL  0X10
#define MODE_SLAVE_INT  0X11

/*────────── TWSR status codes (master TX subset) ─────────*/
#define START_TRANSMITTED               0X08
#define START_RETRANSMITTED             0X10
#define SLA_PLUS_W_ACK                  0X18
#define SLA_PLUS_W_NOT_ACK              0X20
#define DATA_BYTE_TRANSMITTED_ACK       0X28
#define DATA_BYTE_TRANSMITTED_NO_ACK    0X30
#define ARBITRATION_LOST                0X38

/*────────── Config struct ─────────*/
typedef struct {
    uint32_t TWBR_VAL;          /* Optional precomputed TWBR; 0 to compute via f_cpu/frequency/prescaler */
    uint32_t frequency;         /* Target SCL frequency (Hz) */
    uint8_t prescaler;          /* 1, 4, 16, or 64 */
    uint8_t mode;               /* MODE_* */
}I2C;


/*==============================================================================
 *  UART MODULE CONFIGURATION
 *==============================================================================
 *  Reference:
 *    - UART Application Note : ./documentation/protocols/UART.md
 *============================================================================*/
typedef struct{
    uint16_t baudrate;
    uint8_t  frame_size; // 5,6,7,8,9 bits
    uint8_t  parity;     // 0=none, 1=even, 2=odd
    uint8_t  stop_bits;  // 1 or 2
    uint8_t  irq_enable; // 0=disable, 1=enable
    uint32_t MYUBRR;
}UART;


typedef struct{
    I2C i2c;
    SPI spi;
    UART uart;
    uint32_t FOSC;
}SYSTEM;

extern SYSTEM systemConfig;

#endif