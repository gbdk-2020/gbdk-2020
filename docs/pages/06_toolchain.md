@page docs_toolchain GBDK Toolchain


@anchor toolchain_overview
# Overview
GBDK 2020 uses the SDCC compiler along with some custom tools to build Game Boy ROMs.
- All tools are located under `bin/`
- The typical order of tools called is as follows (when using lcc these steps are usually performed automatically).
  1. Compile and assemble source files (.c, .s, .asm) with @ref sdcc and @ref sdasgb
  2. Optional: perform auto banking with @ref bankpack on the object files
  3. Link the object files into .ihx file with @ref sdldgb
  4. Validate the .ihx file with @ref ihxcheck
  5. Convert the .ihx file to a ROM file (.gb, .gbc) with @ref makebin

To see individual arguments and options for a tool, run that tool from the command line with either no arguments or with `-h`.


# Data Types
For data types and special C keywords, see @ref file_asm_sm83_types_h "asm/sm83/types.h" and @ref file_asm_types_h "asm/types.h".

Also see the SDCC manual (scroll down a little on the linked page): http://sdcc.sourceforge.net/doc/sdccman.pdf#section.1.1

## Using variables in High RAM on the Game Boy
The High RAM (`_HRAM`) address space is at `0xFF80 - 0xFFFE`. It is eight bit addressable using the `ldh` opcodes which allow for more efficient read and write access.

There are two main ways variables can be placed into High RAM
- Using the `SFR` keyword
  - For example: `SFR my_hram_variable;`
  - Variables will be allocated in the `_HRAM` area
  - They will be treated as 8-bit unsigned
  - The compiler will automatically tag the variable as `volatile`
  - The codegen will try to optimize them with `ldh` access
- Using the `dataseg` pragma
  - For example: `#pragma dataseg HRAM` followed by `unsigned char my_dataseg_var;`
  - The codegen **will not** try to optimize them with `ldh` access


@anchor toolchain_changing_important_addresses 
# Changing Important Addresses
It is possible to change some of the important addresses used by the toolchain at link time using the -Wl-g XXX=YYY and =Wl-b XXX=YYY flags (where XXX is the name of the data, and YYY is the new address). 

@ref lcc will include the following linker defaults for @ref sdldgb if they are not defined by the user.

  - `_shadow_OAM`
    - Location of sprite ram (requires 0xA0 bytes).
    - Default `-Wl-g _shadow_OAM=0xC000`

  - `.STACK`
    - Initial stack address
    - Default `-Wl-g .STACK=0xE000`

  - `.refresh_OAM`
    - Address to which the routine for refreshing OAM will be copied (must be in HIRAM). Default
    - Default `-Wl-g .refresh_OAM=0xFF80`

  - `_DATA`
    - Start of RAM section (starts after Shadow OAM)
    - Default `-Wl-b _DATA=0xc0A0`

  - `_CODE`
    - Start of ROM section
    - Default `-Wl-b _CODE=0x0200`


@anchor toolchain_compiling_programs
# Compiling programs

The @ref lcc program is the front end compiler driver for the actual compiler, assembler and linker. It works out what you want to do based on command line options and the extensions of the files you give it, computes the order in which the various programs must be called and then executes them in order. Some examples are:

  - Compile the C source 'source.c', assemble and link it producing the Gameboy image 'image.gb'

        lcc -o image.gb source.c

  - Assemble the file 'source.s' and link it producing the Gameboy image 'image.gb'

        lcc -o image.gb source.s

  - Compile the C program 'source1.c' and assemble it producing the object file 'object1.o' for later linking.

        lcc -c -o object1.o source1.c

  - Assemble the file 'source2.s' producing the object file 'object2.o' for later linking

        lcc -c -o object2.o source2.s

  - Link the two object files 'object1.o' and 'object2.o' and produce the Gameboy image 'image.gb'

        lcc -o image.gb object1.o object2.o

  - Do all sorts of clever stuff by compiling then assembling source1.c, assembling source2.s and then linking them together to produce image.gb. 

        lcc -o image.gb source1.c source2.s

