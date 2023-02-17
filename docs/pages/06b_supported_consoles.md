@page docs_supported_consoles Supported Consoles & Cross Compiling
  
@anchor docs_consoles_supported_list
# Consoles Supported by GBDK
As of version `4.1.0` GBDK includes support for other consoles in addition to the Game Boy.

  - Game Boy and related clones
    - Nintendo Game Boy / Game Boy Color (GB/GBC)
    - Analogue Pocket (AP)
    - Mega Duck / Cougar Boy (DUCK)

  - Sega Consoles
    - Sega Master System (SMS)
    - Sega Game Gear (GG)

  - MSX DOS (MSXDOS) (partial support)
  - NES (NES) (partial support)

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
  - Select the appropriate include path: `-I<gbdk-path>lib/<plat>`

When linking with @ref sdldgb-settings "sdldgb" (for GB/AP) and @ref sdldz80-settings "sdldz80" (for SMS/GG or MSXDOS):
  - Select the appropriate include paths: `-k <gbdk-path>lib/<port>`, `-k <gbdk-path>lib/<plat>`
  - Include the appropriate library files `-l <port>.lib`, `-l <plat>.lib`
  - The crt will be under `  <gbdk-path>lib/<plat>/crt0.o`

MSXDOS requires an additional build step with @ref utility_makecom "makecom" after @ref makebin to create the final binary:
  - `makecom <image.bin> [<image.noi>] <output.com>`

The NES port has `--no-peep` specified (in @ref lcc) due to a peephole related codegen bug in SDCC that has not yet been merged.
  - If you wish to build without that flag then SDCC can be called directly instead of through lcc.
  - Alternately, custom peephole rules from a file can be passed in using `-Wf--peep-file` (lcc) or `--peep-file` (sdcc).

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

  - NES
    - @ref lcc : `-mmos6502:nes`
    - port:`mos6502`, plat:`nes`


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


# Hardware Summaries
The specs below reflect the typical configuration of hardware when used with GBDK and is not meant as a complete list of their capabilities.

GB/AP/DUCK
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



@anchor using_cgb_features
# Using Game Boy Color (CGB) Features

## Differences Versus the Regular Game Boy (DMG/GBP/SGB)
These are some of the main hardware differences between the Regular Game Boy and the Game Boy Color.

  - CPU: Optional 2x Speed mode
  - Serial Link: Additional Speeds 2KB/s, 32KB/s, 64KB/s
  - IR Port
  - Sprites:
    - 2 x 256 banks of tile patterns (2x as many) (typically upper 256 shared with background)
    - 8 x 4 color palettes in CGB mode (BGR-555 per color, 32768 color choices)
  - Background:
    - 2 x 256 banks of tile patterns (2x as many) (typically upper 256 shared with sprites)
    - Second map bank for tile attributes (color, flipping/mirroring, priority, bank)
    - 8 x 4 color palettes in CGB mode (BGR-555 per color, 32768 color choices))
    - BG and Window master priority
  - WRAM: 8 x 4K WRAM banks in the 0xD000 - 0xDFFF region
  - LCD VRAM DMA

## CGB features in GBDK
These are some of the main GBDK API features for the CGB. 
Many of the items listed below link to additional information.

  - Tile and Pattern data:
    - Select VRAM Banks: @ref VBK_REG (used with `set_bkg/win/sprite_*()`)
    - set_bkg_attributes(), set_bkg_submap_attributes()
  - Color:
    - set_bkg_palette(), set_bkg_palette_entry() 
    - set_sprite_palette(), set_sprite_palette_entry() 
    - set_default_palette()
    - RGB(), RGB8(), RGBHTML()
  - Detect and change CPU speed: if (@ref _cpu == @ref CGB_TYPE), cpu_fast()
  - More details in @ref cgb.h (`#include <gb/cgb.h>`)

## CGB Examples
Several examples in GBDK show how to use CGB features, including the following:
  - `gb/colorbar`, `gb/dscan`, `cross-platform/large_map`, `cross-platform/logo`, `cross-platform/metasprites`


# Porting Between Supported Consoles

## From Game Boy to Analogue Pocket
The Analogue Pocket operating in `.pocket` mode is (for practical purposes) functionally identical to the Game Boy / Color though it has a couple changes listed below. These are handled automatically in GBDK as long as the practices outlined below are followed.

