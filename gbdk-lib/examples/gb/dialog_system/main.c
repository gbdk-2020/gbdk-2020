#include <gb/gb.h>
#ifdef CGB
    #include <gb/cgb.h>
#endif

#include "Font.h"

// This is the "canvas" limits.
#define WIDTH_LIMIT 18
#define HEIGHT_LIMIT 4

// Initial tile position.
#define INITPOS_X 1
#define INITPOS_Y 1

// Delay
#define DELAY_AMOUNT 4

// Delay function.
#define vblankDelay(d) for (uint8_t i=0; i<d; i++) vsync();

// This is a list with symbols, which will be used to identify its character position on VRAM.
const unsigned char symbols[25] =
{
	'!', '"', '#', '$', '%', '&', 39, '(', ')', '*', '+', ',', '-', '.', '/',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
};

void dialog_print(unsigned char *text, uint8_t size)
{
    // This is an imaginary cursor.
	uint8_t xpos=0, ypos=0;
    // This is the string index.
	uint8_t index=0;
	
	while (index<(size-1))
	{
        // This do a line jump if there's no more space on tile width (can be personalized).
		if (xpos % WIDTH_LIMIT == 0) {xpos=1; ypos++;}
		else xpos++;

        // This is the string index.
		(xpos == 1 && text[index] == ' ')?index++:index;

        // This put a - sign if there's no width space and a word isn't finished.
		if (xpos == WIDTH_LIMIT && text[index] != ' '
			&& text[index+1] != ' ') // This line is optional. Just remember to close the brackets.
			{set_bkg_tile_xy(xpos, ypos, ('-'-0x20)); xpos=1; ypos++;}

        // This variable sets the current character of the string based on its index.
		const unsigned char *current = &text[index];

        // This fills the canvas with an empty tile if there's no more space.
        // Note that it uses the 0x00 tile.
		if (ypos > HEIGHT_LIMIT)
			{waitpad(J_A); fill_bkg_rect(INITPOS_X, INITPOS_Y, WIDTH_LIMIT, HEIGHT_LIMIT, 0x00); xpos=ypos=1;}

        // This print the letters.
		if (*current != ' ') set_bkg_tile_xy(xpos, ypos, (*current-0x47));

        // This print the symbols.
		for (uint8_t i=0; i<(sizeof(symbols)); i++)
			if (*current == symbols[i]) set_bkg_tile_xy(xpos, ypos, (*current-0x20));

        vblankDelay(DELAY_AMOUNT);

		index++;
	}
}

void main(void)
{
    // if system is CGB, then assign a palette.
    if (_cpu == CGB_TYPE) set_default_palette();
    
    set_bkg_data(0x00, sizeof(Font_tiles)>>4, Font_tiles); // Loads the font.
    init_bkg(0x00); // This avoid trash characters.
    SHOW_BKG;
    // Define your dialoge and print.
    const char text1[] = "abcdefghijklmnopqrtsuvwxyz";
    dialog_print(text1, sizeof(text1));

    // Also, if the text array is more large than the "canvas" you sould press A button to continue.
    // Try changing the text1 array content.
}
