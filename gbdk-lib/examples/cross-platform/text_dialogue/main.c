#include <gb/gb.h>
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


void DrawText(uint8_t column, uint8_t row, uint8_t columnWidth, char* text,uint8_t typewriterDelay){

    // Get the address of the first tile in the row
    uint8_t* vramAddress = get_bkg_xy_addr(column,row);

    uint8_t index=0;
    uint8_t columnIndex=0;

    while(text[index]!='\0'){

        char character = text[index];

        // Draw our character at the address
        // THEN, increment the address
        DrawCharacter(vramAddress++,character);

        index++;
        columnIndex++;

        // if we've reached the end of the row
        if(columnIndex>=columnWidth){
            
            // reset for the next row
            vramAddress = get_bkg_xy_addr(column,++row);
            
            columnIndex=0;
        }

        if(typewriterDelay>0){
            
            // Play a basic sound effect
            NR10_REG = 0x34;
            NR11_REG = 0x81;
            NR12_REG = 0x41;
            NR13_REG = 0x7F;
            NR14_REG = 0x86;

            // Wait some frames
            // This creats a typewriter effect
            for(uint8_t i=0;i<typewriterDelay;i++){

                vsync();
            }

        }
    }
}

void main2(void)
{
    SHOW_BKG;

    NR52_REG = 0x80; // Master sound on
    NR50_REG = 0xFF; // Maximum volume for left/right speakers. 
    NR51_REG = 0xFF; // Turn on sound fully

    set_bkg_data(0,Font_TILE_COUNT,Font_tiles);

    fill_bkg_rect(0,0,DEVICE_SCREEN_WIDTH,DEVICE_SCREEN_HEIGHT,0);

    // We'll pass zero for the final argument, to draw the text instantly
    DrawText(1,1,18,"GBDK Text Example",0);

    // By passing 3 as the final argument, the game boy will wait 3 frames between each character
    DrawText(1,3,DEVICE_SCREEN_WIDTH-2,"This is an how you draw text on the screen in GBDK. The code will automatically jump to a new line, when it reaches the end of the row.",3);
}
