
#ifndef MYSYSTEM_H
#define MYSYSTEM_H
#include "types.h"
#include "spi.h"
#include "i2c.h"
#include "uart.h"
#define SYSTEMDEBUG 0


uint8_t systemInit();

#if SYSTEMDEBUG == 1
void systemDisplay();
#endif
#endif
/*Default*/