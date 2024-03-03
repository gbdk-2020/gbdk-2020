@page docs_releases GBDK Release Notes

The GBDK-2020 releases can be found on Github:
https://github.com/gbdk-2020/gbdk-2020/releases


# GBDK-2020 Release Notes

## GBDK-2020 4.2
  2023/08
  - Includes SDCC version ~4.3 with GBDK-2020 patches for Z80 and NES
    - ([Patched SDCC Builds](https://github.com/gbdk-2020/gbdk-2020-sdcc/releases) with support for Sega GG/SMS and the Nintendo NES are used. See the [github workflow](https://github.com/gbdk-2020/gbdk-2020-sdcc/tree/main/.github/workflows) for details
  - Known Issues
    - SDCC may fail on Windows when @ref windows_sdcc_non_c_drive_path_spaces "run from folder names with spaces on non-C drives".
  - Library
    - Added `NORETURN` definition (for `_Noreturn`)
    - Added: set_bkg_attributes(), set_bkg_submap_attributes(), set_bkg_attribute_xy()
    - The following new functions replace old ones. The old functions will continue to work for now, but migration to new versions is strongly encouraged.
      - vsync(): replaces wait_vbl_done()
      - set_default_palette(): replaces cgb_compatibility()
    - metasprites: added metasprite functions which can set base sprite property parameter (`__current_base_prop`) for GB/GBC and NES
      - move_metasprite_flipy(): replaces move_metasprite_hflip()
      - move_metasprite_flipx(): replaces move_metasprite_vflip()
      - move_metasprite_flipxy(): replaces move_metasprite_hvflip()
      - move_metasprite_ex(): (replaces move_metasprite()
    - NES
      - Added support for much of the GBDK API
      - Banking support (library and sdcc toolchain)
      - Added set_bkg_attributes_nes16x16(), set_bkg_submap_attributes_nes16x16(), set_bkg_attribute_xy_nes16x16()
    - SMS/GG
      - Swapped A and B buttons to match game boy buttons
      - X coordinate metasprite clipping on the screen edges
    - Game Boy
      - Minor crt0 optimizations
      - Faster vmemcpy(), set_data(), get_data()
      - Fixed hide_sprites_range(39u, 40u); overflow shadow OAM
      - Increased sgb_transfer() maximum packet length to 7 x 16 bytes
      - Convert gb_decompress routines to the new calling convention
      - Convert rle_decompress routines to the new calling convention
      - Removed legacy MBC register definitions `.MBC1_ROM_PAGE`  and `.MBC_ROM_PAGE` 
      - Workaround for possible HALT bug in Crash Handler
    - Refactored interrupts to use less space
  - Toolchain / Utilities
    - Added @ref utility_png2hicolorgb "png2hicolorgb"
    - @ref lcc "lcc"
      - Fixed `--sdccbindir`
      - Removed the unused `-DINT_16_BITS` from the default SDCC compiler and preprocessor arguments
      - Improved improved Game Gear header compatibility (change header region code from 4 to 6)
    - @ref utility_png2asset "png2asset"
      - Added `-o` as a more standard version of the `-c` argument
      - Added `-repair_index_pal`: Tries to repair tile palettes for indexed color pngs (such as when RGB paint programs mix up indexed colors if the same color exists in multiple palettes). Implies `-keep_palette_order`
      - Added `-no_palettes`: Do not export palette data
      - Fixed support for indexed color pngs with less than 8 bits color depth
      - Fixed incorrect palettes when different colors have same luma value (use RGB values as less-significant bits)
      - Fixed `-keep_duplicate_tiles` not working with `-source_tileset`
      - Changed to use cross-platform constants for metasprite properties (S_FLIPX, S_FLIPY and S_PAL)
    - @ref makebin
      - Warn if RAM banks specified and file size of ROM is less than the 64K required to enable them with in emulators
    - Added sdld6808 (for NES)
  - Examples
     - Fixed mkdir broken in some compile.bat files (remove unsupported -p flag during bat file conversion)
     - Sound Test: Added MegaDuck support
     - Wav Playback: Improved support on AGB/AGS hardware
     - Metasprites: Added sub-palette switching for GBC and NES, software metasprite flipping for sms/gg
     - Large Map: Added color for supported platforms
     - LCD ISR Wobble: Improved interrupt flag settings
     - Added GB-Type example
     - Added Game Boy Color Hi-Color example using @ref utility_png2hicolorgb "png2hicolorgb"
  - Docs:
    - Improved search to do partial matches instead of matching start of string only
    - Added SDAS assembler manual (asmlnk_manual.txt)
    - Added section on @ref nes_technical_details "NES support"
    - Added @ref using_cgb_features "Using Game Boy Color Features"
    - Updated @ref megaduck_sound_register_value_changes "MegaDuck hardware documentation"
    - Added @ref banked_calling_convention "Banked Calling Convention"
    - Added mention of @ref MAX_HARDWARE_SPRITES

## GBDK-2020 4.1.1
  2022/11
  - Includes SDCC version 13350 with GBDK-2020 patches for Z80
  - Library
    - Fixed @ref RGB() and @ref RGB8() macros

## GBDK-2020 4.1.0
  2022/10
  - Includes SDCC version 13350 with GBDK-2020 patches for Z80
  - Known Issues
    - The `compile.bat` batch files for Windows use the an invalid `-p` option for `mkdir`
  - Building GBDK
    - The linux port of SDCC is custom built on Ubuntu 16.04 due to reduced GLIBC compatibility issues in more recent SDCC project builds.
    - Added Windows 32-Bit build
  - Platforms
    - SDCC has renamed the `gbz80` port to `sm83` see @ref faq_gbz80_sm83_old_port_name_error
    - Added experimental support for MSXDOS (`msxdos`) and NES (`nes`). These platforms are not fully functional at this time. See @ref docs_supported_consoles
  - Licensing
    - Clarified licensing status with consent from GBDK original authors, added licensing folder to distribution
  - Library
    - SGB: Use longer wait between the SGB packet transfers
    - SMS/GG: less garbage on screen when clearing VRAM in the init code
    - SMS/GG: Added @ref cgb_compatibility() to set default palette with the four shades of gray
    - Fixed: @ref get_sprite_data(), @ref get_bkg_data() , @ref get_win_data() when @ref LCDCF_BG8000 bit of @ref LCDC_REG is set
    - Fixed ISR chain lengths. VBL: 4 user handlers, LCD: 3 user handlers, SIO/TIM/JOY: 4 user handlers
    - Added new constants for the Game Boy Color (CGB):
      - @ref VBK_BANK_0, @ref VBK_BANK_1
      - @ref VBK_TILES, @ref VBK_ATTRIBUTES
      - @ref BKGF_PRI, @ref BKGF_YFLIP, @ref BKGF_XFLIP, @ref BKGF_BANK0, @ref BKGF_BANK1
      - @ref BKGF_CGB_PAL0, @ref BKGF_CGB_PAL1, @ref BKGF_CGB_PAL2, @ref BKGF_CGB_PAL3, @ref BKGF_CGB_PAL4, @ref BKGF_CGB_PAL5, @ref BKGF_CGB_PAL6, @ref BKGF_CGB_PAL7
      - @ref VBK_TILES, @ref VBK_ATTRIBUTES
  - Toolchain / Utilities
    - @ref lcc
      - Changed to Error out and warn when `gbz80` port is used instead of `sm83`
    - @ref utility_png2asset "png2asset"
      - Added `-tiles_only`: Export tile data only
      - Added `-maps_only`: Export map tilemap only
      - Added `-metasprites_only`: Export metasprite descriptors only
      - Added `-source_tileset`: Use source tileset image with common tiles
      - Added `-keep_duplicate_tiles`: Do not remove duplicate tiles
      - Added `-bin`: Export to binary format (includes header files)
      - Added `-transposed`: Export transposed (column-by-column instead of row-by-row)
      - Added basic MSXDOS support
        - Added 1bpp packing mode (BPP1)
        - `-spr16x16msx`
      - Added basic NES support
        - `-use_nes_attributes`
        - `-use_nes_colors`
      - Changed to only export `_tile_pals[]` arrays when `-use-structs` is set (ZGB specific)
    - @ref utility_gbcompress "gbcompress"
      - Added `--bank=<num>` Add Bank Ref: 1 - 511 (default is none, with `--cout` only)
      - Fixed failure to flush data at end of compression (uncommitted bytes)
      - Fixed `Warning: File read size didn't match expected`
    - @ref lcc "lcc"
      - When `-autobank` is specified `lcc` will automatically add `-yoA` for @ref makebin if no `-yo*` entry is present
      - Fixed broken `-E` Preprocess only flag
    - @ref utility_makecom "makecom"
      - Added `makecom` for post-processing msxdos binaries
    - @ref makebin
      - Fixed (via sdcc) bug with `-yp` not always working correctly
        - https://sourceforge.net/p/sdcc/code/12975/
    - @ref bankpack
      - Added support for the Game Boy Camera MBC
      - Added `-reserve=<bank>:<size>` option to reserve space during autobank packing
        - Workaround for libraries that contain objects in banks (such as gbt-player)
    - @ref ihxcheck
      - Check and warn for bank overflows under specific conditions
        - A multiple write to the same address must occur. The address where the overlap ends is used as BANK.
        - There must also be a write which spans multiple banks, the ending address of that must match BANK. The starting addresses is the OVERFLOW-FROM BANK. 
  - Examples
     - Changed Logo example to use new GBDK logo art from user "Digit"
     - Added example for APA image mode with more than 256 tiles
     - Added SGB Sound Effects example
     - Changed to new WAV sound example
  - Docs
    - Added improved @ref mbc_type_chart "MBC Type chart"
    - Include SDCC manual in pdf format     
    - Various doc updates and improvements

## GBDK-2020 4.0.6
  2022/02
  - Includes SDCC version 12539 with GBDK-2020 patches for Z80
  - Building GBDK
    - Changed to target older version of macOS (10.10) when building for better compatibility
  - Platforms
    - Added support for Mega Duck / Cougar Boy (`duck`). See @ref docs_supported_consoles
  - Library
    - Added @ref memcmp()
    - Added @ref add_low_priority_TIM() function for timer interrupts which allow nesting for GB/CGB
    - Added @ref set_bkg_based_tiles(), @ref set_bkg_based_submap(), @ref set_win_based_tiles(), @ref set_win_based_submap() for when a map's tiles don't start at VRAM index zero
    - Added @ref clock() for SMS/GG
    - Added macro definitions for SDCC features:
      - `#define SFR     __sfr`
      - `#define AT(A)   __at(A)`
    - Added check for OAM overflow to metasprite calls for GB/CGB
    - Added constant definitions @ref PSG_LATCH, @ref PSG_CH0, @ref PSG_CH1, @ref PSG_CH2, @ref PSG_CH3, @ref PSG_VOLUME for SMS/GG
    - Renamed `bgb_emu.h` to `emu_debug.h` and BGB_* functions to EMU_*.
      - Aliases for the BGB_* ones and a `bgb_emu.h` shim are present for backward compatibility
    - Changed headers to wrap SDCC specific features (such as `NONBANKED`) with `#ifdef __SDCC`
    - Changed @ref rand() and @ref arand() to return `uint8_t` instead of `int8_t` (closer to the standard)
    - Fixed declaration for @ref PCM_SAMPLE and definition for @ref AUD3WAVE
    - Fixed definition of `size_t` to be `unsigned int` instead of  `int`
    - Fixed @ref vmemcpy() and @ref memmove() for SMS/GG
    - Fixed random number generation for SMS/GG
    - Fixed letter `U` appearing as `K` for min font
    - Fixed define name in crash_handler.h
    - Exposed @ref __rand_seed
  - Toolchain / Utilities
    - @ref utility_png2asset "png2asset"
      - Added SMS/GG graphics format support
      - Added 4bpp and SGB borders
      - Added warning when image size is not an even multiple of tile size
      - Added `-tile_origin` offset option for when map tiles do not start at tile 0 in VRAM
      - Added `*_TILE_COUNT` definition to output
      - Fixed CGB `...s_map_attributes` type definition in output
      - Fixed values for `num_palettes` in output
      - Fixed incorrect `TILE_COUNT` value when not `-using_structs`
    - @ref lcc "lcc"
      - Changed @ref makebin flags to turn off Nintendo logo copy for GB/CGB (use version in crt instead)
      - Fixed lcc handling of makebin `-x*` arguments
  - Examples
    - Added logo example (cross-platform)
    - Added @ref ISR_VECTOR example of a raw ISR vector with no dispatcher for GB/CGB
    - Changed sgb_border example to use png2asset for graphics
    - Changed use of set_interrupts() in examples so it's outside critical sections (since it disables/enables interrupts)
    - Changed cross-platform auto-banks example to use .h header files
    - Changed SGB border example to also work with SGB on PAL SNES
  - Docs
    - Added new section: Migrating From Pre-GBDK-2020 Tutorials


## GBDK-2020 4.0.5
  2021/09
  - Includes SDCC version 12539 with GBDK-2020 patches for Z80
  - Known Issues
    - SDCC: `z80instructionSize() failed to parse line node, assuming 999 bytes`
      - This is a known issue with the SDCC Peephole Optimizer parsing and can be ignored.
    - `-bo<n>` and `-ba<n>` are not supported by the Windows build of @ref sdcc
    - On macOS the cross platform `banks` example has problems parsing the filename based ROM and RAM bank assignments into `-bo<n>` and `-ba<n>`
  - Added support for new consoles. See @ref docs_supported_consoles
    - Analogue Pocket (`ap`)
    - Sega Master System (`sms`) and Game Gear (`gg`)
  - Library
    - Fixed error when calling get_bkg_tile_xy: `?ASlink-Warning-Undefined Global '.set_tile_xy' referenced by module `
?ASlink-Warning-Byte PCR relocation error for symbol  .set_tile_xy
    - Variables in static storage are now initialized to zero per C standard (but remaining WRAM is not cleared)
    - Added many new register flag constants and names. For example:
      - @ref rLCDC is a new alias for @ref LCDC_REG
      - @ref LCDCF_WINON, @ref LCDCF_WINOFF, @ref LCDCF_B_WINON
    - Added @ref BANK(), @ref BANKREF(), @ref BANKREF_EXTERN()
    - Added @ref INCBIN(), @ref BANK(), @ref INCBIN_SIZE(), @ref INCBIN_EXTERN()
    - Added generic @ref SWITCH_ROM() and @ref SWITCH_RAM()
    - Added @ref BGB_printf() and updated emulator debug output.
    - Added @ref set_native_tile_data(), @ref set_tile_map(), @ref set_1bpp_colors, @ref set_bkg_1bpp_data, @ref set_sprite_1bpp_data, @ref set_2bpp_palette, @ref set_bkg_2bpp_data, @ref set_sprite_2bpp_data, @ref set_tile_2bpp_data (sms/gg only), @ref set_bkg_4bpp_data (sms/gg only), @ref set_sprite_4bpp_data (sms/gg only)
    - Added RLE decompression support: @ref rle_init(), @ref  rle_decompress(),
    - Changed @ref itoa(), @ref uitoa(), @ref ltoa(), @ref ultoa() to now require a radix value (base) argument to be passed. On the Game Boy and Analogue Pocket the parameter is required but not utilized.
  - Examples
    - Added cross-platform examples (build for multiple consoles: gb, ap, sms, gg)
    - Added sms, gg, pocket(ap) examples
    - Added incbin example
    - Added simple physics sub-pixel / fixed point math example
    - Added rle decompression example
    - Changed windows make.bat files to compile.bat
    - Bug fixes and updates for existing examples   
  - Toolchain / Utilities
    - @ref utility_png2asset "png2asset"
      - @ref utility_png2asset "png2asset" is the new name for the `png2mtspr` utility
      - Added collision rectangle width and height (`-pw`, `-ph`)
      - Added option to use the palette from the source png (`-keep_palette_order`)
      - Added option to disable tile flip (`-noflip`)
      - Added export as map: tileset + bg (`-map`)
      - Added option to use CGB BG Map attributes (`-use_map_attributes`)
      - Added option to group the exported info into structs (`-use_structs`)
    - @ref lcc "lcc"
      - Use `-m` to select target port and platform: "-m[port]:[plat]" ports:`gbz80,z80` plats:`ap,gb,sms,gg`
      - Changed default output format when not specified from `.ihx` to `.gb` (or other active rom extension)
      - Changed lcc to always use the linkerfile `-lkout=` option when calling bankpack
      - Fixed name generation crash when outfile lacks extension
    - @ref bankpack
      - Added linkerfile input and output: `-lkin=<file>`, `-lkout=<file>`
      - Added selector for platform specific behavior `plat=<plat>` (Default:gb, Avaialble:`gb,sms`). sms/gg targets prohibits packing `LIT_N` areas in the same banks as `CODE_N` areas
      - Added randomization for auto-banks (`-random`) for debugging and testing
    - @ref utility_gbcompress
      - Added C source array format output (--cout) (optional variable name argument --varname=)
      - Added C source array format input (--cin) (experimental)
      - Added block style rle compression and decompression mode: `--alg=rle`
      - Fixed comrpession errors when input size was larger than 64k
  - Docs
    - Added @ref docs_supported_consoles section
    - Various doc updates and improvements
 

## GBDK-2020 4.0.4
  2021/06
  - Library
    - Support SDCC INITIALIZER area (SDCC ~12207+)
    - Added @ref get_vram_byte() / @ref get_win_tile_xy() / @ref get_bkg_tile_xy()
    - Added @ref set_tile_data()
    - Fixed SGB detection
    - Fixed broken @ref get_tiles() / @ref set_tiles()
    - Fixed broken token handling in @ref gb_decompress_sprite_data() / @ref gb_decompress_bkg_data() / @ref gb_decompress_win_data()
    - Changed all headers to use standard `stdint.h` types (ex: `uint8_t` instead of `UINT8`/`UBYTE`)
    - Made sample.h, cgb.h and sgb.h independent from gb.h
  - Examples
    - Added project using a .lk linkerfile
    - Changed all examples to use standard stdint.h types
    - Moved banks_farptr and banks_new examples to "broken" due to SDCC changes
  - Toolchain / Utilities
    - png2mtspr
      - Added option to change default value for sprite property/attributes in (allows CGB palette, BG/WIN priority, etc).
      - Improved: Turn off suppression of "blank" metasprite frames (composed of entirely transparent sprites)
      - Fixed endless loop for png files taller than 255 pixels
    - bankpack
      - Fixed -yt mbc specifier to also accept Decimal
      - Improved: bank ID can be used in same file it is declared. Requires SDCC 12238+ with `-n` option to defer symbol resolution to link time.
    - gbcompress
      - Added C source input (experimental) and output
      - Added size `#defines`
    - lcc
      - Added `-no-libs` and `-no-crt` options
      - Added support for .lk linker files (useful when number of files on lcc command line exceeds max size on windows)
      - Added support for converting .ihx to .gb
      - Added rewrite .o files -> .rel for linking when called with `-autobank` and `-Wb-ext=.rel`
      - Workaround @ref makebin `-Wl-yp` formatting segfault
  - Docs
    - Improved utility_png2mtspr documentation
    - Various doc updates and improvements


## GBDK-2020 4.0.3
  2021/03
  - Library
    - Added @ref set_vram_byte()
    - Added @ref set_bkg_tile_xy() / @ref set_win_tile_xy()
    - Added @ref get_bkg_xy_addr() / @ref get_win_xy_addr()
    - Added @ref set_bkg_submap() / @ref set_win_submap()
    - Added metasprite api support
    - Added gb_decompress support
    - Added @ref calloc / @ref malloc / @ref realloc / @ref free and generic @ref memmove
    - Improved @ref printf(): ignore %0 padding and %1-9 width specifier instead of not printing, support upper case %X
    - Fixed @ref line(): handle drawing when x1 is less than x2
  - Examples
    - Added large_map: showing how to use @ref set_bkg_submap()
    - Added scroller: showing use of @ref get_bkg_xy_addr(), @ref set_bkg_tile_xy() and @ref set_vram_byte
    - Added gbdecompress: de-compressing tile data into vram
    - Added metasprites: show creating a large sprite with the new metasprite api
    - Added template projects
    - Fixed build issue with banks_autobank example
    - Improved sgb_border
  - Toolchain / Utilities
    - Added @ref utility_gbcompress utility
    - Added utility_png2mtspr metasprite utility
  - Docs
    - Added extensive documentation (some of which is imported and updated from the old gbdk docs)
    - Added PDF version of docs


## GBDK-2020 4.0.2
  2021/01/17
  - Includes SDCC snapshot build version 12016 (has a fix for duplicate debug symbols generated from inlined header functions which GBDK 4.0+ uses)
  - Updated documentation
  - Library was improved
      - Linking with stdio.h does not require that much ROM now
      - Default font is changed to the smaller one (102 characters), that leaves space for user tiles
      - Fixed broken support for multiplying longs
      - memset/memcpy minor enhancements
      - safer copy-to-VRAM functions
      - loading of 1bit data fixed, also now it is possible to specify pixel color
      - Improved code generation for the GBDK Library with SDCC switch on by default: `--max-allocs-per-node 50000`
      - fixed wrong parameter offsets in hiramcpy() (broken ram_function example)
      - Multiple minor improvements
  - New bankpack feature, allows automatic bank allocation for data and code, see banks_autobank example, feature is in beta state, use with care
  - Lcc improvements
    - Fixed option to specify alternate base addresses for shadow_OAM, etc
  - Examples: Added bgb debug example


## GBDK-2020 4.0.1
  2020/11/14
  - Updated API documentation
  - IHX is checked for correctness before the makebin stage. That allows to warn about overwriting the same ROM addresses (SDCC toolchain does not check this anymore).
  - Library was improved
    - set_*_tiles() now wrap maps around horizontal and vertical boundaries correctly
    - new fill_*_rect() functions to clear rectangle areas
    - runtime initialization code now does not initialize whole WRAM with zeros anymore, that allows BGB to raise exceptions when code tries to read WRAM that was not written before.
    - enhanced SGB support
     - joypad_init() / joypad_ex() support for multiple joypads
     - SGB border example
    - _current_bank variable is updated when using bank switching macros
    - Reorganized examples: each example is in separate folder now, that simplifies understanding.
    - Lcc improvements
      - Fix -S flag
      - Fix default stack location from 0xDEFF to 0xE000 (end of WRAM1)
      - Fix cleanup of .adb files with -Wf--debug flag
      - Fix output not working if target is -o some_filename.ihx


## GBDK-2020 4.0
  2020/10/01
  - GBDK now requires SDCC 4.0.3 or higher, that has fully working toolchain. Old link-gbz80 linker is not used anymore, sdldgb and makebin are used to link objects and produce binary roms; maccer tool is no longer needed either
    - SDCC 4.0.3 has much better code generator which produces smaller and faster code. Code is twice faster
    - SOURCE LEVEL DEBUGGING is possible now! Native toolchain produces *.CDB files that contain detailed debug info. Look for EMULICIOUS extension for vs.code. It supports breakpoints, watches, inspection of local variables, and more!
    - SDCC 4.0.4 has fixed RGBDS support; library is not updated to support that in full yet, but it is possible to assemble and link code emitted by SDCC with RGDBS
    - New banked trampolines are used, they are faster and smaller
    - New (old) initialization for non-constant arrays do NOT require 5 times larger rom space than initialized array itself, SDCC even tries to compress the data
  - Library was improved
    - itoa/ltoa functions were rewritten, div/mod is not required now which is about 10 times faster
    - sprite functions are inline now, which is faster up to 12 times and produces the same or smaller code; .OAM symbol is renamed into _shadow_OAM that allows accessing shadow OAM directly from C code
    - interrupt handling was revised, it is now possible to make dedicated ISR's, that is important for time-sensitive handlers such as HBlank.
    - printf/sprintf were rewritten and splitted, print functions are twice faster now and also requre less rom space if you use sprintf() only, say, in bgb_emu.h
    - crash_handler.h - crash handler that allows to detect problems with ROMs after they are being released (adapted handler, originally written by ISSOtm)
    - improved and fixed string.h
    - many other improvements and fixes - thanks to all contributors!
  - Revised examples
  - Improved linux support
  - Lcc has been updated
    - it works with the latest version of sdcc
    - quoted paths with spaces are working now


## GBDK-2020 3.2
  2020/06/05
  - Fixed OAM initialization that was causing a bad access to VRAM
  - Interrupt handlers now wait for lcd controller mode 0 or 1 by default to prevent access to inaccessible VRAM in several functions (like set_bkg_tiles)
  - Several optimizations here and there


## GBDK-2020 3.1.1
  2020/05/17
  - Fixed issues with libgcc_s_dw2-1.dll


## GBDK-2020 3.1
  2020/05/16
  - Banked functions are working! The patcher is fully integrated in link-gbz80, no extra tools are needed. It is based on Toxa's work
    - Check this post for more info
    - Check the examples/gb/banked code for basic usage
  - Behavior formerly enabled by USE_SFR_FOR_REG is on by default now (no need to specify it, it isn't a tested `#ifdef` anymore). check here why:
    https://gbdev.gg8.se/forums/viewtopic.php?id=697
  - Fixed examples that were not compiling in the previous version and some improvements in a few of them. Removed all warnings caused by changing to the new SDCC
  - Fixed bug in lcc that was causing some files in the temp folder not being deleted
  - Removed as-gbz80 (the lib is now compiled with sdasgb thanks to this workaround)
    https://github.com/gbdk-2020/gbdk-2020/commit/d2caafa4a66eb08998a14b258cb66af041a0e5c8
  - Profile support with bgb emulator
    - Basic support including <gb/bgb_emu.h> and using the macros BGB_PROFILE_BEGIN and BGB_PROFILE_END. More info in this post
      https://gbdev.gg8.se/forums/viewtopic.php?id=703
    - For full profiling check this repo and this post
      https://github.com/untoxa/bgb_profiling_toolkit/blob/master/readme.md
      https://gbdev.gg8.se/forums/viewtopic.php?id=710


## GBDK-2020 3.0.1
  2020/04/12
  - Updated SDCC to v.4.0
  - Updated LCC to work with the new compiler


## GBDK-2020 3.0
  2020/04/12
  - Initial GBDK-2020 release 
  Updated SDCC to v4.0
  The new linker is not working so the old version is still there
  There is an issue with sdagb compiling drawing.s (the JP in
  line 32 after ".org .MODE_TABLE+4*.G_MODE" it's writing more
  than 4 bytes invading some addresses required by input.s:41)
  Because of this, all .s files in libc have been assembled
  with the old as-gbz80 and that's why it is still included


# Historical GBDK Release Notes

## GBDK 2.96
  17 April, 2000 
  Many changes. 
  - Code generated is now much more reliable and
  passes all of sdcc's regression suite.
  - Added support for large sets of local variables (>127 bytes).
  - Added full 32 bit long  support.
  - Still no floating pt support.

## GBDK 2.95-3
  19th August, 2000 

  * Stopped lcc with sdcc from leaking .cdb files all across
    /tmp.
  * Optimised < and > for 16 bit varibles.
  * Added a new lexer to sdcc.  Compiling files with large
    initalised arrays takes 31% of the time (well, at least
    samptest.c does :) 

  This is an experimental release for those who feel keen. The main change is a new lexer (the first part in the compilation process which recognises words and symbols like '!=' and 'char' and turns them into a token number) which speeds up compilation of large initialised arrays like tile data by a factor of three. Please report any bugs that show up - this is a big change.

  I have also included a 'minimal' release for win32 users which omits the documentation, library sources, and examples. If this is useful I will keep doing it.

## GBDK 2.95-2
  5th August, 2000 
  Just a small update. From the README:

  * Added model switching support
     --model-medium uses near (16 bit) pointers for data, and
     banked calls for anything not declared as 'nonbanked'
     --model-small uses near (16 bit) pointers for data and
     calls.  Nothing uses banked calls.  'nonbanked' functions
     are still placed in HOME.  Libraries are under lib/medium
     and lib/small.
  * Added the gbdk version to 'sdcc --version'
  * Changed the ways globals are exported, reducing the amount
    of extra junk linked in.
  * Turned on the optimisations in flex.  Large constant arrays
    like tile data should compile a bit faster.

## GBDK 2.95
  22nd July, 2000
  * Fixed 'a << c' for c = [9..15]
  * no$gmb doesn't support labels of > 32 chars.  The linker now
  trims all labels to 31 chars long.
  * Fixed wait_vbl for the case where you miss a vbl
  * Fixed + and - for any type where sizeof == 2 and one of the
  terms was on the stack.  This includes pointers and ints. Fixes
  the text output bug in the examples.  Should be faster now as
  well.  Note that + and - for longs is still broken.
  * Fixed the missing */ in gb.h
  * Added basic far function support.  Currently only works for isas
  and rgbasm.  See examples/gb/far/*
  * bc is now only pushed if the function uses it.  i.e. something
  like:
      int silly(int i)
      {
      return i;
      }  
  will not have the push bc; pop bc around it.
  * Better rgbasm support.  Basically:
         o Use "sdcc -mgbz80 --asm=rgbds file.c" for each file.c
         o Use "sdcc -mgbz80 --asm=rgbds crt0.o gbz80.lib gb.lib file1.o file2.o..."
 
  to link everything together.  The .lib files are generated using
  astorgb.pl and sdcc to turn the gbdk libraries into something
  rgbds compatible.
  The libraries are _not_ fully tested.  Trust nothing.  But give it
  a go :)
  * Ran a spell checker across the README and ChangeLog 

  This is a recommended upgrade. Some of the big features are:

  Decent rgbds support. All the libraries and most of the examples can now compile with rgbds as the assembler.
  Banked function support. It is now easier to break the 32k barrier from within C. Functions can live in and be called transparently from any bank. Only works with rgbds
  Fixed some decent bugs with RSH, LSH, and a nasty bug with + and - for int's and pointers.
  Various optimisations in the code generator.

  7th July, 2000 
  Information on float and long support. Someone asked about the state of float/long support recently. Heres my reply:

  long support is partly there, as is float support. The compiler will correctly recognise the long and float keywords, and will generate the code for most basic ops (+, -, &, | etc) for longs correctly and will generate the function calls for floats and hard long operations (*, /, %) correctly. However it wont generate float constants in the correct format, nor will it 'return' a long or float - gbdk doesn't yet support returning types of 4 bytes. Unfortunately its not going to make it into 2.95 as there's too much else to do, but I should be able to complete long support for 2.96

## GBDK 2.94
  7th May, 2000 
  Many fixes - see the README for more.

  7th May - Library documentation up. A good size part of the libraries that go with gbdk have been documented - follow the HTML link above to have a look. Thanks to quang for a good chunk of the gb.h documentation. Please report any errors :)
   * Fixed
        \#define BLAH  7  // Unterminated ' error
      in sdcpp
    * Fixed
        SCY_REG += 2, SCY_REG -= 5
      (add and subtract in indirect space) as they were both quite broken.
    * externs and static's now work as expected.
    * You can now specify which bank code should be put into using a
      \#pragma e.g:
        \#pragma bank=HOME
    Under rgbds and asxxxx putting code in the HOME bank will force
    the code into bank 0 - useful for library functions.  The most
    recent \#pragma bank= will be the one used for the whole file.
    * Fixed an interesting bug in the caching of lit addresses
    * Added support for accessing high registers directly using the
    'sfr' directive.  See libc/gb/sfr.s and gb/hardware.h for an
    example.  It should be possible with a bit of work to make high
    ram directly usable by the compiler; at the moment it is
    experimental.  You can test sfr's by enabling USE_SFR_FOR_REG=1
    * Added remove_VBL etc functions.
    * Documented the libs - see the gbdk-doc tarball distributed
    seperatly.
    * Two dimensional arrays seem to be broken.

## GBDK 2.93
  6th April, 2000 
  From the README

  * Added multi-bank support into the compiler - The old -Wf-boxx
  and -Wf-baxx options now work
  * Has preliminary support for generating rgbds and ISAS compatible
  assembler.  Try -W--asm=rgbds or -W--asm=isas.  The ISAS code is
  untested as I dont have access to the real assembler.
  * RSH is fixed
  * AND is fixed
  * The missing parts of 2.1.0's libs are there.  Note: They are
  untested.
  * The dscan demo now fully works (with a hack :)
  * There is a bug with cached computed values which are later used
  as pointers.  When the value is first used as a BYTE arg, then
  later as a pointer the pointer fails as the high byte was never
  computed and is now missing.  A temporary fix is to declare
  something appropriate as 'volatile' to stop the value being
  cached.  See dscan.c/bombs() for an example.

## GBDK 2.92-2 for win32
  26th March, 2000 
  This is a maintenance release for win32 which fixes some of the niggly install problems, especially:
  * win32 only.  Takes care of some of the install bugs, including:
     - Now auto detects where it is installed.
      This can be overridden using set GBDKDIR=...
     - Problems with the installer (now uses WinZip)
     - Problems with the temp directory
      Now scans TMP, TEMP, TMPDIR and finally c: tmp
     - cygwin1.dll and 'make' are no longer required
      gbdk is now built using mingw32 which is win32 native
      make.bat is automagically generated from the Makefile
     - I've reverted to using WORD for signed 16 bit etc.
      GBDK_2_COMPAT is no longer required.

  WORDS are now back to signed. GBDK_2_COMPAT is no longer needed.
  Temporary files are created in TMP, TEMP, or TMPDIR instead of c: tmp
  The installer is no more as it's not needed. There is a WinZip wrapped version for those with the extra bandwidth :).
  gbdk autodetects where it is installed - no more environment variables.
  cygwin1.dll and make are no longer required - gbdk is now compiled with mingw32.

  See the ChangeLog section in the README for more information.

  21st March, 2000 
  Problems with the installer. It seems that the demo of InstallVISE has an unreasonably short time limit. I had planed to use the demo until the license key came through, but there's no sign of the key yet and the 3 day evaluation is up. If anyone knows of a free Windows installer with the ability to modify environment variables, please contact me. I hear that temporarily setting you clock back to the 15th works...

  18th March, 2000 
  libc5 version available / "Error creating temp file" Thanks to Rodrigo Couto there is now a Linux/libc5 version of gbdk3-2.92 available - follow the download link above. At least it will be there when the main sourceforge site comes back up... Also some people have reported a bug where the compiler reports '*** Error creating temp file'. Try typing "mkdir c: tmp" from a DOS prompt and see if that helps.

## GBDK 2.92
  8th March, 2000 
  Better than 2.91 :). Can now be installed anywhere. All the demos work. See the README for more.
  * All the examples now work (with a little bit of patching :)
    * Fixed problem with registers being cached instead of being
    marked volatile.
    * More register packing - should be a bit faster.
    * You can now install somewhere except c: gbdk | /usr/lib/gbdk
    * Arrays initialised with constant addresses a'la galaxy.c now
    work.
    * Fixed minor bug with 104$: labels in as.
    * Up to 167d/s... 

## GBDK 2.91
  27th Feb, 2000 
 Better than 2.90 and includes Linux, win32 and a source tar ball. Some notes:

  Read the README first
  Linux users need libgc-4 or above. Debian users try apt-get install libgc5.
  All the types have changed. Again, please read the README first.
  I prefer release early, release often. The idea is to get the bugs out there so that they can be squashed quickly.
  I've split up the libs so that they can be used on other platforms and so that the libs can be updated without updating the compiler. One side effect is that gb specific files have been shifted into their own directory i.e. gb.h is now gb/gb.h.

  23rd Feb, 2000 
  First release of gbdk/sdcc.
  This is an early release - the only binary is for Linux and the source is only available through cvs. If your interested in the source, have a look at the cvs repository gbdk-support first, which will download all the rest of the code. Alternatively, look at gbdk-support and gbdk-lib at cvs.gbdk.sourceforge.net and sdcc at cvs.sdcc.sourceforge.net. I will be working on binaries for Win32 and a source tar ball soon. Please report any bugs through the bugs link above.

  31st Jan, 2000 
  Added Dermot's far pointer spec. It's mainly here for comment. If sdcc is ported to the Gameboy then I will be looking for some way to do far calls.

  8th Jan, 2000 
  Moved over to sourceforge.net. Thanks must go to David Pfeffer for gbdk's previous resting place, www.gbdev.org. The transition is not complete, but cvs and web have been shifted. Note that the cvs download instructions are stale - you should now look to cvs.gbdk.sourceforge.net.
  I am currently working on porting sdcc over to the Z80. David Nathan is looking at porting it to the GB.

  6th Jan, 2000 
  Icehawk wrote "I did write some rumble pack routines. Just make sure to remind people to add -Wl-yt0x1C or -Wl-yt0x1D or -Wl-yt0x1E depending on sram and battery usage. Find the routines on my site (as usual). =)"

  18th Oct, 1999 
  Bug tracking / FAQ up. Try the link on the left to report any bugs with GBDK. It's also the first place to look if your having problems.

## GBDK 2.1.5
  17th Oct, 1999 

  The compiler is the same, but some of the libraries have been improved. memset() and memcpy() are much faster, malloc() is fixed, and a high speed fixed block alternative malloc() was added.



## GBDK 2.0b11 (DOS binary only) - 24 November 1997

* Fixed another bug in code generation, that could happen when performing logical operations on 1-byte variables.

## GBDK 2.0b10 (DOS binary only) - 6 November 1997

* Fixed a nasty bug in code generation, that could happen when performing arithmetic operations on 1-byte variables.
* Changed the name of same files of the gb-dtmf example so that it compiles on DOS.

## GBDK 2.0b9 (DOS binary only)

* Several bug fixes in the compiler and in the libraries.

## GBDK 2.0b8 (DOS binary only)

* Limited all file names to 8 characters to solve problems on DOS.
* Added communication routines that enable to send data through the link port of the GameBoy. Unfortunately, these routines do not always work; so use them with care until the next GBDK release.
* Added the comm.c example which illustrates how to use communication routines.
* It is possible to specify the name of the program (to be written in the image header) at link time using the -Wl-yn="XXX" flag (where X is the name of the program, which can contain up to 16 characters in quotes, including spaces; on Unix, depending on your shell, you must add backslashes before quotes and spaces like in -Wl-yn=\"My\ Game\").
* Several bug fixes in the compiler.

## GBDK 2.0b7 (DOS binary only)

* GBDK now uses a pre-release of lcc 4.1 (DOS binary only), that fixes a couple of problems in code generation.
* A couple of important points have been documented in the GBDK Programming Guidelines and Known Problems sections.
* Several improvements and optimizations to the code generator.

## GBDK 2.0b6

* Added a peephole optimizer (with few rules at the moment).
* Changed the name of the hardware registers to match the "official" names.
* Added support for copying complete functions to RAM or HIRAM (memcpy() and hiramcpy() functions). The compiler now automatically generates two symbol for the start and the end of each function, named start_X and end_X (where X is the name of the function). This enables to calculate the length of a function when copying it to RAM.
* Added the ram_fn.c example which illustrates how to copy functions to RAM and HIRAM.
* Added support for installing IRQ handlers.
* Added the irq.c example which illustrates how to install IRQ handlers.
* Added RAM banks support (switch_ram_bank() function). The switch_bank() function has been renamed to switch_rom_bank(). The banks.c example has been updated. The flags for generating multiple bank images have been modified.
* It is possible to set the sprite ram location at link time using the -Wl-g.OAM=# flag (where # is the address of the sprite ram). The sprite ram address must begin at an address multiple of 0x100, and is 0xA0 bytes long.

## GBDK 2.0b5

* New documentation (not finished yet).
* Fixed a bug that could generate wrong code in switch statements.
* Fixed a bug in int comparison.
* Added a DTMF program written by Osamu Ohashi.
* Added a game (Deep Scan) written by a friend of Osamu.
* Modified the delay() function so that it takes a long parameter. It can be used to wait between 1 and 65536 milliseconds (0 = 65536). The pause() function has been removed.

## GBDK 2.0b4

* Fixed a bug that could generate wrong code when using hexadecimal constants.
* A new example (galaxy.c) has been added. It is the C version of the space.s example. sprite.c has been removed.
* Most of the libraries have been split into small modules for reducing final code size.

## GBDK 2.0b3

* GBDK can generate multiple-banks images, i.e. images greater than 32kB (see the banks example).
* It is possible to set the stack pointer at link time using the -Wl-g.STACK=# flag (where # is the address of the stack pointer).
    Several functions (e.g. show_bkg()) have been changed into macros (e.g. SHOW_BKG).
    The delay() function waits exactly 1 millisecond, and the pause() waits 256 milliseconds.
    Linking with the standard libraries is no more required. The lib/gb.lib (lib\gb.lib on DOS) text file contains a list of modules in which to look for undefined symbols. The linker will parse this file, and link your code with the required modules only. The stdio library has been split in several object files, and only necessary modules will be added to your code, thus reducing its size.
    The GBDK distribution can be located anywhere in your system if you use the -Wo-lccdir=GBDK-DIR flag when invoking lcc.
    Bug fixes.

## GBDK 2.0b2

* Lots of bug fixes.
* GBDK has to be in the `\GBDK-2.0` directory on DOS machines.

## GBDK 2.0b1

* The code generator has been completely rewritten with the new version of lcc. It produces much smaller and more efficient code. The size of the code is generally between 20 and 50% smaller. A number of small optimizations are still to be done.
* The size of basic types has been changed:
        * An int is 8 bits.
        * A long is 16 bits.
* This change was required for the code generator to produce better code, because the Z80 is actually an 8-bit processor.
* The linker generates the complement checksum correctly now.
* The libraries and example programs have been modified for the new code generator.

## GBDK 1.1

* Removed Xloadimage from the GBDK distribution. It is now available as a separate archive.
* A compiled DOS version is now available (cross-compiled on my Sun Workstation!).
* The libraries and the example programs have been improved.
* The make script has been improved. Compiling on UNIX should be easier.
* Many bugfixes.

## GBDK 1.0-1 1996

