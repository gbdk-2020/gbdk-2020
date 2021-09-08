@page docs_releases GBDK Releases

The GBDK 2020 releases can be found on Github:
https://github.com/gbdk-2020/gbdk-2020/releases


# GBDK 2020 Release Notes

## GBDK 2020 4.0.5
  2021/09
  - Known Issues
    - The `bgb_debug` example has a compile error on macOS.
      - The compiler has a bug on macOS where long strings used with macros cause a crash. @ref bgb_emu.h can still be used with shorter strings.
  - Overall
    - Added support for new consoles
      - Analogue Pocket (`ap`)
      - Sega Master System (`sms`) and Game Gear (`gg`)
    - The windows make.bat files were renamed to compile.bat
  - Library
    - Variables in static storage are now initialized to zero per C standard (but remaining WRAM is not cleared)
    - Added new register flag constants and names. For example:
      - @ref rLCDC is a new alias for @ref LCDC_REG
      - @ref LCDCF_WINON, @ref LCDCF_WINOFF, @ref LCDCF_B_WINON
    - Added @ref BANK(), @ref BANKREF(), @ref BANKREF_EXTERN()
    - Added @ref INCBIN(), @ref BANK(), @ref INCBIN_SIZE(), @ref INCBIN_EXTERN()
    - Updated bgb debug output. Added @ref BGB_printf()
  - Examples
    - Added cross-platform examples (builds for multiple consoles: gb, ap, sms, gg)
    - Improved `bgb_debug` example
  - Toolchain / Utilities
    - @ref utility_png2asset "png2asset"
      - @ref utility_png2asset "png2asset" is the new name for the `png2mtspr` utility
      - Added collision rectangle width and height (`-pw`, `-ph`)
      - Added option to use the palette from the source png (`-keep_palette_order`)
      - Added option to disable tile flip (`-noflip`)
      - Added export as map: tileset + bg (`-map`)
      - Added option to use CGB BG Map attributes (`-use_map_attributes`)
      - Added option to group the exported info into structs (`-use_structs`)
    - @ref lcc
      - Use `-m` to select target port and platform: "-m[port]:[plat]" ports:`gbz80,z80` plats:`ap,gb,sms,gg`
      - Changed default output format when not specified from `.ihx` to `.gb` (or other active rom extension)
      - 
    - @ref bankpack
      - Added linkerfile input and output: `-lkin=<file>`, `-lkout=<file>`
      - Added selector for platform specific behavior `plat=<plat>` (Default:gb, Avaialble:`gb,sms`). sms/gg targets prohibits packing `LIT_N` areas in the same banks as `CODE_N` areas
      - Added randomization for auto-banks (`-random`) for debugging and testing

  - Docs
  

## GBDK 2020 4.0.4
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
      - Added C source input (expirimental) and output
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


## GBDK 2020 4.0.3
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
 

## GBDK 2020 4.0.2
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


## GBDK 2020 4.0.1
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


## GBDK 2020 4.0
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


## GBDK 2020 3.2
  2020/06/05
  - Fixed OAM initialization that was causing a bad access to VRAM
  - Interrupt handlers now wait for lcd controller mode 0 or 1 by default to prevent access to inaccessible VRAM in several functions (like set_bkg_tiles)
  - Several optimizations here and there


## GBDK 2020 3.1.1
  2020/05/17
  - Fixed issues with libgcc_s_dw2-1.dll


## GBDK 2020 3.1
  2020/05/16
  - Banked functions are working! The patcher is fully integrated in link-gbz80, no extra tools are needed. It is based on Toxa's work
    - Check this post for more info
    - Check the examples/gb/banked code for basic usage
  - USE_SFR_FOR_REG is the default now check here why
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


## GBDK 2020 3.0.1
  2020/04/12
  - Updated SDCC to v.4.0
  - Updated LCC to work with the new compiler


## GBDK 2020 3.0
  2020/04/12
  - Initial GBDK 2020 release  
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