@page docs_supported_consoles Supported Consoles & Cross Compiling
  
@anchor docs_consoles_supported_list
# Consoles Supported by GBDK
As of version `4.2.0` GBDK includes support for other consoles in addition to the Game Boy.

  - Game Boy and related clones
    - Nintendo Game Boy / Game Boy Color (GB/GBC)
    - Analogue Pocket (AP)
    - Mega Duck / Cougar Boy (DUCK)

  - Sega Consoles
    - Sega Master System (SMS)
    - Sega Game Gear (GG)

  - NES/Famicom (NES)

  - MSX DOS (MSXDOS) (partial support)

While the GBDK API has many convenience functions that work the same or similar across different consoles, it's important to keep their different capabilities in mind when writing code intended to run on more than one. Some (but not all) of the differences are screen sizes, color capabilities, memory layouts, processor type (z80 vs gbz80/sm83) and speed.

 
@anchor docs_consoles_compiling
# Cross Compiling for Different Consoles

## lcc
When compiling and building through @ref lcc use the `-m<port>:<plat>` flag to select the desired console via its port and platform combination. See below for available settings.


## sdcc
When building directly with the sdcc toolchain, the following must be specified manually
(when using @ref lcc it will populate these automatically based on `-m<port>:<plat>`).

When compiling with @ref sdcc-settings "sdcc":
  - `-m<port>`, `-D__PORT_<port>` and `-D__TARGET_<plat> `

When assembling select the appropriate include path: `-I<gbdk-path>lib/<plat>`.

The assemblers used are:
  - @ref sdasgb-settings "sdasgb" (for GB/AP)
  - @ref sdasz80-settings "sdasz80" (for SMS/GG)
  - @ref sdas6500-settings "sdas6500" (for NES)

When linking:
  - Select the appropriate include paths: `-k <gbdk-path>lib/<port>`, `-k <gbdk-path>lib/<plat>`
  - Include the appropriate library files `-l <port>.lib`, `-l <plat>.lib`
  - The crt will be under `  <gbdk-path>lib/<plat>/crt0.o`

The linkers used are:
   - @ref sdldgb-settings "sdldgb" (for GB/AP)
   - @ref sdldz80-settings "sdldz80" (for SMS/GG or MSXDOS)
   - @ref sdld6808-settings "sdld6808" (for NES)

MSXDOS requires an additional build step with @ref utility_makecom "makecom" after @ref makebin to create the final binary:
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

  - NES
    - @ref lcc : `-mmos6502:nes`
    - port:`mos6502`, plat:`nes`

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

  - When `<nes/nes.h>` is included (either directly or through `<gbdk/platform.h>`)
    - `NINTENDO_NES` will be \#defined

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
  - @ref MAX_HARDWARE_SPRITES
  - @ref HARDWARE_SPRITE_CAN_FLIP_X, @ref HARDWARE_SPRITE_CAN_FLIP_Y


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

In the example @ref utility_png2asset is used to generate assets in the native format for each console at compile-time from separate source PNG images. The Makefile is set to use the source PNG folder which matches the current console being compiled, and the source code uses @ref set_bkg_native_data() to load the assets tiles in native format to the tile memory used for background tiles on that platform.


# Hardware Summaries
The specs below reflect the typical configuration of hardware when used with GBDK and is not meant as a complete list of their capabilities.

GB/AP/DUCK
- Sprites:
  - 256 tiles (upper 128 are shared with background) (amount is doubled in CGB mode)
  - tile flipping/mirroring: yes
  - 40 total, max 10 per line
  - 2 x 4 color palette (color 0 transparent). 8 x 4 color palettes in CGB mode
- Background: 256 tiles (typical setup: upper 128 are shared with sprites) (amount is doubled in CGB mode)
  - tile grid size: 8x8
  - tile attribute grid size: 8x8 (CGB mode only)
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
  - tile grid size: 8x8
  - tile attribute grid size: 8x8
  - tile flipping/mirroring: yes
  - 2 x 16 color palettes
- Window "layer": not available
- SMS
  - Screen: 256 x 192
  - Hardware Map: 256 x 224
