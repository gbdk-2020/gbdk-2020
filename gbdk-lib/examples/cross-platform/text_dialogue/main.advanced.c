#include <gb/gb.h>
#include <stdint.h>
#include <string.h>
#include "Font.h"
#include "DialogueBox.h"

int16_t windowYPosition=0;
uint8_t fontTilesStart=0;

uint8_t joypadCurrent=0,joypadPrevious=0;

uint8_t loadedCharacters[45];
uint8_t loadedCharacterCount=0;

void MoveWindow(void){


    int16_t y = windowYPosition>>3;

    move_win(7,y);
}


uint8_t GetTileForCharacter(char character){
    
    uint8_t vramTile=0;

    // Char's can be interpreted as integers
    // We don't need to map every alpha-numeric character
    // We can use basic math to simplify A-Z and 0-9
    if(character>='a'&&character<='z')vramTile = (character-'a')+1;
    else if(character>='A'&&character<='Z')vramTile = (character-'A')+1;
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

    return vramTile;
}

uint8_t IsAlphaNumeric(char character){

    
    if(character>='a'&&character<='z')return TRUE;
    else if(character>='A'&&character<='Z')return TRUE;
    else if(character>='0'&&character<='9')return TRUE;

    return FALSE;
}

uint8_t BreakLineEarly(uint16_t index, uint8_t columnSize,uint8_t columnWidth, char* text){

    char character = text[index++];

    if(columnSize>=columnWidth)return TRUE;
    if(IsAlphaNumeric(character))return FALSE;

    uint8_t spaceLeftOnLine=columnWidth-columnSize;

    uint8_t next =1;
    
    while((character=text[index++])!='\0'){

        if(!IsAlphaNumeric(character))break;

        next++;
    }

    return next>spaceLeftOnLine;
}   


void WaitForAButton(void){
    while(TRUE){

        joypadPrevious=joypadCurrent;
        joypadCurrent = joypad();
        
        if((joypadCurrent & J_A) && !(joypadPrevious & J_A))break;
    }

}

void ClearDialogueBox(void){
    
    set_win_based_tiles(0,0,DialogueBox_WIDTH>>3,DialogueBox_HEIGHT>>3,DialogueBox_map,1);
}

void ShowDialgoueBox(void){

    ClearDialogueBox();

    int16_t desiredWindowPosition = (DEVICE_SCREEN_HEIGHT<<3)-DialogueBox_HEIGHT;

    while((windowYPosition>>3)>desiredWindowPosition){

        windowYPosition-=10;
        MoveWindow();
        vsync();
    }
}

void HideDialgoueBox(void){

    int16_t desiredWindowPosition = (DEVICE_SCREEN_HEIGHT<<3);

    while((windowYPosition>>3)<desiredWindowPosition){

        windowYPosition+=10;
        MoveWindow();
        vsync();
    }
}

void ResetLoadedCharacters(void){

    loadedCharacterCount=1;

    // Reset everything to 255
    for(uint8_t i=0;i<45;i++)loadedCharacters[i]=255;
}

void vsyncMultiple(uint8_t count){
     // Wait some frames
            // This creats a typewriter effect
            for(uint8_t i=0;i<count;i++){

                vsync();
            }
}

void DrawTextAdvanced(uint8_t column, uint8_t row, uint8_t columnWidth, char* text,uint8_t typewriterDelay){
        
        ShowDialgoueBox();

    ResetLoadedCharacters();

    uint16_t index=0;
    uint8_t columnSize=0;
    uint8_t rowCount=0;

    // Get the address of the first tile in the row
    uint8_t* vramAddress = get_win_xy_addr(column,row);

    char c;

    while((c=text[index])!='\0'){

        uint8_t vramTile = GetTileForCharacter(c);

        if(loadedCharacters[vramTile]==255){

            loadedCharacters[vramTile]=fontTilesStart+loadedCharacterCount;

            set_bkg_data(fontTilesStart+loadedCharacterCount++,1,Font_tiles+vramTile*16);

        }

        // Draw our character at the address
        // THEN, increment the address
        set_vram_byte(vramAddress++,loadedCharacters[vramTile]);

        index++;
        columnSize++;

         // if we've reached the end of the row
        if(BreakLineEarly(index,columnSize,columnWidth,text) || c=='.'){

            rowCount+=2;

            if(c=='.'||c=='?'){
                WaitForAButton();
                ClearDialogueBox();
                rowCount=0;
            }

            else if( rowCount>2){

                uint8_t copyBuffer[36];
                get_win_tiles(1,3,columnWidth,1,copyBuffer);
                fill_win_rect(1,1,columnWidth,3,0);
                set_win_tiles(1,2,columnWidth,1,copyBuffer);

                vsyncMultiple(15);

                fill_win_rect(1,2,columnWidth,2,0);
                set_win_tiles(1,1,columnWidth,1,copyBuffer);
                rowCount=2;

            }
            
            // reset for the next row
            vramAddress = get_win_xy_addr(column,row+rowCount);
            
            columnSize=0;

            // If we just started a new line, skip spaces
            while(text[index]==' '){
                index++;
            }
        }

        if(typewriterDelay>0){
            
            // Play a basic sound effect
            NR10_REG = 0x34;
            NR11_REG = 0x81;
            NR12_REG = 0x41;
            NR13_REG = 0x7F;
            NR14_REG = 0x86;

           vsyncMultiple(3);

        }
    }

    HideDialgoueBox();
    
}



void ClearScreen(void){
    fill_win_rect(0,0,DEVICE_SCREEN_WIDTH,DEVICE_SCREEN_HEIGHT,0);
    fill_bkg_rect(0,0,DEVICE_SCREEN_WIDTH,DEVICE_SCREEN_HEIGHT,0);
}

void main(void)
{
    SHOW_BKG;
    SHOW_WIN;


    NR52_REG = 0x80; // Master sound on
    NR50_REG = 0xFF; // Maximum volume for left/right speakers. 
    NR51_REG = 0xFF; // Turn on sound fully

    fontTilesStart = DialogueBox_TILE_COUNT+1;
    uint8_t emptyTile[16];
    for(uint8_t i=0;i<16;i++)emptyTile[i]=0;

    set_bkg_data(0,1,emptyTile);

    ClearScreen();

    set_bkg_data(1,DialogueBox_TILE_COUNT,DialogueBox_tiles);

    // Completely hide the window
    windowYPosition = (DEVICE_SCREEN_HEIGHT << 3)<<3;
    MoveWindow();
    
    while(TRUE){

        // We'll pass in one long string, but the game will present to the player multiple pages.
        // By passing 3 as the final argument, the game boy will wait 3 frames between each character
        DrawTextAdvanced(1,1,DEVICE_SCREEN_WIDTH-2,"This is an how you draw text on the screen in GBDK. The code will automatically jump to a new line, when it cannot fully draw a word.  When you reach three lines, it will wait until you press A. After that, it will start a new page and continue. The code will also automatically start a new page after periods. For every page, the code will dynamically populate VRAM. Only letters and characters used, will be loaded into VRAM.",3);
        
    }
}
