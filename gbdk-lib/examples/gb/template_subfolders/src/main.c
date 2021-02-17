#include <gb/gb.h>
#include "../res/dungeon_map.h"
#include "../res/dungeon_tiles.h"

// Sprite and Background tiles from:
//   https://sondanielson.itch.io/gameboy-simple-rpg-tileset
//   "Licence is under CCA so you can use these assets both privately and commercially"
//   "You are free to use them as is or modify them to your liking. All i ask is you credit me if you do use them please :)"


void init_gfx() {
    // Load tiles (background + window) and map
    set_bkg_data(0, 79u, dungeon_tiles);
    set_bkg_tiles(0, 0, 32u, 32u, dungeon_mapPLN0);

	// Turn the background map on (make it visible)
    SHOW_BKG;
}


void main(void)
{
	init_gfx();

    // Loop forever
    while(1) {


		// Game main loop processing goes here


		// Done processing, yield CPU and wait for start of next frame
        wait_vbl_done();
    }
}
