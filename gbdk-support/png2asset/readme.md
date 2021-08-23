# png2asset

A tool that converts png to maps or meta sprites in C for gbdk 2020

## Working with png2asset

pn2asset accepts any png as input. That doesn't mean any image will be valid. The program will follow the next steps:
- The image will be subdivided into tiles of 8x8 or 8x16
- For Each tile a palette fill be generated
- If there are more than 4 colors in the palette it will throw an error
- The palette will be sorted from black to white, and if there is a transparent color that will be the first one (this will create a palette that will also work in DMG)
- If there are more than 8 palettes the program will throw an error

With all this, the program will generate a new indexed image (with palette), where each 4 colors define a palette and all colors within a tile can only have colors from one of these palettes

It is also posible to pass a png8 with the palette properly sorted out, using -keep_palette_order
- Palettes will be extracted from the image palette in groups of 4 colors.
- Each tile can only have colors from one of these palettes
- The maximum number of colors is 32

Using this image a tileset will be created
- Duplicated tiles will be removed
- Tiles will be matched without mirror, using vertical mirror, horizontal mirror or both
- The palette won't be taken into account, only the order, meaning there will be a match between tiles using different palettes but looking identical on grayscale

### Maps

Passing -map the png can be converted to a map that can be used in both the background and the window. In this case, png2asset will generate:
- The palettes
- The tileset
- The map
- The color info
  - By default, an array of palette index for each tile. This is not the way the hardware works but it takes less space and will create maps compatibles with both DMG and GBC
  - Passing -use_map_attributes will create an array of map attributes. It will also add mirror info for each tile and because of that maps created with this won't be compatible with DMG



### Meta sprites

By default the png will be converted to metasprites. The image will be subdivided into meta sprites of -sw x -sh. In this case png2asset will generate:
- The metasprites, containing an array of:
  - tile index
  - y offset
  - x offset
  - flags, containing the mirror info, the palettes for both DMG and GBC and the sprite priority
- The metasprites array


