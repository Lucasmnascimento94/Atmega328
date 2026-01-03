
#ifndef MYSYSTEMCODES_H
#define MYSYSTEMCODES_H

enum __attribute__((__packed__)) errCode {
    ERR_OK = 0,
    ERR_INIT_FAIL = 1,
    ERR_TIMEOUT = 2,
    ERR_INVALID_PARAM = 3,
    ERR_BUFFER_OVERFLOW = 4,
    ERR_NULL_POINTER = 5,
    ERR_INVALID_MODE = 6,
    ERR_GPIO_INIT_FAIL = 7,
    ERR_CS_ENABLE_FAIL = 8,
    ERR_EMPTY_BUFFER = 9,
    SPI_INVALID_STATE = 10,

    ERR_UART_INVALID_MODE = 11,
    ERR_SPI_BUSY = 12,
    ERR_UNKNOWN = 255,
};


enum __attribute__((__packed__)) spiCodes{
    // SPI CODES
    SPI_MSB = 0,
    SPI_DISABLE = 0,
    SPI_LSB = 1,
    SPI_ENABLE = 1,
    SPI_MODE_IRQ = 1,
    SPI_MASTER = 2,
    SPI_MASTER_SLAVE = 3,
    SPI_SLAVE = 4,
    SPI_MODE_POL = 10,

    // SPI STATE MACHINE
    SPI_STATE_START = 5,
    SPI_STATE_RUNNING = 6,
    SPI_IT_RUNNING_TRANSMIT = 7,
    SPI_IT_RUNNING_RECEIVE = 8,
    SPI_IT_DONE = 9,
    SPI_START = 10,
    SPI_STOP = 11,

        // SPI ERR_CODES
    ERR_SPI_EMPTY_BUFFER = 6,
    ERR_SPI_INVALID_STATE = 7,
};

enum __attribute__((__packed__)) i2cCodes {
    I2C_WRITE = 0,
    I2C_READ = 1,
    I2C_START_TRANSMITTED = 0X08,
    I2C_TWSR_FLAG_MASK = 0XF8,
    I2C_MODE_MASTER_POL = 0,
    I2C_MODE_MASTER_INT = 1,
    I2C_MODE_SLAVE_POL = 0X02,
    I2C_MODE_SLAVE_INT = 0X03,
    ERR_I2C_INIT_FAIL = 13,
    ERR_I2C_INVALID_MODE = 10,
};

enum __attribute__((__packed__)) sramCodes8{
    SRAM_MODE_BYTE = (uint8_t)(0x00),
    SRAM_MODE_PAGE = (uint8_t)(0x00 | (0x02<<6)),
    SRAM_MODE_SEQU = (uint8_t)(0x00 | (0x01<<6)),
    SRAM_MODE_RESE = (uint8_t)(0x00 | (0x03<<6)),

};

enum sramCodes32{
    SRAM_READ = (uint32_t)0x03, // Read data from memory array beginning at selected address
    SRAM_WRITE = (uint32_t)0X02, // Write data to memory array beginning at selected address
    SRAM_EDIO = (uint32_t)0X3B, // Enter Dual I/O access (enter SDI bus mode)
    SRAM_EQIO = (uint32_t)0X38, // Enter Quad I/O access (enter SQI bus mode)
    SRAM_RSTIO = (uint32_t)0XFF, // Reset Dual and Quad I/O access (revert to SPI bus mode)
    SRAM_RDMR = (uint32_t)0X05, // Read Mode Register
    SRAM_WRMR = (uint32_t)0X01, // Write Mode Register
};


#endif