- GG
  - Screen: 160 x 144
  - Hardware Map: 256 x 224

NES/Famicom
- Sprites:
  - 8x8 or 8x16
  - 256 tiles
  - tile flipping/mirroring: yes
  - 64 total, max 8 per line
  - 4 x 4 color palette (color 0 transparent)
- Background: 256 tiles
  - tile grid size: 8x8
  - tile attribute grid size: 16x16 (bit packed into 32x32)
  - tile flipping/mirroring: no
  - 4 x 4 color palette (color 0 same for all sub-palettes)
- Window "layer": not available
- Screen: 256 x 240
- Hardware Map: Depends on mirroring mode
  - 256 x 240 (single-screen mirroring)
  - 512 x 240 (vertical mirroring / horizontal scrolling)
  - 256 x 480 (horizontal mirroring / vertical scrolling)
  - 512 x 480 (4-screen layout. Requires additional RAM on cartridge)

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


NES/Famicom
- See @ref nes_technical_details "NES technical details"

@anchor using_cgb_features
# Using Game Boy Color (GBC/CGB) Features

## Differences Versus the Regular Game Boy (DMG/GBP/SGB)
These are some of the main hardware differences between the Regular Game Boy and the Game Boy Color.

  - CPU: Optional 2x Speed mode
  - Serial Link: Additional Speeds 2KB/s, 32KB/s, 64KB/s
  - IR Port
  - Sprites:
    - 2 banks x 256 tile patterns (2x as many) (typically upper 128 of each bank shared with background)
    - 8 x 4 color palettes in CGB mode (BGR-555 per color, 32768 color choices)
  - Background:
    - 2 banks x 256 tile patterns (2x as many) (typically upper 128 of each bank shared with sprites)
    - Second map bank for tile attributes (color, flipping/mirroring, priority, bank)
    - 8 x 4 color palettes in CGB mode (BGR-555 per color, 32,768 color choices))
    - BG and Window master priority
  - WRAM: 8 x 4K WRAM banks in the 0xD000 - 0xDFFF region
  - LCD VRAM DMA

## Game Boy Color features in GBDK
These are some of the main GBDK API features for the CGB. 
Many of the items listed below link to additional information.

  - ROM header settings:
    - See the FAQ entry @ref faq_gb_type_header_setting "How do I set SGB, Color only and Color compatibility in the ROM header?"
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

### RAM Banks
- The SMS/GG ROM file size must be at least 64K to enable mapper support for RAM banks in emulators.
  - If the generated ROM is too small then `-yo 4` for makebin (or `-Wm-yo4` for LCC) can be used to set the size to 64K.

### Tile Data and Tile Map loading

#### Tile and Map Data in 2bpp Game Boy Format
- @ref set_bkg_data() and @ref set_sprite_data() will load 2bpp tile data in "Game Boy" format on both GB and SMS/GG.
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


### Colors and Palettes
The SMS/GG have 2 x 16 color palettes:
- The first (0) is just for the background
- The second (1) is shared between sprites and the background (and for sprites a single color 0 of that palette is transparent)

On the Game Gear
- Each Palette is 32 bytes in size: 16 colors x 2 bytes per palette color entry.
- Each color (16 per palette) is packed as BGR-444 format (x:4:4:4, MSBits [15..12] are unused).
- Each component (R, G, B) may have values from 0 - 15 (4 bits), 15 is brightest.

On the SMS
- On SMS each Palette is 16 bytes in size: 16 colors x 1 byte per palette color entry.
- Each color (16 per palette) is packed as BGR-222 format (x:2:2:2, MSBits [7..6] are unused).
- Each component (R, G, B) may have values from 0 - 3 (2 bits), 3 is brightest.

For setting palette data:
- @ref set_palette_entry(): Will set a single color in a palette
- @ref set_palette(): Can set all the colors for one or both palettes
- @ref set_bkg_palette(): Is just an alias for @ref set_palette(). The full 16 colors can be set using this call.
- @ref set_sprite_palette(): Is also an alias for @ref set_palette(), but it offsets to write to the second 16 color palette.
- Also see: @ref RGB(), RGB8(), @ref RGBHTML()


