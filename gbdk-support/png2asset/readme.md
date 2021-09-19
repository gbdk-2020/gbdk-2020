# png2asset
A tool that converts png to maps or meta sprites in C for gbdk 2020

### Working with png2asset
  - The origin (pivot) for the metasprite is not required to be in the upper left-hand corner as with regular hardware sprites. See `-px` and `-py`.

  - The conversion process supports using both SPRITES_8x8 (`-spr8x8`) and SPRITES_8x16 mode (`-spr8x16`). If 8x16 mode is used then the height of the metasprite must be a multiple of 16.

#### Terminology
The following abbreviations are used in this section:
* Original Game Boy and Game Boy Pocket style hardware: `DMG`
* Game Boy Color: `CGB`

#### Conversion Process
png2asset accepts any png as input, although that does not mean any image will be valid. The program will follow the next steps:
  - The image will be subdivided into tiles of 8x8 or 8x16
  - For each tile a palette will be generated
  - If there are more than 4 colors in the palette it will throw an error
  - The palette will be sorted from darkest to lightest. If there is a transparent color that will be the first one (this will create a palette that will also work with `DMG` devices)
  - If there are more than 8 palettes the program will throw an error

With all this, the program will generate a new indexed image (with palette), where each 4 colors define a palette and all colors within a tile can only have colors from one of these palettes

It is also posible to pass an indexed 8-bit png with the palette properly sorted out, using `-keep_palette_order`
  - Palettes will be extracted from the image palette in groups of 4 colors.
  - Each tile can only have colors from one of these palettes per tile
  - The maximum number of colors is 32

Using this image a tileset will be created
  - Duplicated tiles will be removed
  - Tiles will be matched without mirror, using vertical mirror, horizontal mirror or both (use `-noflip` to turn off matching mirrored tiles)
  - The palette won't be taken into account for matching, only the pixel color order, meaning there will be a match between tiles using different palettes but looking identical on grayscale

#### Maps
Passing `-map` the png can be converted to a map that can be used in both the background and the window. In this case, png2asset will generate:
  - The palettes
  - The tileset
  - The map
  - The color info
    - By default, an array of palette index for each tile. This is not the way the hardware works but it takes less space and will create maps compatibles with both `DMG` and `CGB` devices.
    - Passing `-use_map_attributes` will create an array of map attributes. It will also add mirroring info for each tile and because of that maps created with this won't be compatible with.
      - Use `-noflip` to make background maps which are compatible with `DMG` devices.

#### Meta sprites
By default the png will be converted to metasprites. The image will be subdivided into meta sprites of `-sw` x `-sh`. In this case png2asset will generate:
  - The metasprites, containing an array of:
    - tile index
    - y offset
    - x offset
    - flags, containing the mirror info, the palettes for both DMG and GBC and the sprite priority
  - The metasprites array