Arguments to the assembler, linker, etc can be passed via lcc using -Wp..., -Wf..., -Wa... and -Wl... to pass options to the pre-processor, compiler, assembler and linker respectively. Some common options are:

  - To generate an assembler listing file.

        -Wa-l

  - To generate a linker map file.

        -Wl-m

  - To bind var to address 'addr' at link time.

        -Wl-gvar=addr

For example, to compile the example in the memory section and to generate a listing and map file you would use the following. Note the leading underscore that C adds to symbol names.

    lcc -Wa-l -Wl-m -Wl-g_snd_stat=0xff26 -o image.gb hardware.c


@subsection Makefiles
## Using Makefiles

Please see the sample projects included with GBDK-2020 for a couple different examples of how to use Makefiles.

You may also want to read a tutorial on Makefiles. For example:  
https://makefiletutorial.com/  
https://www.tutorialspoint.com/makefile/index.htm

@anchor linkerfiles_and_autobanking
## Linker Files and ROM Auto Banking

When @ref bankpack is called through @ref lcc it will now always use linkerfile output (`-lkout=`) for passing files to the linker (all input object files and linkerfiles will get get consolidated to a single linkerfile).

Bankpack:
- `lkin=<filename>` : Adds a input linkerfile (can specify multiple ones)
- `-lkout=<filename>` : Enables linkerfile output and sets name (only one can be specified). ALL loaded object files, both from the command line and any loaded from linkerfiles will have their names written to this single output.

LCC + Bankpack:
- `lcc` passes all input linkerfiles (from `-Wl-f<name>`) to bankpack (`-lkin=`)
- Linkerfile output is always used when lcc calls `bankpack` (`-lkout=`)
- A temporary file name is used for bankpack linkerfile output.
- `lcc` clears out the linker object file and linkerfile lists, then uses the single linkerfile generated by `bankpack`

Also see the `linkerfile` example project.


@anchor build_tools
# Build Tools

@anchor lcc
## lcc
lcc is the compiler driver (front end) for the GBDK/sdcc toolchain.

For detailed settings see @ref lcc-settings

It can be used to invoke all the tools needed for building a rom.
If preferred, the individual tools can be called directly.
- the `-v` flag can be used to show the exact steps lcc executes for a build
- lcc can compile, link and generate a binary in a single pass: `lcc -o somerom.gb somesource.c`
- @anchor lcc_debug
  lcc now has a `-debug` flag that will turn on the following recommended flags for debugging
    - `--debug` for sdcc (lcc equiv: `-Wf-debug`)
    - `-y` enables .cdb output for @ref sdldgb (lcc equiv: `-Wl-y`)
    - `-j` enables .noi output for @ref sdldgb (lcc equiv: `-Wl-j`)


@anchor sdcc
## sdcc
SDCC C Source compiler.

For detailed settings see @ref sdcc-settings

- Arguments can be passed to it through @ref lcc using `-Wf-<argument>` and `-Wp-<argument>` (pre-processor)

@anchor sdasgb
## sdasgb
SDCC Assembler for the Game Boy.

For detailed settings see @ref sdasgb-settings

- Arguments can be passed to it through @ref lcc using `-Wa-<argument>`


@anchor bankpack
## bankpack
Automatic Bank packer.

For detailed settings see @ref bankpack-settings

When enabled, automatically assigns banks for object files where bank has been set to `255`, see @ref rom_autobanking.
Unless an alternative output is specified the given object files are updated with the new bank numbers.
- Can be enabled by using the `-autobank` argument with @ref lcc.
- Must be called after compiling/assembling and before linking.
- Arguments can be passed to it through @ref lcc using `-Wb-<argument>`


@anchor sdldgb
## sdldgb
The SDCC linker for the gameboy.

