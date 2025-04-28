#include <gbdk/platform.h>
#include <stdint.h>
#include "Font.h"

#define THREE_FRAMES 3

uint8_t GetCharacterVRamTile(char character) {

    uint8_t vramTile=0;


    // Char's can be interpreted as integers
    // We don't need to map every alpha-numeric character
    // We can use basic math to simplify A-Z and 0-9
    if      (character >= 'a' && character <= 'z') vramTile = (character - 'a') + 1;
    else if (character >= 'A' && character <= 'Z') vramTile = (character - 'A') + 1;
    else if (character >= '0' && character <= '9') vramTile = (character - '0') + 27;
    else {
        switch(character) {
            case '!': vramTile = 37; break;
            case ':': vramTile = 38; break;
            case '?': vramTile = 39; break;
            case '/': vramTile = 40; break;
            case '=': vramTile = 41; break;
            case ',': vramTile = 42; break;
            case '.': vramTile = 43; break;
            case '<': vramTile = 44; break;
            case '>': vramTile = 45; break;
        }
    }

    return vramTile;

}

void DrawText( char* text,uint8_t typewriterDelay){

    uint8_t column = 0;
    uint8_t row = 0;

    // Get the address of the first tile in the row
    uint8_t* vramAddress = get_bkg_xy_addr(column,row);

    uint16_t index=0;
    uint8_t columnIndex=0;

    while(text[index]!='\0'){

        char character = text[index];

        // Get the vram tile to draw
        uint8_t vramTile = GetCharacterVRamTile(character);

        set_vram_byte(vramAddress++,vramTile);

        #if defined(SEGA)
        set_vram_byte(vramAddress++,0);
        #endif

        index++;
        columnIndex++;

        // if we've reached the end of the row
        if(columnIndex>=DEVICE_SCREEN_WIDTH){
            
            // reset for the next row
            vramAddress = get_bkg_xy_addr(column,++row);
            
            columnIndex=0;
        }

        if(typewriterDelay>0){

            // Wait some frames
            // This creats a typewriter effect
            for(uint8_t i=0;i<typewriterDelay;i++){

                vsync();
            }

        }
    }
}

void main(void)
{
    SHOW_BKG;

    set_native_tile_data(0,Font_TILE_COUNT,Font_tiles);

    fill_bkg_rect(0,0,DEVICE_SCREEN_WIDTH,DEVICE_SCREEN_HEIGHT,0);

    // By passing 3 as the final argument, the game boy will wait 3 frames between each character
    DrawText("This is a way to draw text on the screen in GBDK. The code will automatically jump to a new line, when it reaches the end of the row.",THREE_FRAMES);
}
