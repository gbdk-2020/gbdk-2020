@page docs_supported_consoles Supported Consoles & Cross Compiling
  
@anchor docs_consoles_supproted_list
# Consoles Supported by GBDK
As of version `4.0.5` GBDK includes support for other consoles in addition to the Game Boy. 

  - Nintendo Game Boy / Game Boy Color
  - Analogue Pocket
  - Sega Master System
  - Sega Game Gear

While the GBDK API has many convenience functions that work the same or similar across different consoles, it's important to keep their different capabilities in mind when writing code intended to run on more than one. Some (but not all) of the differences are screen sizes, color abilities, memory layouts, processor type (z80 vs gbz80/sm83) and speed.

 
@anchor docs_consoles_compiling
# Cross Compiling for Different Consoles

## lcc
When compiling and building through @ref lcc use the `-m<port>:<plat>` flag to select the desired console via it's port and platform combination.


## sdcc
When compiling directly with @ref sdcc use: `-m<port>`, `-D__PORT_<port>` and `-D__TARGET_<plat> `


## Console Port and Platform Settings
  - Nintendo Game Boy / Game Boy Color
    - @ref lcc : `-mgbz80:gb`
    - port:`gbz80`, plat:`gb`
  - Analogue Pocket
    - @ref lcc : `-mgbz80:ap`
    - port:`gbz80`, plat:`ap`

  - Sega Master System
    - @ref lcc : `-mz80:sms`
    - port:`z80`, plat:`sms`
  - Sega Game Gear
    - @ref lcc : `-mz80:gg`
    - port:`z80`, plat:`gg`


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

  - When `<sms/sms.h >` is included (either directly or through `<gbdk/platform.h>`)
    - When building for Master System
      - `SEGA` will be \#defined
      - `MASTERSYSTEM` will be \#defined
    - When building for Game Gear
      - `SEGA` will be \#defined
      - `GAMEGEAR` will be \#defined

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

# Porting from the Game Boy to the Analogue Pocket
The Analogue Pocket is (for practical purposes) functionally identical to the Game Boy / Color, but has a couple altered register flag and address definitions and a different boot logo. In order for software to be ported to the Analogue Pocket, or to run on both, the following practices should be used.

## Boot logo
As long as the target console is @ref docs_consoles_compiling "set during build time" then the correct boot logo will be automatically selected.

## Registers and Flags
Change these hardwired registers and register flags to use API defined ones.
   - LCDC register: @ref LCDC_REG or @ref rLCDC
   - STAT register: @ref STAT_REG or @ref rSTAT
   - LCDC flags: -> LCDCF_... (example: @ref LCDCF_ON)
   - STAT flags: -> STATF_... (example: @ref STATF_LYC)
