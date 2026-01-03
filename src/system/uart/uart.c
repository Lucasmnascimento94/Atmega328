#include "uart.h"
uint32_t default_baudrate = 9600UL;

void uartWrite(char *c, uint16_t size){
    for(uint16_t i=0; i<size; i++){
        while(!( UCSR0A & (1<<UDRE0))){
            __builtin_avr_delay_cycles(1);
        }
        UDR0 = ((uint8_t)c[i]);
    }
}


void uartWrite_(char *c){
    for(uint16_t i=0; i<strlen(c); i++){
        while(!( UCSR0A & (1<<UDRE0))){
            __builtin_avr_delay_cycles(1);
        }
        UDR0 = ((uint8_t)c[i]);
    }
}

void uartWriteToBinary(uint8_t val){
    char c[20] = {0};
    for(int i=0; i<8; i++){
        if(val & 0x80){strcat(c, "1");}
        else{strcat(c, "0");}
        val = val << 1;
    }
    strcat(c, "\n");
    uartWrite_(c);
}

void calcMYUBRR(){
    uint32_t baud = systemConfig.uart.baudrate;
    uint32_t fosc = systemConfig.FOSC;
    systemConfig.uart.MYUBRR = (uint16_t) ( ((fosc + (8UL * baud)) / (16UL * baud)) - 1 );
}

void uartGPIO(){
    DDRD |= (1<<PD1);
    DDRD &= !(1<<PD0);
    PORTD |= (1<< PD1) | (1<<PD0);
}

uint8_t uartInit(){
    uartGPIO();
    calcMYUBRR();
    uint32_t MYUBRR = systemConfig.uart.MYUBRR;

    UBRR0H = (unsigned char)(MYUBRR>>8);
    UBRR0L = (unsigned char)MYUBRR;

    UCSR0C &= ~((1<<UMSEL01)|(1<<UMSEL00) | (1<<UPM01) | (1<<UPM00)); // Asynchronous Mode
    UCSR0C |= (1<<USBS0);// 2-STOP BITS
    
    UCSR0A &= ~(1<<U2X0); 

    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    /* Set frame format: 8data, 2stop bit */
   
    /*8 BIT CHARACTER*/
    UCSR0B &= ~(1<<UCSZ02);
    UCSR0C |= (1<<UCSZ01) |(1<<UCSZ00);

    return ERR_OK;
}