#### Emulated Game Boy Color map attributes on the SMS/Game Gear
On the Game Boy Color, @ref VBK_REG is used to select between the regular background tile map and the background attribute tile map (for setting tile color palette and other properties).

This behavior is emulated for the SMS/GG when using @ref set_bkg_tiles() and @ref VBK_REG. It allows writing a 1-byte tile map separately from a 1-byte attributes map.

@note Tile map attributes on SMS/Game Gear use different control bits than the Game Boy Color, so a modified attribute map must be used.


@anchor nes_technical_details
## From Game Boy to NES

The NES graphics architecture is similar to the GB's. However, there are a number of design choices in the NES hardware that make the NES a particularly cumbersome platform to develop for, and that will require special attention.

Most notably:
* PPU memory can only be written in a serial fashion using a data port at 0x2007 (PPUDATA)
* PPU memory can only be written to during vblank, or when manually disabling rendering via PPUMASK. Hblank writes to PPU memory are not possible
* PPU memory write address is re-purposed for scrolling coordinates when rendering is enabled which means PPU memory updates / scrolling must cooperate
* PPU internal palette memory is also mapped to external VRAM area making palette updates during rendering very expensive and error-prone
* The base NES system has no support for any scanline interrupts. And cartridge mappers that add scanline interrupts do so using wildly varying solutions
* There's no easy way to determine the current scanline or CPU-to-PPU alignment meaning timed code is often required on the NES
* The PAL variant of the NES has very different CPU / PPU timings, as do the Dendy clone and other clone systems
* The stock 2 kB CPU RAM is just 1/4th the 8kB CPU RAM on a Game Boy
  - Free RAM after accounting for ZP, stack, OAM page and system variables further cuts this in half
  - This means a lot of GB code will need to be carefully optimized for RAM usage when ported to the NES
  - In particular, make sure to use the "const" modifier for arrays that are read-only, to make sure they don't end up in RAM
  
To provide an easier experience, gbdk-nes attempts to hide most of these quirks so that in theory the programming experience for gbdk-nes should be as close as possible to that of the GB/GBC. However, to avoid surprises it is recommended to familiarize yourself with the NES-specific quirks and implementation choices mentioned here.

This entire section is written as a guide on porting GB projects to NES. If you are new to GBDK, you may wish to familiarize yourself with using GBDK for GB development first as the topics covered will make a lot more sense after gaining experience with GB development.

### Mapper

Currently the NES support in GBDK uses UNROM-512 (Mapper30) with single-screen mirroring.

### Buffered mode vs direct mode

On the GB, the vblank period serves as an optimal time to write data to PPU memory, and PPU memory can also be written efficiently with VRAM DMA.

On the NES, writing PPU memory during the vblank period is not optional. Whenever rendering is turned on the PPU is in a state where accessing PPU memory results in undefined behavior outside the short vblank period. The NES also has no VRAM DMA hardware to help with data writes. This makes the vblank period not only more precious, but important to never exceed to avoid glitched games.

To deal with this limitation, all functions in gbdk-nes that write to PPU memory can run in either *Buffered* or *Direct* mode.

The good news is that switching between buffered and direct mode in gbdk-nes is usually done behind-the-scenes and normally shouldn't affect your code too much, as long as you use the portable GBDK functions and macros to do this.

* DISPLAY_ON / SHOW_BG / SHOW_SPR will all switch the system into buffered mode, allowing limited amounts of transfers during vblank, without affecting the display of graphics
* DISPLAY_OFF will switch the system into direct mode, allowing much larger/faster transfers while the screen is blanked

The following sections describe how the buffered / direct modes work in more detail. As buffered / direct mode is mostly hidden by the API calls, feel free to skip these sections if you wish.

#### Buffered mode implementation details

To take maximum advantage of the short vblank period, gbdk-nes implements a popular optimization: An unrolled loop that pulls prepared data bytes from the stack.

    PLA
    STA PPUDATA
    ...
    PLA
    STA PPUDATA
    RTS

