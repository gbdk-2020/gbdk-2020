#include <gbdk/platform.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <gbdk/console.h>

#include "mbc7_accelerometer.h"
#include "vsync_no_halt.h"


/*                         **** IMPORTANT ****

** DO NOT ** use regular vsync() or wait_vbl_done() for at least 1.2 msec after
 latching accelerometer data with mbc7_accel_read(). Doing that will cause the
MBC7 accelerometer sensor data to have errors.

This is because vsync() puts the CPU into HALT (until the next frame)
which turns off the cartridge PHI clock that the MBC7 accelerometer
sensor relies on.

This example provides an alternative vsync_no_halt() that is safe to use
without timing limitations.
*/


// New and previous values of the joypad input
uint8_t joy  = 0, old_joy;
// User input macros
#define INPUT_UPDATE (old_joy=joy,joy=joypad())
#define INPUT_BUTTON(key) (joy&(key))
#define INPUT_BUTTONPRESS(key) ((joy & ~old_joy) & (key))


// A plus shaped spite for showing the accelerometer movement
#define SPR_TILE_0    0u  // Use tile number 0
#define SPR_ID_CROSS  0u  // Use sprite id number 0

#define SCRN_CENTER_X (((DEVICE_SCREEN_PX_WIDTH - 8u) / 2u) + DEVICE_SPRITE_PX_OFFSET_X)
#define SCRN_CENTER_Y (((DEVICE_SCREEN_PX_HEIGHT - 8u) / 2u) + DEVICE_SPRITE_PX_OFFSET_Y)

const uint8_t sprite_tile[] = {
    0b00111100, 0b00011000,
    0b01111110, 0b00011000,
    0b11111111, 0b00011000,
    0b11111111, 0b11111111,
    0b11111111, 0b11111111,
    0b11111111, 0b00011000,
    0b01111110, 0b00011000,
    0b00111100, 0b00011000,
};


// How much to divide the relative accelerometer data by.
// Dividing by larger amounts filters out small unwanted movements, but reduces sensitivity
#define ACCEL_DIVIDE_AMT  8


void main(void)
{
    bool do_set_origin = false;
    bool show_data     = false;

    HIDE_SPRITES;
    // Assign a sprite with a tile, but hide it by default
    set_sprite_data(SPR_TILE_0, 1u, sprite_tile);
    set_sprite_tile(SPR_ID_CROSS, SPR_TILE_0);

    printf("-B to Center\n");
    printf("-SELECT toggles info");

    mbc7_accel_init();

    while(1) {

        // Check for Gamepad input
        INPUT_UPDATE;
        switch (INPUT_BUTTONPRESS(J_B | J_SELECT)) {
            case J_B: do_set_origin = true; break;
            case J_SELECT: show_data ^= true; break;
        }

        mbc7_accel_update(do_set_origin);

        // Once the origin has been set, clear the flag and show some data about it
        if (do_set_origin) {
            do_set_origin = false;
            // Display origin, center the sprite and turn on sprites
            gotoxy(0,3);
            printf("origin x:%x\norigin y:%x", accel_x_origin, accel_y_origin);
            SHOW_SPRITES;
            move_sprite(SPR_ID_CROSS, SCRN_CENTER_X, SCRN_CENTER_Y);
        }

        // Use the Accelerometer data if the origin has been set
        if (accel_origin_set) {

            // Move the sprite around the screen using the accelerometer data
            scroll_sprite(SPR_ID_CROSS, accel_x_relative / ACCEL_DIVIDE_AMT, accel_y_relative / ACCEL_DIVIDE_AMT);

            if (show_data) {
                gotoxy(0,6);
                printf("%x\n"
                       "%x\n"
                       "%d   \n"
                       "%d   \n",
                       (uint16_t)accel_x, (uint16_t)accel_y,
                       (int16_t)accel_x_relative, (int16_t)accel_y_relative);
            }
        }

        // Use an alternate to regular vsync() / wait_vbl_done() which
        // does not put the CPU into HALT. This is important since HALT
        // interferes with the reading the MBC7 Accelerometer sensor data.
        vsync_no_halt();
    }
}