For detailed settings see @ref sdldgb-settings

Links object files (.o) into a .ihx file which can be processed by @ref makebin
- Arguments can be passed to it through @ref lcc using `-Wl-<argument>`


@anchor ihxcheck
## ihxcheck
IHX file validator.

For detailed settings see @ref ihxcheck-settings

Checks .ihx files produced by @ref sdldgb for correctness.
- It will warn if there are multiple writes to the same ROM address. This may indicate mistakes in the code or ROM bank overflows
- Arguments can be passed to it through @ref lcc using `-Wi-<argument>`


@anchor makebin
## makebin
IHX to ROM converter.

- For detailed settings see @ref makebin-settings
- For makebin `-yt` MBC values see @ref setting_mbc_and_rom_ram_banks

Converts .ihx files produced by @ref sdldgb into ROM files (.gb, .gbc). Also used for setting some ROM header data.
- Arguments can be passed to it through @ref lcc using `-Wm-<argument>`


@anchor gbdk_utilities
# GBDK Utilities


@anchor utility_gbcompress
## GBCompress
Compression utility.

For detailed settings see @ref gbcompress-settings

Compresses (and decompresses) binary file data with the gbcompress algorithm (also used in GBTD/GBMB). Decompression support is available in GBDK:
- gb_decompress(), gb_decompress_bkg_data(),  gb_decompress_win_data(), gb_decompress_sprite_data()
- The `cross-platform/gbdecompress` example demonstrates how to use this compression


The utility can also compress (and decompress) using block style RLE encoding with the `--alg=rle` flag. Decompression support is available in GBDK:
- rle_init(), rle_decompress()
- The `cross-platform/rle_map` example demonstrates how to use this compression


@anchor utility_png2asset
## png2asset
Tool for converting PNGs into GBDK format MetaSprites and Tile Maps.

- Convert single or multiple frames of graphics into metasprite structured data for use with the ...metasprite...() functions.
- When `-map` is used, converts images into Tile Maps and matching Tile Sets
- Supports Game Boy / Color, SGB borders, SMS/GG, NES

For detailed settings see @ref png2asset-settings  
For working with sprite properties (including cgb palettes), see @ref metasprite_and_sprite_properties  
For API support see @ref move_metasprite() and related functions in @ref metasprites.h  

### Working with png2asset
  - The origin (pivot) for the metasprite is not required to be in the upper left-hand corner as with regular hardware sprites. See `-px` and `-py`.

  - The conversion process supports using both SPRITES_8x8 (`-spr8x8`) and SPRITES_8x16 mode (`-spr8x16`). If 8x16 mode is used then the height of the metasprite must be a multiple of 16.

#### Terminology
The following abbreviations are used in this section:
* Original Game Boy and Game Boy Pocket style hardware: `DMG`
* Game Boy Color: `CGB`

#### Conversion Process
png2asset accepts any png as input, although that does not mean any image will be valid. The program will follow the next steps:
  - The image will be subdivided into tiles of 8x8 or 8x16.
  - For each tile a palette will be generated.
  - If there are more than 4 colors in the palette it will throw an error.
  - The palette will be sorted from darkest to lightest. If there is a transparent color that will be the first one (this will create a palette that will also work with `DMG` devices).
  - If there are more than 8 palettes the program will throw an error.

With all this, the program will generate a new indexed image (with palette), where each 4 colors define a palette and all colors within a tile can only have colors from one of these palettes

It is also posible to pass a indexed 8-bit png with the palette properly sorted out, using `-keep_palette_order`
  - Palettes will be extracted from the image palette in groups of 4 colors.
  - Each tile can only have colors from one of these palettes per tile.
  - The maximum number of colors is 32.

For indexed color images, sometimes RGB paint programs mix up indexed colors in tiles if the same color exists in multiple palettes.
  - `-repair_indexed_pal` can be used to fix this problem, though tiles must still follow the rule of using only one palette per tile.
  
