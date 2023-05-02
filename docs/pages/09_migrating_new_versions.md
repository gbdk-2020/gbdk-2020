@page docs_migrating_versions Migrating to new GBDK Versions

This section contains information that may be useful to know or important when upgrading to a newer GBDK release.

# GBDK-2020 versions

## Porting to GBDK-2020 4.2
  - GBDK now requires SDCC 4.3 or higher with GBDK-2020 patches for the the z80 linker
  - The following new functions replace old ones.
    - The old functions will continue to work for now, but migration to new versions is strongly encouraged.
    - vsync(): replaces wait_vbl_done()
    - set_default_palette(): replaces cgb_compatibility()
    - move_metasprite_flipy(): replaces move_metasprite_hflip()
    - move_metasprite_flipx(): replaces move_metasprite_vflip()
    - move_metasprite_flipxy(): replaces move_metasprite_hvflip()
    - move_metasprite_ex(): replaces move_metasprite()

## Porting to GBDK-2020 4.1.1
  - No significant changes required


## Porting to GBDK-2020 4.1.0
  - GBDK now requires SDCC 4.2 or higher with GBDK-2020 patches for the the z80 linker
  - The default calling convention changed in SDCC 4.2, see @ref sdcc_calling_convention "Calling Conventions" for more details.
    - If you are linking to libraries compiled with an older version of SDCC / GBDK then you may have to recompile them.
    - If there are existing functions written in ASM which __receive parameters__ they should also be reviewed to make sure they work with the new `__sdcccall(1)` calling convention, or have their header declaration changed to use `OLDCALL`.
    - If there are existing functions written in ASM which __call other functions written in C__ the callee C function should be declared OLDCALL.
    - Function pointer declarations should be checked to see if they need OLDCALL added to the declaration.
      - Example (add OLDCALL at the end)
      - FROM: `typedef void (*someFunc)(uint8_t, uint8_t);`
      - TO:   `typedef void (*someFunc)(uint8_t, uint8_t) OLDCALL;`
    - If you are using tools such as `rgb2sdas` (from hUGETracker/Driver) you may need to edit the resulting .o file and replace `-mgbz80` with `-msm83` in addition to using `OLDCALL`
  - The SDCC `PORT` name for the Game Boy and related clones changed from `gbz80` to `sm83`.
    - Additional details in the @ref console_port_plat_settings "Console Port and Platform Settings" section and @ref faq_gbz80_sm83_old_port_name_error "FAQ entry". @ref lcc will error out if the old `PORT` name is passed in.
  - The library base path changed from `lib/small/asxxxx/` to `lib/`.
    - For example  `lib/small/asxxxx/gb` becomes `lib/gb`
  - Allocations for ISR chain lengths were fixed.
    - Now they are VBL: 4 user handlers, LCD: 3 user handlers, SIO/TIM/JOY: 4 user handlers
    

## Porting to GBDK-2020 4.0.6
  - Renamed `bgb_emu.h` to `emu_debug.h` and BGB_* functions to EMU_*
    - Aliases for the BGB_* ones and a `bgb_emu.h` shim are present for backward compatibility, but updating to the new naming is recommended


## Porting to GBDK-2020 4.0.5
  - GBDK now requires SDCC 12259 or higher with GBDK-2020 patches
  - Variables in static storage are now initialized to zero per C standard (but remaining WRAM is not cleared)
  - @ref utility_png2asset "png2asset" is the new name for the `png2mtspr` utility
  - @ref lcc : Changed default output format when not specified from `.ihx` to `.gb` (or other active rom extension)
  - The `_BSS` area is deprecated (use `_DATA` instead)
  - The `_BASE` area is renamed to `_HOME`
  - Variables in static storage are now initialized to zero per C standard (but remaining WRAM is not cleared)
  - @ref itoa(), @ref uitoa(), @ref ltoa(), @ref ultoa() all now require a radix value (base) argument to be passed. On the Game Boy and Analogue Pocket the parameter is required but not utilized.
  - set_bkg_1bit_data has been renamed to @ref set_bkg_1bpp_data
  - The following header files which are now cross platform were moved from `gb/` to `gbdk/`: `bcd.h`, `console.h`, `far_ptr.h`, `font.h`, `gbdecompress.h`, `gbdk-lib.h`, `incbin.h`, `metasprites.h`, `platform.h`, `version.h`
    - When including them use `#include <gbdk/...>` instead of `#include <gb/>`
  
