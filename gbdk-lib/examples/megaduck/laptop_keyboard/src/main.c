#include <gbdk/platform.h>
#include <gbdk/console.h>
#include <gb/isr.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <duck/laptop_io.h>
#include <duck/model.h>

#include "megaduck_keyboard.h"

uint8_t cursor_x, cursor_y;
bool keyboard_read_ok;


duck_keyboard_data_t keydata;

// A dashed underscore cursor sprite
const uint8_t cursor_tile[16] = {
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b01010101, 0b01010101,
    0b10101010, 0b10101010,
};

#define SPR_CURSOR 0u

static void update_edit_cursor(int8_t delta_x, int8_t delta_y);
static void use_key_data(char key);
static void gfx_init(void);


// Moves sprite based cursor and changes console.h current text position
static void update_edit_cursor(int8_t delta_x, int8_t delta_y) {
    gotoxy(posx() + delta_x, posy() + delta_y);
    move_sprite(SPR_CURSOR, (posx() * 8) + DEVICE_SPRITE_PX_OFFSET_X, (posy() * 8) + DEVICE_SPRITE_PX_OFFSET_Y);
}


// Example of typing and moving a cursor around the screen with arrow keys
static void use_key_data(char key) {

    switch (key) {

        case NO_KEY: break;

        case KEY_ARROW_UP:    update_edit_cursor( 0, -1); break;
        case KEY_ARROW_DOWN:  update_edit_cursor( 0,  1); break;
        case KEY_ARROW_LEFT:  update_edit_cursor(-1,  0); break;
        case KEY_ARROW_RIGHT: update_edit_cursor( 1,  0); break;

        // Clears the screen
        case KEY_HELP:
            cls();
            update_edit_cursor(1, 0);
            break;

        case KEY_ENTER:
            gotoxy(0, posy() + 1);
            update_edit_cursor(1, 0);
            break;

        case KEY_BACKSPACE:
            gotoxy(posx() - 1, posy());
            putchar(' ');
            update_edit_cursor(-1, 0);
            break;

        // All other keys
        default:
            putchar(key);
            update_edit_cursor(0, 0);
            break;
    }
}


static void gfx_init(void) {

    // Set up sprite cursor
    set_sprite_data(0,1,cursor_tile);
    set_sprite_tile(SPR_CURSOR,0);
    hide_sprite(SPR_CURSOR);

    SPRITES_8x8;
    SHOW_SPRITES;
    SHOW_BKG;
}


void main(void) {

    uint8_t megaduck_model = duck_check_model(); // This must be called before any vram tiles are loaded or cleared
    bool megaduck_laptop_init_ok = duck_io_laptop_init();

    gfx_init();
    printf("Initializing..\n");

    if (!megaduck_laptop_init_ok) {
        // If laptop hardware is not present then there isn't anything
        // for this program to do, so just idle in a loop
        printf("Laptop not detected\n"
               "or Failed to Initialize");
        while(1) {
            vsync();
        }
    }


    // Otherwise laptop init succeeded
    printf("Laptop Detected! >%hu<\n", megaduck_model);

    // This may not work in emulators which don't simulate
    // the preloaded Laptop System ROM font tiles in VRAM
    if (megaduck_model == MEGADUCK_LAPTOP_SPANISH)
		printf("Spanish model\n");
    else if (megaduck_model == MEGADUCK_LAPTOP_GERMAN)
        printf("German model\n");

    // Put the editing cursor in a starting location
    update_edit_cursor(1,1);


	while(1) {
	    vsync();

        // Laptop serial command intervals below 20 msec may cause laptop hardware lockup
        // Poll for keyboard every other frame (~32 msec)
	    if (sys_time & 0x01u) {

	        if (duck_io_poll_keyboard(&keydata)) {
	            // Convert from key scancodes to ascii and apply key repeat
	            char current_key = duck_io_process_key_data(&keydata, megaduck_model);

	            use_key_data(current_key);
	        }
	    }
	}
}
