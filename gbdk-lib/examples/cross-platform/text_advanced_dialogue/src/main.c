#include <gbdk/platform.h>
#include <stdint.h>
#include <string.h>
#include "Font.h"
#include "DialogueBox.h"

#define DIALOG_BOX_HEIGHT 5

#if defined(SEGA)
    #define BYTES_PER_TILE  2
#else
    #define BYTES_PER_TILE  1
#endif

#if defined(SEGA) || defined(NINTENDO_NES)

    #define get_winbkg_xy_addr get_bkg_xy_addr
    #define set_winbkg_based_tiles set_bkg_based_tiles
    #define set_text_tiles set_bkg_tiles
    #define fill_winbkg_rect fill_bkg_rect
    #define set_winbkg_tile_xy set_bkg_tile_xy
    #define DIALOGUE_BOX_Y (DEVICE_SCREEN_HEIGHT-DIALOG_BOX_HEIGHT)
#else
    #define get_winbkg_xy_addr get_win_xy_addr
    #define set_winbkg_based_tiles set_win_based_tiles
    #define fill_winbkg_rect fill_win_rect
    #define set_winbkg_tile_xy set_win_tile_xy
    #define set_text_tiles set_win_tiles
    #define DIALOGUE_BOX_Y 0
#endif

#define TILE_SIZE_BYTES  (BYTES_PER_TILE*16)
#define INNER_DIALOGUE_BOX_WIDTH (DEVICE_SCREEN_WIDTH-2)

int16_t windowYPosition=0;
uint8_t fontTilesStart=0;

uint8_t joypadCurrent=0,joypadPrevious=0;

uint8_t loadedCharacters[Font_TILE_COUNT];
uint8_t loadedCharacterCount=0;

void MoveWindow(void){
    
    #if !defined(SEGA) &&  !defined(NINTENDO_NES)

    int16_t y = windowYPosition>>3;

    move_win(7,y);

    #endif
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

    // Return true for a-z,A-Z, and 0-9
    if(character>='a'&&character<='z')return TRUE;
    else if(character>='A'&&character<='Z')return TRUE;
    else if(character>='0'&&character<='9')return TRUE;

    return FALSE;
}

uint8_t BreakLineEarly(uint16_t index, uint8_t columnSize, char* text){

    char character = text[index++];

    // We can break, if we are at the end of our row
    if(columnSize>=INNER_DIALOGUE_BOX_WIDTH)return TRUE;

    // We DO NOT  break on alpha-numeric characters
    if(IsAlphaNumeric(character))return FALSE;

    // How many characters are left on the current line
    uint8_t spaceLeftOnLine=INNER_DIALOGUE_BOX_WIDTH-columnSize;
    uint8_t nextColumnSize=columnSize+1;

    // Loop ahead until we reach the end of the string
    while((character=text[index++])!='\0'){

        // Stop when we reach a non alphanumeric character
        if(!IsAlphaNumeric(character))break;

        // Increase how many characters we've skipped
        nextColumnSize++;
    }

    // Return TRUE if the distance to the next non alphanumeric character, is larger than we have left on the line
    return nextColumnSize>INNER_DIALOGUE_BOX_WIDTH;
}   


void WaitForAButton(void){
    while(TRUE){

        joypadPrevious=joypadCurrent;
        joypadCurrent = joypad();
        
        if((joypadCurrent & J_A) && !(joypadPrevious & J_A))break;
    }

}

void ClearDialogueBox(void){

    // Fill the middle of the dialog box with blank space
    fill_winbkg_rect(1,DIALOGUE_BOX_Y+1,DEVICE_SCREEN_WIDTH-2,DIALOG_BOX_HEIGHT-2,0);
    
    // Top  and Bottom sides
    fill_winbkg_rect(1,DIALOGUE_BOX_Y,DEVICE_SCREEN_WIDTH-2,1,2);
    set_winbkg_tile_xy(DEVICE_SCREEN_WIDTH-1,DIALOGUE_BOX_Y+DIALOG_BOX_HEIGHT-1,6);
    fill_winbkg_rect(1,DIALOGUE_BOX_Y+DIALOG_BOX_HEIGHT-1,DEVICE_SCREEN_WIDTH-2,1,2);

    // Left and right sides
    fill_winbkg_rect(0,DIALOGUE_BOX_Y+1,1,DIALOG_BOX_HEIGHT-2,4);
    fill_winbkg_rect(DEVICE_SCREEN_WIDTH-1,DIALOGUE_BOX_Y+1,1,DIALOG_BOX_HEIGHT-2,4);

    // Top Left and Top Right corners
    set_winbkg_tile_xy(0,DIALOGUE_BOX_Y,1);
    set_winbkg_tile_xy(DEVICE_SCREEN_WIDTH-1,DIALOGUE_BOX_Y,3);

    // Bottom Left and Bottom Right corners
    set_winbkg_tile_xy(0,DIALOGUE_BOX_Y+DIALOG_BOX_HEIGHT-1,5);
    set_winbkg_tile_xy(DEVICE_SCREEN_WIDTH-1,DIALOGUE_BOX_Y+DIALOG_BOX_HEIGHT-1,6);
}