Using this image a tileset will be created
  - Duplicated tiles will be removed.
  - Tiles will be matched without mirror, using vertical mirror, horizontal mirror or both (use `-noflip` to turn off matching mirrored tiles).
  - The palette won't be taken into account for matching, only the pixel color order, meaning there will be a match between tiles using different palettes but looking identical on grayscale.


#### Maps
Passing `-map` the png can be converted to a map that can be used in both the background and the window. In this case, png2asset will generate:
  - The palettes
  - The tileset
  - The map
  - The color info
    - By default, an array of palette index for each tile. This is not the way the hardware works but it takes less space and will create maps compatibles with both `DMG` and `CGB` devices.
    - Passing `-use_map_attributes` will create an array of map attributes. It will also add mirroring info for each tile and because of that maps created with this won't be compatible with `DMG`.
      - Use `-noflip` to make background maps which are compatible with `DMG` devices.

#### Meta sprites
By default the png will be converted to metasprites. The image will be subdivided into meta sprites of `-sw` x `-sh`. In this case png2asset will generate:
  - The metasprites, containing an array of:
    - tile index
    - y offset
    - x offset
    - flags, containing the mirror info, the palettes for both DMG and GBC and the sprite priority
  - The metasprites array


#### Super Game Boy Borders (SGB)
Screen border assets for the Super Game Boy can be generated using png2asset.

The following flags should be used to perform the conversion:
  - `<input_border_file.png> -map -bpp 4 -max_palettes 4 -pack_mode sgb -use_map_attributes -c <output_border_data.c>`
  - Where `<input_border_file.png>` is the image of the SGB border (256x224) and `<output_border_data.c>` is the name of the source file to write the assets out to.


See the `sgb_border` example project for more details.


@anchor utility_makecom
## makecom
Converts a binary .rom file to .msxdos com format, including splitting the banks up into separate files.

- For detailed settings see @ref makecom-settings

@anchor utility_makenes
## makenes
Converts a binary .rom file to a .nes file, prepending a 16-byte header.

- For an explanation of the interpretation of header bits see https://www.nesdev.org/wiki/INES

@anchor utility_png2hicolorgb
## png2hicolorgb
An updated version of Glen Cook's Windows GUI "hicolour.exe" 1.2 conversion tool for the Game Boy Color. The starting code base was the 1.2 release.

- For detailed settings see @ref png2hicolorgb-settings

"Hi Color" on the Game Boy Color is a technique for displaying backgrounds with thousands of colors instead being limtied to 32 colors for the entire screen background. It achieves this by changing ~16 colors of the background palette per scanline. The main tradeoffs are that it uses much of the Game Boy's available cpu processing per frame and requires more ROM space. The tile patterns, map, attributes and per-scanline palettes are pre-calculated using the PC based conversion tool.

For the current GBDK example ISR implementation there is a limit of 6 sprites per line before the hi-color timing breaks down and there start to be background artifacts.

Example: `png2hicolorgb myimage.png --csource -o=my_output_filename`
Example with higher quality (slower conversion): `png2hicolorgb myimage.png --csource -o=my_output_filename --type=3 -L=2 -R=2`

```
Historical credits and info:
   Original Concept : Icarus Productions
   Original Code : Jeff Frohwein
   Full Screen Modification : Anon
   Adaptive Code : Glen Cook
   Windows Interface : Glen Cook
   Additional Windows Programming : Rob Jones
   Original Quantiser Code : Benny
   Quantiser Conversion : Glen Cook
```

### Additional Details
For technical details about the conversion process and rendering, see:
https://github.com/bbbbbr/png2hicolorgb


@anchor utility_romusage
## romusage
A utility for estimating usage of Game Boy and SMS/GG ROMs from .noi and .map files, binary ROMs and more.

- For detailed settings see @ref romusage-settings

Example: `romusage myprogram.noi -g`

