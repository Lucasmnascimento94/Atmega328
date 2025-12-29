#include "system.h"
#include "screen.h"
#include "sram_map.h"

void sramtesting();


SCREEN screen;
int main(void){
    char *msg = "hello here\n";
    if(systemInit() != ERR_OK) {return 0;}
    uartWrite_("passed system\n");
    screenInit(&screen, true);
    screenWrite(&screen, "hello from the new API");
    sramtesting();
    while(true){
        uartWrite_(msg);
        _delay_ms(1000);
    }
}


void sramtesting(){
    sram_map.cmd.arg1 = 0xAA;
    sram_map.cmd.arg2 = 0XBB;
    sram_map.cmd.arg3 = 0XCC;

    sram_map.score.current_score = 0X81;
    sram_map.score.record_score = 0X99;
    strcpy((char *)sram_map.score.game_name, "SNAKE GAME");
    strcpy((char *)sram_map.score.player_name, "LUCAS NASC");
    
    loadScore();
    //loadCommand();
    sram_map.cmd.cmdID = 0;
    sram_map.cmd.arg1 = 0;
    sram_map.cmd.arg2 = 0;
    sram_map.cmd.arg3 = 0;

    sram_map.score.current_score = 0;
    sram_map.score.record_score = 0;
    memset(sram_map.score.game_name, 0, 12);
    memset(sram_map.score.player_name, 0, 12);

    getCommand();
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
