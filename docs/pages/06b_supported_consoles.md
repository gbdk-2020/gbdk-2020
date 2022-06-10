@page docs_supported_consoles Supported Consoles & Cross Compiling
  
@anchor docs_consoles_supported_list
# Consoles Supported by GBDK
As of version `4.0.5` GBDK includes support for other consoles in addition to the Game Boy.

  - Game Boy and related clones
    - Nintendo Game Boy / Game Boy Color (GB/GBC)
    - Analogue Pocket (AP)
    - Mega Duck / Cougar Boy (DUCK)

  - Sega Consoles
    - Sega Master System (SMS)
    - Sega Game Gear (GG)

  - MSX DOS (MSXDOS)

While the GBDK API has many convenience functions that work the same or similar across different consoles, it's important to keep their different capabilities in mind when writing code intended to run on more than one. Some (but not all) of the differences are screen sizes, color capabilities, memory layouts, processor type (z80 vs gbz80/sm83) and speed.

 
@anchor docs_consoles_compiling
# Cross Compiling for Different Consoles

## lcc
When compiling and building through @ref lcc use the `-m<port>:<plat>` flag to select the desired console via its port and platform combination.


## sdcc
When building directly with the sdcc toolchain, the following must be specified manually
(when using @ref lcc it will populate these automatically based on `-m<port>:<plat>`).

When compiling with @ref sdcc-settings "sdcc":
  - `-m<port>`, `-D__PORT_<port>` and `-D__TARGET_<plat> `

When assembling with @ref sdasgb-settings "sdasgb" (for GB/AP) and @ref sdasz80-settings "sdasz80" (for SMS/GG):
  - Select the appropriate include path: `-I<gbdk-path>lib/small/asxxxx/<plat>`

When linking with @ref sdldgb-settings "sdldgb" (for GB/AP) and @ref sdldz80-settings "sdldz80" (for SMS/GG or MSXDOS):
  - Select the appropriate include paths: `-k <gbdk-path>lib/small/asxxxx/<port>`, `-k <gbdk-path>lib/small/asxxxx/<plat>`
  - Include the appropriate library files `-l <port>.lib`, `-l <plat>.lib`
  - The crt will be under `  <gbdk-path>lib/small/asxxxx/<plat>/crt0.o`

MSXDOS requires an additional build step after makebin to create the final binary:
  - `makecom <image.bin> [<image.noi>] <output.com>`


@anchor console_port_plat_settings
## Console Port and Platform Settings
Note: Starting with GBDK-2020 4.1.0 and SDCC 4.2, the Game Boy and related clones use `sm83` for the port instead of `gbz80`

  - Nintendo Game Boy / Game Boy Color    
    - @ref lcc : `-msm83:gb`
    - port:`sm83`, plat:`gb`
  - Analogue Pocket
    - @ref lcc : `-msm83:ap`
    - port:`sm83`, plat:`ap`
  - Mega Duck / Cougar Boy
    - @ref lcc : `-msm83:duck`
    - port:`sm83`, plat:`duck`

  - Sega Master System
    - @ref lcc : `-mz80:sms`
    - port:`z80`, plat:`sms`
  - Sega Game Gear
    - @ref lcc : `-mz80:gg`
    - port:`z80`, plat:`gg`

  - MSX DOS
    - @ref lcc : `-mz80:msxdos`
    - port:`z80`, plat:`msxdos`


# Cross-Platform Constants
There are several constant \#defines that can be used to help select console specific code during compile time (with `#ifdef`, `#ifndef`) .

## Console Identifiers
  - When `<gb/gb.h>` is included (either directly or through `<gbdk/platform.h>`)
    - When building for Game Boy:
      - `NINTENDO` will be \#defined
      - `GAMEBOY` will be \#defined
    - When building for Analogue Pocket
      - `NINTENDO` will be \#defined
      - `ANALOGUEPOCKET` will be \#defined
    - When building for Mega Duck / Cougar Boy
      - `NINTENDO` will be \#defined
      - `MEGADUCK` will be \#defined


  - When `<sms/sms.h>` is included (either directly or through `<gbdk/platform.h>`)
    - When building for Master System
      - `SEGA` will be \#defined
      - `MASTERSYSTEM` will be \#defined
    - When building for Game Gear
      - `SEGA` will be \#defined
      - `GAMEGEAR` will be \#defined

  - When `<msx/msx.h>` is included (either directly or through `<gbdk/platform.h>`)
    - `MSXDOS` will be \#defined