The data structure to facilitate this is usually called a vram transfer buffer, often affectionately called a "popslide" buffer after Damian Yerrick's implementation. This buffer essentially forms a list of commands where each command sets up a new PPU address and then writes a sequence of bytes with an auto-increment of either +1 or +32. Each such command is often called a "stripe" in the nesdev community.

The transfer buffer starts at 0x100 and takes around half of the hardware stack page. You can think of the transfer buffer as a software-implemented DMA that allows writing bytes at the optimal rate of 8 cycles / byte. (ignoring the PPU address setup cost)

The buffer supports writing up to 32 continuous bytes at a time. This allows updating a full screen row / column, or two 8x8 tiles worth of tile data in one command / "stripe".

By doing writes to this buffer during game logic, your game will effectively keep writing data transfer commands for the vblank NMI handler to process in the next vblank period, without having to wait until the vblank.

Given that the transfer buffer only has space for around 100 data bytes, it is important to not overfill the buffer, as this will bring code execution to a screeching halt, until the NMI handler empties the old contents of the buffer to free up space and allow new commands to be written.

Buffered mode is typically used for scrolling updates or dynamically animated tiles, where only a small amount of bytes need updating per frame.

#### Direct mode implementation details

During direct mode, all graphics routines will write directly to the PPUADDR / PPUDATA ports and the transfer buffer limit is never a concern because the transfer buffer is effectively bypassed.

Direct mode is typically used for initializing large amounts of tile data at boot and/or level loading time. Unless you plan to have an animated loading screen and decompress a lot of data, it makes more sense to just fade the screen to black and allow direct mode to write data as fast as possible.

Direct mode also affects how (fake) interrupt handlers are processed. As long as vsync() is called on each frame, the VBL and LCD handlers will still be executed in direct mode - but no graphics registers will be written.

The TIM handler will still be executed as normal.

#### Caveat: Make sure the transfer buffer is emptied before switching to direct mode

Because the switch to direct mode is instant and doesn't wait for the next invocation of the vblank, it is possible to create situations where there is still remaining data in the transfer buffer that would only get written once the system is switched back to buffered mode.

To avoid this situation, make sure to always "drain" the buffer by doing a call to vsync when you expect your code to finish.

#### Caveat: Only update the PPU palette during buffered mode

The oddity that PPU palette values are accessed through the same mechanism as other PPU memory bytes comes with the side effect that the vblank NMI handler will only write the palette values in buffered mode.

The reason for this design choice is two-fold:
* Having the NMI handler keep doing the palette updates when in direct mode would result in a race condition when the NMI handler interrupts the direct mode code and messes with the PPUADDR state that the direct mode code expects to remain unchanged
* Having the palette updates also switch to direct mode would run into another quirk of the system: Pointing PPUADDR at palette registers when display is turned off will make the display output that palette color instead of the common background color. The result would be glitchy artifacts on screen when updating the palette, leading to a slightly-glitchy looking game whenever the palette is updated with the screen off

To work around this, you are advised to never fully turn the display off during a palette fade. If you don't follow this advice all your palette updates will get delayed until the screen is turned back on.

### Shadow PPU registers

Like the SMS, the NES hardware is designed to only allow loading the full X/Y scroll on the very first scanline. i.e., under normal operation you are only allowed to change the Y-scroll once.

In contrast to the SMS, this limitation can be circumvented with a specific set of out-of-order writes to the PPUSCROLL/PPUADDR registers, taking advantage of the quirk that the PPUADDR and PPUSCROLL share register bits. But this write sequence is very timing-sensitive as the writes need to fall into (a smaller portion of) the hblank period in order to avoid race conditions when the CPU and PPU both try to update the same register during scanline rendering.

To simplify the programming interface, gbdk-nes functions like move_bkg / scroll_bkg only ever update shadow registers in RAM. The vblank NMI handler will later pick these values up and write them to the actual PPU registers registers.

### Implementation of (fake) vbl / lcd handlers

GBDK provides an API for installing Interrupt Service Routines that execute on start of vblank (VBL handler), or on a specific scanline (LCD handler).