### Official differences:
   - Altered register flag and address definitions
     - @ref STAT_REG "STAT" & @ref LCDC_REG "LCDC": Order of register bits is reversed
       - Example: @ref LCDCF_B_ON "LCD on/off" is LCDC.0 instead of .7
       - Example: @ref STATF_B_LYC "LYC Interrupt enable" is STAT.1 instead of .6
     - @ref LCDC_REG "LCDC" address is `0xFF4E` instead of `0xFF40`
   - Different logo data in the header at address `0x0104`:
     - `0x01, 0x10, 0xCE, 0xEF, 0x00, 0x00, 0x44, 0xAA, 0x00, 0x74, 0x00, 0x18, 0x11, 0x95, 0x00, 0x34, 0x00, 0x1A, 0x00, 0xD5, 0x00, 0x22, 0x00, 0x69, 0x6F, 0xF6, 0xF7, 0x73, 0x09, 0x90, 0xE1, 0x10, 0x44, 0x40, 0x9A, 0x90, 0xD5, 0xD0, 0x44, 0x30, 0xA9, 0x21, 0x5D, 0x48, 0x22, 0xE0, 0xF8, 0x60`
                

### Observed differences:
  - MBC1 and MBC5 are supported, MBC3 won't save and RTC doesn't progress when game is not running, the HuC3 isn't supported at all (via JoseJX and sg).
  - The Serial Link port does not work
  - The IR port in CGB mode does not work as reliably as the Game Boy Color

 In order for software to be easily ported to the Analogue Pocket, or to run on both, use the following practices.

### Registers and Flags
Use API defined registers and register flags instead of hardwired ones.
   - LCDC register: @ref LCDC_REG or @ref rLCDC
   - STAT register: @ref STAT_REG or @ref rSTAT
   - LCDC flags: -> LCDCF_... (example: @ref LCDCF_ON)
   - STAT flags: -> STATF_... (example: @ref STATF_LYC)

### Boot logo
As long as the target console is @ref docs_consoles_compiling "set during build time" then the correct boot logo will be automatically selected.


## From Game Boy to SMS/GG

### Tile Data and Tile Map loading

#### Tile and Map Data in 2bpp Game Boy Format
- @ref set_bkg_data() and @ref set_sprite_data() will load 2bpp tile data in "game boy" format on both GB and SMS/GG.
- On the SMS/GG @ref set_2bpp_palette() sets 4 colors that will be used when loading 2bpp assets with set_bkg_data(). This allows GB assets to be easily colorized without changing the asset format. There is some performance penalty for using the conversion.
- @ref set_bkg_tiles() loads 1-byte-per-tile tilemaps both for the GB and SMS/GG.

#### Tile and Map Data in Native Format
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

#### Emulated Game Boy Color map attributes on the SMS/Game Gear
On the Game Boy Color, @ref VBK_REG is used to select between the regular background tile map and the background attribute tile map (for setting tile color palette and other properties).

This behavior is emulated for the SMS/GG when using @ref set_bkg_tiles() and @ref VBK_REG. It allows writing a 1-byte tile map separately from a 1-byte attributes map.

@note Tile map attributes on SMS/Game Gear use different control bits than the Game Boy Color, so a modified attribute map must be used.



## From Game Boy to Mega Duck / Cougar Boy
The Mega Duck is (for practical purposes) functionally identical to the Original Game Boy though it has a couple changes listed below.

### Summary of Hardware changes:
  - Cartridge Boot Logo: not present on Mega Duck
  - Cartridge Header data: not present on Mega Duck
  - Program Entry Point: `0x0000` (on Game Boy: `0x0100` )
  - Display registers and flag definitions: Some changed
  - Audio registers and flag definitions: Some changed
  - MBC ROM bank switching register address: `0x0001` (many Game Boy MBCs use `0x2000 - 0x3FFF`)

### Best Practices
In order for software to be easily ported to the Mega Duck, or to run on both, use these practices. That will allow GBDK to automatically handle _most_ of the differences (for the exceptions see @ref megaduck_sound_register_value_changes "Sound Register Value Changes").
  - @ref docs_consoles_compiling "Set the target console during build time"
  - Use the GBDK definitions and macros for:
    - Video Registers and Flags (examples: @ref LCDC_REG, @ref LCDCF_BG8000, etc)
    - Audio Registers and Flags (examples: @ref NR12_REG, @ref NR43_REG, etc)
    - Use the default @ref SWITCH_ROM macro for changing ROM banks

@anchor megaduck_sound_register_value_changes
### Sound Register Value Changes
There are two hardware changes which will not be handled automatically when following the practices mentioned above.

These changes may be required when using existing Sound Effects and Music Drivers written for the Game Boy.