## Console Hardware Properties
Constants that describe properties of the console hardware are listed below. Their values will change to reflect the current console target that is being built.

  - @ref DEVICE_SCREEN_X_OFFSET, @ref DEVICE_SCREEN_Y_OFFSET
  - @ref DEVICE_SCREEN_WIDTH, @ref DEVICE_SCREEN_HEIGHT
  - @ref DEVICE_SCREEN_BUFFER_WIDTH, @ref DEVICE_SCREEN_BUFFER_HEIGHT
  - @ref DEVICE_SCREEN_MAP_ENTRY_SIZE
  - @ref DEVICE_SPRITE_PX_OFFSET_X, @ref DEVICE_SPRITE_PX_OFFSET_Y
  - @ref DEVICE_SCREEN_PX_WIDTH, @ref DEVICE_SCREEN_PX_HEIGHT


# Using <gbdk/...> headers
Some include files under `<gbdk/..>` are cross platform and others allow the build process to auto-select the correct include file for the current target port and platform (console).

For example, the following can be used
    
    #include <gbdk/platform.h>
    #include <gbdk/metasprites.h>

Instead of

    #include <gb/gb.h>
    #include <gb/metasprites.h>

and

    #include <sms/sms.h>
    #include <sms/metasprites.h>


@anchor docs_consoles_cross_platform_examples
# Cross Platform Example Projects
GBDK includes an number of cross platform example projects. These projects show how to write code that can be compiled and run on multiple different consoles (for example Game Boy and Game Gear) with, in some cases, minimal differences. 

They also show how to build for multiple target consoles with a single build command and `Makefile`. The `Makefile.targets` allows selecting different `port` and `plat` settings when calling the build stages.

## Cross Platform Asset Example
The cross-platform `Logo` example project shows how assets can be managed for multiple different console targets together.

In the example @ref utility_png2asset is used to generate assets in the native format for each console at compile-time from separate source PNG images. The Makefile is set to use the source PNG folder which matches the current console being compiled, and the source code uses @ref set_native_tile_data() to load the assets tiles in native format.


# Porting From Game Boy to Analogue Pocket
The Analogue Pocket operating in `.pocket` mode is (for practical purposes) functionally identical to the Game Boy / Color though it has a couple changes:

Official differences:
   - Altered register flag and address definitions
   - Different boot logo

Observed differences:
  - MBC1 and MBC5 are supported, MBC3 won't save, the HuC3 isn't supported at all (via JoseJX)
  - The Serial Link port does not work
  - The IR port in CGB mode does not work as reliably as the Game Boy Color

 In order for software to be easily ported to the Analogue Pocket, or to run on both, use the following practices.

## Registers and Flags
Use API defined registers and register flags instead of hardwired ones.
   - LCDC register: @ref LCDC_REG or @ref rLCDC
   - STAT register: @ref STAT_REG or @ref rSTAT
   - LCDC flags: -> LCDCF_... (example: @ref LCDCF_ON)
   - STAT flags: -> STATF_... (example: @ref STATF_LYC)

## Boot logo
As long as the target console is @ref docs_consoles_compiling "set during build time" then the correct boot logo will be automatically selected.


# Porting From Game Boy to Mega Duck / Cougar Boy
The Mega Duck is fairly similar to the classic Game Boy. It has a couple altered register flag and address definitions, no boot logo and a different startup/entry-point address. In order for software to be easily ported to the Mega Duck, or to run on both, use the following practices.

## Registers and Flags
Use API defined registers and register flags instead of hardwired ones
   - LCDC register: @ref LCDC_REG or @ref rLCDC
   - STAT register: @ref STAT_REG or @ref rSTAT
   - LCDC flags: -> LCDCF_... (example: @ref LCDCF_ON)
   - STAT flags: -> STATF_... (example: @ref STATF_LYC)


