@page docs_migrating_versions Migrating to new GBDK Versions

This section contains information that may be useful to know or important when upgrading to a newer GBDK release.

# GBDK 2020 versions

## Porting to GBDK 2020 4.0.5
- GBDK now requires SDCC 12259 or higher with GBDK-2020 patches
- @ref utility_png2asset is the new name for the `png2mtspr` utility
- @ref lcc : Changed default output format when not specified from `.ihx` to `.gb` (or other active rom extension)
- The `_BSS` area is deprecated (use `_DATA` instead)
- Variables in static storage are now initialized to zero per C standard (but remaining WRAM is not cleared)

## Porting to GBDK 2020 4.0.4
- GBDK now requires SDCC 12238 or higher
- Made sample.h, cgb.h and sgb.h independent from gb.h

## Porting to GBDK 2020 4.0.3
  - No significant changes required

## Porting to GBDK 2020 4.0.2
 - The default font has been reduced from 256 to 96 characters.
   - Code using special characters may need to be updated.
   - The off-by-1 character index offset was removed for fonts. Old fonts with the offset need to be re-adjusted.

## Porting to GBDK 2020 4.0.1
 - __Important!__ : The `WRAM` memory region is no longer automatically initialized to zeros during startup.
   - Any variables which are declared without being initialized may have __indeterminate values instead of 0__ on startup. This might reveal previously hidden bugs in your code.
   - Check your code for variables that are not initialized before use.
   - In BGB you can turn on triggering exceptions (options panel) reading from unitialized RAM. This allows for some additional runtime detection of uninitialized vars.
 - In .ihx files, multiple writes to the same ROM address are now warned about using @ref ihxcheck.
 - `set_*_tiles()` now wrap maps around horizontal and vertical boundaries correctly. Code relying on it not wrapping correctly may be affected.


## Porting to GBDK 2020 4.0
 - GBDK now requires SDCC 4.0.3 or higher
 - The old linker `link-gbz80` has been REMOVED, the linker @ref sdldgb from SDCC is used.
   - Due to the linker change, there are no longer warnings about multiple writes to the same ROM address.
 - GBDK now generates .ihx files, those are converted to a ROM using @ref makebin (lcc can do this automatically in some use cases)
 - Setting ROM bytes directly with `-Wl-yp0x<address>=0x<value>` is no longer supported. Instead use @ref makebin flags. For example, use `-Wm-yC` instead of `-Wl-yp0x143=0xC0`. See @ref faq_gb_type_header_setting.
 - OAM symbol has been renamed to `_shadow_OAM`, that allows accessing shadow OAM directly from C code


## Porting to GBDK 2020 3.2
  - No significant changes required

## Porting to GBDK 2020 3.1.1
  - No significant changes required

## Porting to GBDK 2020 3.1
  - No significant changes required

## Porting to GBDK 2020 3.0.1
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

