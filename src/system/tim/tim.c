#include "tim.h"

/*
Formula:
    focnx = (fclk_io)/(2*n*(1+OCRnx))
    -> 2*n*(1+OCRnx) = (fclk_io) / focnx 
    (1+OCRnx) = (fclk_io) / (focnx*2*n) 

    OCRnx = ((fclk_io) / (focnx*2*n)) - 1  (n = prescaler)
*/
uint8_t timer_8bit_OCR_cal(uint16_t frequency, uint16_t prescaler)
{
    uint32_t clk = systemConfig.FOSC; 
    uint32_t denom = (uint32_t)frequency * (uint32_t)prescaler;
    if (denom == 0) { systemConfig.tim.errFlag = ERR_INVALID_PARAM; return 0; }

    uint32_t division = clk / denom;
    if (division == 0) { systemConfig.tim.errFlag = ERR_INVALID_PARAM; return 0; }

    uint32_t ocr32 = division - 1;
    if (ocr32 > 255) { systemConfig.tim.errFlag = ERR_INVALID_PARAM; return 255; }

    systemConfig.tim.errFlag = ERR_OK;
    return (uint8_t)ocr32;
}


uint8_t setTimer(){
    uartWriteToBinary(PRR);
    systemConfig.tim.tim0.OCR0A_VAL = (uint8_t)timer_8bit_OCR_cal(systemConfig.tim.tim0.frequency, systemConfig.tim.tim0.prescaler);
    if(systemConfig.tim.errFlag != ERR_OK){ return systemConfig.tim.errFlag;}
    
    OCR0A = systemConfig.tim.tim0.OCR0A_VAL;
    TCCR0A = 0X00;
    TCCR0A |= 0X02;
}

void startTimer(){
    switch(systemConfig.tim.tim0.prescaler){
        case 1: TCCR0B |= (1<<CS00); break;
        case 8: TCCR0B |= (1<<CS01); break;
        case 64: TCCR0B |= (1<<CS00) | (1<<CS01); break;
        case 256: TCCR0B |= (1<<CS02); break;
        case 1024: TCCR0B |= (1<<CS00) | (1<<CS02); break;
    }
    TCNT0  = 0; 
    TIMSK0 |= (1<<OCIE0A);

}

void resetTimer(){
    uint8_t sreg = SREG;        // save global interrupt state
    cli();

    TIMSK0 &= ~(1 << OCIE0A);   // disable compare A interrupt
    TIFR0  |=  (1 << OCF0A);    // clear pending compare flag
    TCNT0  = 0;                // reset counter
    TIMSK0 |=  (1 << OCIE0A);   // re-enable interrupt

    SREG = sreg;               // restore interrupt state
}

void stopTimer(){
    TCCR0B &= 0xF8;
    TIMSK0 &= ~(1<<OCIE0A);
    TCNT0 = 0X00;
}