## Porting to GBDK-2020 4.0.4
  - GBDK now requires SDCC 12238 or higher
  - Made sample.h, cgb.h and sgb.h independent from gb.h


## Porting to GBDK-2020 4.0.3
  - No significant changes required


## Porting to GBDK-2020 4.0.2
 - The default font has been reduced from 256 to 96 characters.
   - Code using special characters may need to be updated.
   - The off-by-1 character index offset was removed for fonts. Old fonts with the offset need to be re-adjusted.


## Porting to GBDK-2020 4.0.1
 - __Important!__ : The `WRAM` memory region is no longer automatically initialized to zeros during startup.
   - Any variables which are declared without being initialized may have __indeterminate values instead of 0__ on startup. This might reveal previously hidden bugs in your code.
   - Check your code for variables that are not initialized before use.
   - In BGB you can turn on triggering exceptions (options panel) reading from unitialized RAM. This allows for some additional runtime detection of uninitialized vars.
 - In .ihx files, multiple writes to the same ROM address are now warned about using @ref ihxcheck.
 - `set_*_tiles()` now wrap maps around horizontal and vertical boundaries correctly. Code relying on it not wrapping correctly may be affected.


## Porting to GBDK-2020 4.0
 - GBDK now requires SDCC 4.0.3 or higher
 - The old linker `link-gbz80` has been REMOVED, the linker @ref sdldgb from SDCC is used.
   - Due to the linker change, there are no longer warnings about multiple writes to the same ROM address.
 - GBDK now generates .ihx files, those are converted to a ROM using @ref makebin (lcc can do this automatically in some use cases)
 - Setting ROM bytes directly with `-Wl-yp0x<address>=0x<value>` is no longer supported. Instead use @ref makebin flags. For example, use `-Wm-yC` instead of `-Wl-yp0x143=0xC0`. See @ref faq_gb_type_header_setting.
 - OAM symbol has been renamed to `_shadow_OAM`, that allows accessing shadow OAM directly from C code


## Porting to GBDK-2020 3.2
  - No significant changes required

## Porting to GBDK-2020 3.1.1
  - No significant changes required

## Porting to GBDK-2020 3.1
  - Behavior formerly enabled by USE_SFR_FOR_REG is on by default now (no need to specify it, it isn't a tested `#ifdef` anymore). check here why:
    https://gbdev.gg8.se/forums/viewtopic.php?id=697

## Porting to GBDK-2020 3.0.1
 - LCC was upgraded to use SDCC v4.0. Makefile changes may be required
   - The symbol format changed. To get bgb compatible symbols turn on `.noi` output (LCC argument: `-Wl-j` or `-debug`) and use `-Wm-yS`
   - ?? Suggested: With LCC argument: `-Wa-l` (sdasgb:`-a   All user symbols made global`)
   - In SDCC 3.6.0, the default for char changed from signed to unsigned.
     - If you want the old behavior use `--fsigned-char`.
     - lcc includes `--fsigned-char` by default
     - Explicit declaration of unsigned vars is encouraged (for example, '15U' instead of '15')
   - `.init` address has been removed

--------------------------------------------------

# Historical GBDK versions

## GBDK 1.1 to GBDK 2.0
 - Change your int variables to long if they have to be bigger than 255. If they should only contain values between 0 and 255, use an unsigned int.
 - If your application uses the delay function, you'll have to adapt your delay values.
 - Several functions have new names. In particular some of them have been changed to macros (e.g. show_bkg() is now SHOW_BKG).
 - You will probably have to change the name of the header files that you include.

