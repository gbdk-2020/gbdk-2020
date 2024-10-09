#include <gbdk/platform.h>
#include <stdint.h>
#include "Font.h"

void DrawCharacter(uint8_t* address, char character) {

    uint8_t vramTile=0;

    // Char's can be interpreted as integers
    // We don't need to map every alpha-numeric character
    // We can use basic math to simplify A-Z and 0-9
    if((character>='a'&&character<='z')||(character>='A'&&character<='Z'))vramTile = (character-'A')+1;
    else if(character>='0'&&character<='9')vramTile = (character-'0')+27;
    else if(character=='!')vramTile = 37;
    else if(character==':')vramTile = 38;
    else if(character=='?')vramTile = 39;
    else if(character=='/')vramTile = 40;
    else if(character=='=')vramTile = 41;
    else if(character==',')vramTile = 42;
    else if(character=='.')vramTile = 43;
    else if(character=='<')vramTile = 44;
    else if(character=='>')vramTile = 45;

    set_vram_byte(address,vramTile);
}


void DrawText(uint8_t column, uint8_t row, char* text){

    // Get the address of the first tile in the row
    uint8_t* vramAddress = get_bkg_xy_addr(column,row);

    uint8_t index=0;

    while(text[index]!='\0'){

        char character = text[index];

        // Draw our character at the address
        // THEN, increment the address
        DrawCharacter(vramAddress++,character);

        index++;

    }
}

void main(void)
{
    SHOW_BKG;

    NR52_REG = 0x80; // Master sound on
    NR50_REG = 0xFF; // Maximum volume for left/right speakers. 
    NR51_REG = 0xFF; // Turn on sound fully

    set_bkg_data(0,Font_TILE_COUNT,Font_tiles);

    fill_bkg_rect(0,0,DEVICE_SCREEN_WIDTH,DEVICE_SCREEN_HEIGHT,0);

    // We'll pass zero for the final argument, to draw the text instantly
    DrawText(1,1,"GBDK Text Example");
}
