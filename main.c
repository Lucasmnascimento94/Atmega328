#include "system.h"
#include "screen.h"
#include "sram_map.h"
#include "isr.h"
void sramtesting();


SCREEN screen;
int main(void){
    char *msg = "hello here\n";
    if(systemInit() != ERR_OK) {return 0;}
    systemDisplay();


    screenInit(&screen, true);
    _delay_ms(100);
    screenWrite(&screen, "hello");
    //sramtesting();

    _delay_ms(500);
    //sramWrite("TALKING TO SRAM\n", 16, 0x00);
    uint8_t s[20];
    //sramRead(&s, 16, 0x00);
    while(systemConfig.spi.interruptFLag != SPI_IT_DONE){_delay_us(1);}
    uartWrite_(s);

    while(true){
        uartWrite_(systemConfig.spi.buffer);
        uartWrite_("\n");
        _delay_ms(500);
    }
}


void sramtesting(){

    sram_map.score.current_score = 0X81;
    sram_map.score.record_score = 0X99;
    strcpy((char *)sram_map.score.game_name, "SNAKE GAME");
    strcpy((char *)sram_map.score.player_name, "LUCAS NASC");
    
    loadScore();

    sram_map.score.current_score = 0;
    sram_map.score.record_score = 0;
    memset(sram_map.score.game_name, 0, 12);
    memset(sram_map.score.player_name, 0, 12);

    getScore();
    uartWrite_("GAME NAME: ");
    _delay_us(200);
    uartWrite_((char *)sram_map.score.game_name);
    _delay_us(200);
    uartWrite_("\nPLAYER NAME: ");
    _delay_us(200);
    uartWrite_((char *)sram_map.score.player_name);
    uartWrite_("\n");
}