# Porting From Game Boy to SMS/GG

## Tile Data and Tile Map loading

### Tile and Map Data in 2bpp Game Boy Format
- @ref set_bkg_data() and @ref set_sprite_data() will load 2bpp tile data in "game boy" format on both GB and SMS/GG.
- On the SMS/GG @ref set_2bpp_palette() sets 4 colors that will be used when loading 2bpp assets with set_bkg_data(). This allows GB assets to be easily colorized without changing the asset format. There is some performance penalty for using the conversion.
- @ref set_bkg_tiles() loads 1-byte-per-tile tilemaps both for the GB and SMS/GG.

### Tile and Map Data in Native Format
Use the following api calls when assets are avaialble in the native format for each platform.

@ref set_native_tile_data()
  - GB/AP: loads 2bpp tiles data
  - SMS/GG: loads 4bpp tile data

@ref set_tile_map()
  - GB/AP: loads 1-byte-per-tile tilemaps
  - SMS/GG: loads 2-byte-per-tile tilemaps

There are also bit-depth specific API calls:
- 1bpp: @ref set_1bpp_colors, @ref set_bkg_1bpp_data, @ref set_sprite_1bpp_data
- 2bpp: @ref set_2bpp_palette, @ref set_bkg_2bpp_data, @ref set_sprite_2bpp_data, @ref set_tile_2bpp_data (sms/gg only)
- 2bpp: @ref set_bkg_4bpp_data (sms/gg only), @ref set_sprite_4bpp_data (sms/gg only)

### Emulated Game Boy Color map attributes on the SMS/Game Gear
On the Game Boy Color, @ref VBK_REG is used to select between the regular background tile map and the background attribute tile map (for setting tile color palette and other properties).

This behavior is emulated for the SMS/GG when using @ref set_bkg_tiles() and @ref VBK_REG. It allows writing a 1-byte tile map separately from a 1-byte attributes map.

@note Tile map attributes on SMS/Game Gear use different control bits than the Game Boy Color, so a modified attribute map must be used.

# Hardware Comparison
The specs below reflect the typical configuration of hardware when used with GBDK and is not meant as a complete list of their capabilities.

GB/AP
- Sprites:
  - 256 tiles (upper 128 are shared with background) (amount is doubled in CGB mode)
  - tile flipping/mirroring: yes
  - 40 total, max 10 per line
  - 2 x 4 color palette (color 0 transparent). 8 x 4 color palettes in CGB mode
- Background: 256 tiles (typical setup: upper 128 are shared with sprites) (amount is doubled in CGB mode)
  - tile flipping/mirroring: no (yes in CGB mode)
  - 1 x 4 color palette. 8 x 4 color palettes in CGB mode
- Window "layer": available
- Screen: 160 x 144
- Hardware Map: 256 x 256


SMS/GG
- Sprites:
  - 256 tiles (a bit less in the default setup)
  - tile flipping/mirroring: no
  - 64 total, max 8 per line
  - 1 x 16 color palette (color 0 transparent)
- Background: 512 tiles (upper 256 are shared with sprites)
  - tile flipping/mirroring: yes
  - 2 x 16 color palettes
- Window "layer": not available
- SMS
  - Screen: 256 x 192
  - Hardware Map: 256 x 224
- GG
  - Screen: 160 x 144
  - Hardware Map: 256 x 224


@anchor docs_consoles_safe_display_controller_access
## Safe VRAM / Display Controller Access

GB/AP
- VRAM / Display Controller (PPU)
  - VRAM and some other display data / registers should only be written to when the @ref STATF_B_BUSY bit of @ref STAT_REG is off. Most GBDK API calls manage this automatically.

SMS/GG
- Display Controller (VDP)
  - Writing to the VDP should not be interrupted while an operation is already in progress (since that will interfere with the internal data pointer causing data to be written to the wrong location).
  - Recommended approach: Avoid writing to the VDP (tiles, map, scrolling, colors, etc) during an interrupt routine (ISR).
  - Alternative (requires careful implementation): Make sure writes to the VDP during an ISR are only performed when the @ref _shadow_OAM_OFF flag indicates it is safe to do so.