1. Registers @ref NR12_REG, @ref NR22_REG, @ref NR42_REG, and @ref NR43_REG have their contents nybble swapped.
    - To maintain compatibility the value to write (or the value read) can be converted this way: `((uint8_t)(value << 4) | (uint8_t)(value >> 4))`
2. Register @ref NR32_REG has the volume bit values changed.
    - `Game Boy:  Bits:6..5 : 00 = mute, 01 = 100%, 10 = 50%, 11 = 25%`
    - `Mega Duck: Bits:6..5 : 00 = mute, 01 = 25%,  10 = 50%, 11 = 100%`
    - To maintain compatibility the value to write (or the value read) can be converted this way: `(((~(uint8_t)value) + (uint8_t)0x20u) & (uint8_t)0x60u)`

### Graphics Register Bit Changes
These changes are handled automatically when their GBDK definitions are used.

| @ref LCDC_REG Flag   | Game Boy | Mega Duck |        | Purpose                                          |
| -------------------- | -------- | --------- | ------ | ------------------------------------------------ |
| @ref LCDCF_B_ON      | .7       | .7        | (same) | Bit for LCD On/Off Select                        |
| @ref LCDCF_B_WIN9C00 | .6       | .3        |        | Bit for Window Tile Map Region Select            |
| @ref LCDCF_B_WINON   | .5       | .5        | (same) | Bit for Window Display On/Off Control            |
| @ref LCDCF_B_BG8000  | .4       | .4        | (same) | Bit for BG & Window Tile Data Region Select      |
| @ref LCDCF_B_BG9C00  | .3       | .2        |        | Bit for BG Tile Map Region Select                |
| @ref LCDCF_B_OBJ16   | .2       | .1        |        | Bit for Sprites Size Select                      |
| @ref LCDCF_B_OBJON   | .1       | .0        |        | Bit for Sprites Display Visible/Hidden Select    |
| @ref LCDCF_B_BGON    | .0       | .6        |        | Bit for Background Display Visible Hidden Select |


### Detailed Register Address Changes
These changes are handled automatically when their GBDK definitions are used.

| Register      | Game Boy | Mega Duck |
| ------------- | -------- | --------- |
| @ref LCDC_REG | 0xFF40   | 0xFF10    |
| @ref STAT_REG | 0xFF41   | 0xFF11    |
| @ref SCY_REG  | 0xFF42   | 0xFF12    |
| @ref SCX_REG  | 0xFF43   | 0xFF13    |
| @ref LY_REG   | 0xFF44   | 0xFF18    |
| @ref LYC_REG  | 0xFF45   | 0xFF19    |
| @ref DMA_REG  | 0xFF46   | 0xFF1A    |
| @ref BGP_REG  | 0xFF47   | 0xFF1B    |
| @ref OBP0_REG | 0xFF48   | 0xFF14    |
| @ref OBP1_REG | 0xFF49   | 0xFF15    |
| @ref WY_REG   | 0xFF4A   | 0xFF16    |
| @ref WX_REG   | 0xFF4B   | 0xFF17    |
| -             | -        | -         |
| @ref NR10_REG | 0xFF10   | 0xFF20    |
| @ref NR11_REG | 0xFF11   | 0xFF22    |
| @ref NR12_REG | 0xFF12   | 0xFF21    |
| @ref NR13_REG | 0xFF13   | 0xFF23    |
| @ref NR14_REG | 0xFF14   | 0xFF24    |
| -             | -        | -         |
| @ref NR21_REG | 0xFF16   | 0xFF25    |
| @ref NR22_REG | 0xFF17   | 0xFF27    |
| @ref NR23_REG | 0xFF18   | 0xFF28    |
| @ref NR24_REG | 0xFF19   | 0xFF29    |
| -             | -        | -         |
| @ref NR30_REG | 0xFF1A   | 0xFF2A    |
| @ref NR31_REG | 0xFF1B   | 0xFF2B    |
| @ref NR32_REG | 0xFF1C   | 0xFF2C    |
| @ref NR33_REG | 0xFF1D   | 0xFF2E    |
| @ref NR34_REG | 0xFF1E   | 0xFF2D    |
| -             | -        | -         |
| @ref NR41_REG | 0xFF20   | 0xFF40    |
| @ref NR42_REG | 0xFF21   | 0xFF42    |
| @ref NR43_REG | 0xFF22   | 0xFF41    |
| @ref NR44_REG | 0xFF23   | 0xFF43    |
| -             | -        | -         |
| @ref NR50_REG | 0xFF24   | 0xFF44    |
| @ref NR51_REG | 0xFF25   | 0xFF46    |
| @ref NR52_REG | 0xFF26   | 0xFF45    |
| -             | -        | -         |
