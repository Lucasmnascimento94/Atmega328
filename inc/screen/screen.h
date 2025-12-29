#ifndef SCREEN_H
#define SCREEN_H
#ifdef __cplusplus
extern "C" {
#endif
#include <types.h>

enum __attribute__((__packed__)) screenCodes{
    RS_BIT = 0,
    RW_BIT = 1,
    E_BIT = 2,
    BT_BIT = 3,

    DL_BIT = 4,
    N_BIT = 3,
    F_BIT = 2,
    S_C_BIT = 3,
    R_L_BIT = 2,
    D_BIT = 2,
    C_BIT = 1,
    B_BIT = 0,
    I_D_BIT = 1,
    SH_BIT = 0,
    R_W_BIT = 1,
    RAM_BIT = 7,

    CLEAR_DISPLAY = 0x01,
    HOME = 0x02,

    READ_BUSY_FLAG = 0x01,
    WRITE_TO_RAM = 0x02,
    READ_FROM_RAM = 0x03,
};

#define ENTRY_MODE(I_D, SH)          ((0X04) | (I_D << I_D_BIT) | (SH<<SH_BIT))
#define DISPLAY_ON_OFF(D, C, B)      ((0X08) | (D<<D_BIT) | (C<<C_BIT) | (B<<B_BIT))
#define CURSOR(S_C, R_L)             ((0X10) | (S_C<<S_C_BIT) | (R_L<<R_L_BIT))
#define FUNCTION_SET(DL, N, F)       ((0X20) | (DL<<DL_BIT) | (N<<N_BIT) | (F<<F_BIT))

typedef struct {
    uint8_t I_D, SH;   // Entry mode
    uint8_t D, C, B;   // Display on/off, cursor, blink
    uint8_t S_C, R_L;  // Shift command fields (rarely used at init)
    uint8_t DL, N, F;  // Function set
}SCREEN_CONF;

typedef struct{
    uint8_t pcf8574_addr;
    uint8_t current_row;
    uint8_t current_column;
    SCREEN_CONF conf;
}SCREEN;

void screenInit(SCREEN *screen, bool default_conf);
void screenWrite(SCREEN *screen, char *buffer);
void setCursor(uint8_t pos, uint8_t pcf_address);
uint8_t getAddress(uint8_t row, uint8_t column);

#ifdef __cplusplus
}
#endif
#endif