void ShowDialgoueBox(void){

    ClearDialogueBox();

    int16_t desiredWindowPosition = (DEVICE_SCREEN_HEIGHT<<3)-(DIALOG_BOX_HEIGHT*8);

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

void DrawTextAdvanced(char* text){

    uint8_t column=1;
    uint8_t row=DIALOGUE_BOX_Y+1;
        
    ShowDialgoueBox();

    ResetLoadedCharacters();

    uint16_t index=0;
    uint8_t columnSize=0;
    uint8_t rowCount=0;

    uint8_t copyBuffer[INNER_DIALOGUE_BOX_WIDTH];
    uint8_t copyBufferCount=0;

    // Clear the copy buffer
    for(uint8_t k=0;k<INNER_DIALOGUE_BOX_WIDTH;k++)copyBuffer[k]=0;


    // Get the address of the first tile in the row
    uint8_t* vramAddress = get_winbkg_xy_addr(column,row);

    char c;

    while((c=text[index])!='\0'){

        uint8_t vramTile = GetTileForCharacter(c);

        // If we haven't loaded this character into VRAM
        if(loadedCharacters[vramTile]==255){

            // Save where we place this character in VRAM
            loadedCharacters[vramTile]=fontTilesStart+loadedCharacterCount++;

            // Place this character in VRAM
            set_native_tile_data(loadedCharacters[vramTile],1,Font_tiles+vramTile*TILE_SIZE_BYTES);

        }

        // Draw our character at the address
        // THEN, increment the address
        set_vram_byte(vramAddress++,loadedCharacters[vramTile]);

        #if defined(SEGA)
        set_vram_byte(vramAddress++,0);
        #endif

        if(rowCount>=2){

            // Copy everything row 2 to a buffer
            // So we can easily slide that row upwards, without having to access VRAM
            copyBuffer[copyBufferCount++] = loadedCharacters[vramTile];
        }

        index++;
        columnSize++;

         // if we've reached the end of the row
        if(BreakLineEarly(index,columnSize,text) || c=='.'){

            rowCount+=2;

            // If we just drew a period or question mark,
            // wait for the a button  and afterwards clear the dialogue box.
            if(c=='.'||c=='?'){
                WaitForAButton();
                ClearDialogueBox();
                rowCount=0;

                // Reset/Clear the copy buffer
                copyBufferCount=0;
                for(uint8_t k=0;k<INNER_DIALOGUE_BOX_WIDTH;k++)copyBuffer[k]=0;
            }

            // if we've drawn our 2 rows
            else if( rowCount>2){

                // Clear the inner dialogue box
                fill_winbkg_rect(1,DIALOGUE_BOX_Y+1,INNER_DIALOGUE_BOX_WIDTH,3,0);

                // Draw the line of text one tile up
                set_text_tiles(1,DIALOGUE_BOX_Y+2,INNER_DIALOGUE_BOX_WIDTH,1,copyBuffer);

                // Wait a little bit
                vsyncMultiple(15);

                // Clear the previous line of text
                fill_winbkg_rect(1,DIALOGUE_BOX_Y+2,INNER_DIALOGUE_BOX_WIDTH,1,0);

                // Draw on the first row of our dialogue box
                set_text_tiles(1,DIALOGUE_BOX_Y+1,INNER_DIALOGUE_BOX_WIDTH,1,copyBuffer);

                copyBufferCount=0;

                // Clear the copy buffer
                for(uint8_t k=0;k<INNER_DIALOGUE_BOX_WIDTH;k++)copyBuffer[k]=0;

                rowCount=2;

            }
            
            // reset for the next row
            vramAddress = get_winbkg_xy_addr(column,row+rowCount);
            
            columnSize=0;

            // If we just started a new line, skip spaces
            while(text[index]==' '){
                index++;
            }
        }

        #if !defined(SEGA) && !defined(NINTENDO_NES)

            // Play a basic sound effect
            NR10_REG = 0x34;
            NR11_REG = 0x81;
            NR12_REG = 0x41;
            NR13_REG = 0x7F;
            NR14_REG = 0x86;

        #endif

        vsyncMultiple(3);
    }

    HideDialgoueBox();
    
}



void ClearScreen(void){
    #if !defined(SEGA) && !defined(NINTENDO_NES)
    // Game Gear doesn't have a window.
    fill_win_rect(0,0,DEVICE_SCREEN_WIDTH,DEVICE_SCREEN_HEIGHT,0);
    #endif
    fill_bkg_rect(0,0,DEVICE_SCREEN_WIDTH,DEVICE_SCREEN_HEIGHT,0);
}

void main(void)
{
    DISPLAY_ON;
    SHOW_BKG;

    #if !defined(SEGA) && !defined(NINTENDO_NES)
    SHOW_WIN;
    #endif

    fontTilesStart = DialogueBox_TILE_COUNT+1;
    uint8_t emptyTile[TILE_SIZE_BYTES];
    for(uint8_t i=0;i<TILE_SIZE_BYTES;i++)emptyTile[i]=0;

    set_native_tile_data(0,1,emptyTile);

    ClearScreen();

    set_native_tile_data(1,DialogueBox_TILE_COUNT,DialogueBox_tiles);

    
    #if !defined(SEGA) && !defined(NINTENDO_NES)

        NR52_REG = 0x80; // Master sound on
        NR50_REG = 0xFF; // Maximum volume for left/right speakers. 
        NR51_REG = 0xFF; // Turn on sound fully

        // Completely hide the window
        windowYPosition = (DEVICE_SCREEN_HEIGHT << 3)<<3;
        MoveWindow();

    #endif
    
    while(TRUE){

        // We'll pass in one long string, but the game will present to the player multiple pages.
        // By passing 3 as the final argument, the game boy will wait 3 frames between each character
        DrawTextAdvanced("This is an how you draw text on the screen in GBDK. The code will automatically jump to a new line, when it cannot fully draw a word.  When you reach three lines, it will wait until you press A. After that, it will start a new page and continue. The code will also automatically start a new page after periods. For every page, the code will dynamically populate VRAM. Only letters and characters used, will be loaded into VRAM.");
        
    }
}