But the base NES system has no suitable scanline interrupts that can provide such functionality. So instead, gbdk-nes API allows *fake* handlers to be installed in the goal of keeping code compatible with other platforms.

* An installed VBL handler will be called immediately when calling vsync. This handler should only update PPU shadow registers.
* An installed LCD handler for a specific scanline will then be called repeatedly until the value of _lcd_scanline is either set to an earlier scanline or >= 240. After each invocation, shadow registers are stored into a buffer.
* After the vblank NMI handler has finished palette updates, OAM DMA, VRAM updates and scroll updates it will then manually run a delay loop to reach the particular scanlines that the installed LCD handler was pre-called for, and use the contents of the buffer to update registers.

Because the LCD "ISR" is actually implemented with a delay loop, it will burn a lot of CPU cycles in the frame - the further down the requested scanline is the larger the CPU cycle loss.
In practice this makes this faked-LCD-ISR functionality mostly suitable for status bars at the top of the screen screen. Or for simple parallax cutscenes where the CPU has little else to do.

@note The support for VBL and LCD handlers is currently under consideration and subject to change in newer versions of gbdk-nes.

### Caveat: Make sure to call vsync on every frame

On the GB, the call to vsync is an optional call that serves two purposes:

1. It provides a consistent frame timing for your game 
2. It allows future register writes to be synchronized to the screen

On gbdk-nes the second point is no longer true, because writes need to be made to the shadow registers *before* vsync is called.

But the vsync call serves three other very important purposes:

A. It calls the optional VBL handler, where shadow registers can be written (and will later be picked up by the actual vblank NMI handler)
B. It repeatedly calls the optional LCD handler up to MAX_LCD_ISR_CALLS times. After each call, PPU shadow registers are stored into a buffer that will later be used by timed code in the NMI to handle mid-frame changes for screen splits / sprite hiding / etc.
C. It calls flush_shadow_attributes so that updates to background attributes actually get written to PPU memory

For these reasons you should always include a call to vsync if you expect to see any graphical updates on the screen.

### Caveat: Do all status bar scroll movement in LCD handlers to mitigate glitches

The fake LCD ISR system is not bullet-proof. In particular, it has a problem where lag frames can cause the shadow register updates in LCD handlers not to be ready in time for when the timed code in the NMI handler would be called. This will effectively cause all those updates to be missing for one frame, and result in glitched scroll updates.

There is currently no complete work-around for this problem other than avoiding lag frames altogether. But the glitch can be made less distracting by making sure only the status bar glitches rather than the main background.

If you are using LCD handlers to achieve a top-screen stationary status bar, it is recommended that you follow the following guidelines to make sure the background itself has consistent scrolling:
* Use move_bkg either in your main loop or in the VBL handler, to set the level scrolling
* Use move_bkg in the first invocation of the LCD handler, to set the (stationary) status bar scroll position
* Use move_bkg in the second invocation of the LCD handler, to reset the background scrolling

In short: Ensuring that the last called LCD handler sets the scroll back to the original value means the PPU rendering keeps rendering the background from the same scrolling position even when the NMI handling was missed.

### Implementation of timer handler

The nature of the deferred handling for fake VBL and LCD handlers in gbdk-nes means that lag frames will cause these handlers to be called at delayed irregular times.

For graphics updates this is the behaviour you usually want. But for non-graphics tasks like music playback it will cause distracting stutter.

The timer overflow handler (TIM) provides an alternative method that is guaranteed to be stutter-free. The TIM handler is always called if timer overflow occurs at the end of the NMI handler in both buffered and direct mode. 

The TMA_REG hardware register is emulated via a RAM variable with the same name. TMA_REG is set to 0xFF at reset, corresponding to 60Hz for a Famicom / US NES, and 50Hz for a PAL NES / PAL Famiclone. Changing TMA_REG allows setting a slower frequency for this emulated timer overflow interrupt if your GB game uses the timer overflow hardware for regular events like music that are slower than 60Hz (50Hz for PAL).

If you are porting a GB game to gbdk-nes where the music handler is called in the VBL or LCD handler then it is advisable to move this call to the timer handler, in order to achieve reliable music playback at 60Hz (50Hz for PAL).

