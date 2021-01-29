#include <gb/gb.h>
#include <gb/sgb.h>
#include <gb/metasprites.h>

#include "sprite.h"

const unsigned char pattern[] = {0x80,0x80,0x40,0x40,0x20,0x20,0x10,0x10,0x08,0x08,0x04,0x04,0x02,0x02,0x01,0x01};

joypads_t joypads;

#define ACC_X 1
#define ACC_Y 2

// sprite coords
UINT16 PosX, PosY;
INT16 SpdX, SpdY;
UINT8 PosF;
UINT8 hide, jitter;
UINT8 idx, rot;

// main funxction
void main(void) {
    // init palettes
    BGP_REG = OBP0_REG = 0xE4;
    OBP1_REG = 0xE0;

    fill_bkg_rect(0, 0, 20, 18, 0);
    set_bkg_data(0, 1, pattern);

    // load tile data into VRAM
    set_sprite_data(0, sizeof(sprite_data) >> 4, sprite_data);

    // set sprite tile
    set_sprite_tile(0, 0);

    // show bkg and sprites
    SHOW_BKG; SPRITES_8x16; SHOW_SPRITES;

    // init 2 joypads
    joypad_init(1, &joypads);
 
    PosX = PosY = 64 << 4;
    SpdX = SpdY = 0;

    hide = 0; jitter = 0; idx = 0; rot = 0;

    while(1) {        
        // poll joypads
        joypad_ex(&joypads);
        
        PosF = 0;
        // game object
        if (joypads.joy0 & J_UP) {
            SpdY -= 2;
            if (SpdY < -64) SpdY = -64;
            PosF |= ACC_Y; 
        } else if (joypads.joy0 & J_DOWN) {
            SpdY += 2;
            if (SpdY > 64) SpdY = 64;
            PosF |= ACC_Y;
        }
        if (joypads.joy0 & J_LEFT) {
            SpdX -= 2;
            if (SpdX < -64) SpdX = -64;
            PosF |= ACC_X; 
        } else if (joypads.joy0 & J_RIGHT) {
            SpdX += 2;
            if (SpdX > 64) SpdX = 64;
            PosF |= ACC_X;
        }
        if ((joypads.joy0 & J_A) && (!jitter)) {
            hide = (!hide);
            jitter = 10;
        }
        if ((joypads.joy0 & J_B) && (!jitter) && (!hide)) {
            idx++; if (idx >= (sizeof(sprite_metasprites) >> 1)) idx = 0;
            jitter = 10;
        }
        if ((joypads.joy0 & J_SELECT) && (!jitter) && (!hide)) {
            rot++; rot &= 3;
            jitter = 10;
        }

        // anti-jitter
        if (jitter) jitter--;

        PosX += SpdX, PosY += SpdY; 

        UBYTE hiwater = 0;

        // hide or move sprite
        if (hide)
            hide_metasprite(sprite_metasprites[idx], 0);
        else
            switch (rot) {
                case 0: hiwater = move_metasprite(sprite_metasprites[idx], 0, 0, PosX >> 4, PosY >> 4); break;
                case 1: hiwater = move_metasprite_hflip(sprite_metasprites[idx], 0, 0, PosX >> 4, PosY >> 4); break;
                case 2: hiwater = move_metasprite_hvflip(sprite_metasprites[idx], 0, 0, PosX >> 4, PosY >> 4); break;
                case 3: hiwater = move_metasprite_vflip(sprite_metasprites[idx], 0, 0, PosX >> 4, PosY >> 4); break;
            };

        // hide rest of the hardware sprites, because amount of sprites differ between animation frames
        for (UBYTE i = hiwater; i < 40; i++) shadow_OAM[i].y = 0;


        if (!(PosF & ACC_Y)) {
            if (SpdY >= 0) {
                if (SpdY) SpdY--; 
            } else SpdY ++;
        }
        if (!(PosF & ACC_X)) {
            if (SpdX >= 0) {
                if (SpdX) SpdX--; 
            } else SpdX ++;
        }

        // wait for VBlank to slow down everything
        wait_vbl_done();
    }
}