Keep in mind that you should NOT write any graphics registers in the TIM handler. This will likely not do what you want, and it may result in bad graphical glitches.

### Tile Data and Tile Map loading

#### Tile and Map Data in 2bpp Game Boy Format
- @ref set_bkg_data() and @ref set_sprite_data() will load 2bpp tile data in "Game Boy" format on both GB and NES.
<!--- On the NES @ref set_2bpp_palette() ... not support.  -->
- @ref set_bkg_tiles() loads 1-byte-per-tile tilemaps both for the GB and NES.

#### Tile and Map Data in Native Format
Use the following api calls when assets are available in the native format for each platform.

@ref set_native_tile_data()
  - GB/AP: loads 2bpp tiles data
  - NES: loads 2bpp tiles data

@ref set_tile_map()
  - GB/AP: loads 1-byte-per-tile tilemaps
  - NES: loads 1-byte-per-tile tilemaps

Bit-depth specific API calls:
- 1bpp: @ref set_1bpp_colors, @ref set_bkg_1bpp_data, @ref set_sprite_1bpp_data
- 2bpp: @ref set_2bpp_palette, @ref set_bkg_2bpp_data, @ref set_sprite_2bpp_data

Platform specific API calls:
- set_bkg_attributes_nes16x16(), set_bkg_submap_attributes_nes16x16(), set_bkg_attribute_xy_nes16x16()


#### Game Boy Color map attributes on the NES
On the Game Boy Color, @ref VBK_REG is used to select between the regular background tile map and the background attribute tile map (for setting tile color palette and other properties).

This behavior of setting VBK_REG to specify tile indices/attributes is not supported on the NES platform. Instead the dedicated functions for attribute setting should be used. These will work on other platforms as well and are the preferred way to set attributes.

To maintain API compatibility with other platforms that have attributes on an 8x8 grid specified with a whole byte per attribute, the NES platform supports the dedicated calls for setting attributes on an 8x8 grid:
- set_bkg_attributes()
- set_bkg_submap_attributes()
- set_bkg_attribute_xy()

This allows code to for attribute setting to remain unchanged between platforms. The effect of using these calls is that some attribute setting will be redundant due to the coarser attribute grid. i.e., setting the attribute at coordinates (4, 4), (4,5), (5, 4) and (5, 5) will all set the same attribute.

There is one more platform specific difference to note: While the set_bkg_attribute_xy() function takes coordinates on a 8x8 grid, the set_bkg_attributes() and set_bkg_submap_attributes() functions take a pointer to data in NES packed attribute format, where each byte contains data for 4 16x16 attribute. i.e. a 32x32 region. 

While this implementation detail of how the attribute map is encoded is usually hidden by the API functions it does mean that code which manually tries to read the attribute data is *NOT* portable between NES/other platforms, and is not recommended.

@note Tile map attributes on NES are on a 16x16 grid and use different control bits than the Game Boy Color.
- NES 16x16 Tile Attributes are bit packed into 4 attributes per byte with each 16x16 area of a 32x32 pixel block using the bits as follows:
  - D1-D0: Top-left 16x16 pixels
  - D3-D2: Top-right 16x16 pixels
  - D5-D4: Bottom-left 16x16 pixels
  - D7-D6: Bottom-right 16x16 pixels
  - https://www.nesdev.org/wiki/PPU_attribute_tables


## From Game Boy to Mega Duck / Cougar Boy
The Mega Duck is (for practical purposes) functionally identical to the Original Game Boy though it has a couple changes listed below.

### Summary of Hardware changes:
  - Cartridge Boot Logo: not present on Mega Duck
  - Cartridge Header data: not present on Mega Duck
  - Program Entry Point: `0x0000` (on Game Boy: `0x0100` )
  - Display registers and flag definitions: Some changed
  - Audio registers and flag definitions: Some changed
  - MBC ROM bank switching register address: `0x0001` (many Game Boy MBCs use `0x2000 - 0x3FFF`)
  - Also see @ref links_megaduck_docs "links for megaduck development"

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
