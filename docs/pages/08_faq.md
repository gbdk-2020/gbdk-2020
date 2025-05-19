@page docs_faq Frequently Asked Questions (FAQ)

@anchor toolchain_faq 

# General
  - How can sound effects be made?
    - The simplest way is to use the Game Boy sound hardware directly. See the @ref examples_sound_sample "Sound Example" for a way to test out sounds on the hardware.
    - Further discussion on using the Sound Example rom can be found in the ZGB wiki. Note that some example code there is ZGB specific and not part of the base GBDK API: https://github.com/Zal0/ZGB/wiki/Sounds <!-- -->

# Licensing
  - What license information is required when distributing the compiled ROM (binary) of my game or program?
    - There is no requirement to include or credit any of the GBDK-2020 licenses or authors, although credit of GBDK-2020 is appreciated.
    - This is different and separate from redistributing the GBDK-2020 dev environment itself (or the GBDK-2020 sources) which does require the licenses. <!-- -->

# Graphics and Resources
  - How do I use a tile map when its tiles don't start at index zero?
    - The two main options are:
      - Use @ref set_bkg_based_tiles(), @ref set_bkg_based_submap(), @ref set_win_based_tiles(), @ref set_win_based_submap() and provide a tile origin offset.
      - Use @ref utility_png2asset with `-tile_origin` to create a map with the tile index offsets built in.

  - Is it normal for sprites to disappear when they reach the left border of the screen? (NES/SMS/MSX)
    - You can hide the leftmost column using @ref HIDE_LEFT_COLUMN to work around this.
    - The behavior is due to NES/SMS/MSX having 8-bit Sprite x coordinates while the screen width is also 256 pixels. GB/GG don't have this problem since their screen is smaller and the x-coordinates are larger than the visible screen.
      <!-- -->  

# ROM Header Settings
  - How do I set the ROM's title?
    - Use the @ref makebin `-yn` flag. For example with @ref lcc `-Wm-yn"MYTITLE"` or with @ref makebin directly `-yn "MYTITLE"`. The maximum length is up to 15 characters, but may be shorter.
    - See "0134-0143 - Title" in @ref Pandocs for more details. <!-- -->  

  @anchor faq_gb_type_header_setting
  - How do I set SGB, Color only and Color compatibility in the ROM header?
    - Use the following @ref makebin flags. Prefix them with `-Wm` if using @ref lcc.
      - `-yc` : GameBoy Color compatible
      - `-yC` : GameBoy Color only
      - `-ys` : Super GameBoy compatible <!-- -->  

  - How do I set the ROM @ref MBC type, and what MBC values are available to use with the `-yt` @ref makebin flag?
    - See @ref setting_mbc_and_rom_ram_banks <!-- -->  

# Editors
  - Why is VSCode flagging some GBDK types or functions as unidentified or giving warnings about them?
    - See @ref code_editors_hinting
    - GBDK platform constants can be declared so that header files are parsed more completely in VSCode.

# Errors and Warnings
  @anchor faq_gbz80_sm83_old_port_name_error
  - What does the error `old "gbz80" SDCC PORT name specified (in "-mgbz80:gb"). Use "sm83" instead. You must update your build settings.` mean?
    - The `PORT` name for the Game Boy and related clones changed from `gbz80` to `sm83` in the SDCC version used in GBDK-2020 4.1.0 and later. You must change your Makefile, Build settings, etc to use the new name. Additional details in the @ref console_port_plat_settings "Console Port and Platform Settings" section.  <!-- -->  

  @anchor faq_linker_conflicting_sdcc_options
  - What does the warning `?ASlink-Warning-Conflicting sdcc options: "-msm83" in module "_____" and
   "-mgbz80" in module "_____".` mean?
    - One object file was compiled with the PORT setting as `gbz80` (meaning a version of SDCC / GBDK-2020 __OLDER than GBDK-2020 4.1.0__).
    - The other had the PORT setting as `sm83` (meaning __GBDK-2020 4.1.0 or LATER__).
    - You must rebuild the object files using `sm83` with GBDK-2020 4.1.0 or later so that the linker is able to use them with the other object files. Additional details in the @ref console_port_plat_settings "Console Port and Platform Settings" section.  <!-- -->  

  @anchor faq_linker_undefined_global
  - What does the warning `?ASlink-Warning-Undefined Global ...` mean?
    - The linker is unable to find a variable or function that is referenced by some part of the program. This usually means a required source file is not being passed to the linker stage. Make sure all of your project source files are getting compiled and passed to the linker. <!-- -->  

  @anchor faq_sdcc_peephole_instruction_error
  - What does `z80instructionSize() failed to parse line node, assuming 999 bytes` mean?
    - This is a known issue with SDCC Peephole Optimizer parsing and can be ignored. A bug report has been filed for it. <!-- -->  

  @anchor faq_bank_overflow_errors
  - What do these kinds of warnings / errors mean?
    `WARNING: possibly wrote twice at addr 4000 (93->3E)`
    `Warning: Write from one bank spans into the next. 7ff7 -> 8016 (bank 1 -> 2)`
    - You may have a overflow in one of your ROM banks. If there is more data allocated to a bank than it can hold it then will spill over into the next bank.

      A common problem is when there is too much data in ROM0 (the lower 16K unbanked region) and it spills over into ROM1 (the first upper 16K banked region). Make sure ROM0 has 16K or less in it.

      The warnings are generated by @ref ihxcheck during conversion of an .ihx file into a ROM file.

      See the section @ref docs_rombanking_mbcs for more details about how banks work and what their size is. You may want to use a tool such as @ref romusage to calculate the amount of free and used space. <!-- -->  

  @anchor faq_error_mbc_size
  - What do these errors mean?
    `error: size of the buffer is too small`
    `error: ROM is too large for number of banks specified`
    - Your program is using more banks than you have configured in the toolchain.
      Either the MBC type was not set, or the number of banks or MBC type should be changed to provide more banks. 

      See the section @ref setting_mbc_and_rom_ram_banks for more details. <!-- -->  

  @anchor faq_sdcc_function_declarator_missing_prototype
  - What does `warning 283: function declarator with no prototype` mean?
    - Function forward declarations and definitions which have no arguments should be changed from `func()` to `func(void)`.
    - In C `func()` and `func(void)` do not mean the same thing. `()` means any number of parameters, `(void)` means no parameters. For example if a function with no arguments is declared with `()` then there may not be an error or warning when mistakenly trying to pass arguments to it.   <!-- -->  

  @anchor faq_sdcc_unreachable_code
  - What do these warnings mean?
    `warning 126: unreachable code`
    `warning 110: conditional flow changed by optimizer: so said EVELYN the modified DOG`
    - The compiler is indicating that some branches of the code will never get executed or will have no effect, and so have been removed during optimization. There may be a logical error, unnecessary logic or unused variables.
    - If this is due to a variable which gets modified outside of normal execution (such as during an interrupt) then the `volatile` keyword can be added to it's declaration. This removes some optimizations and tells the compiler the variable's value may change unexpectedly and cannot be predicted only based on the surrounding code. <!-- -->  

  @anchor faq_sdcc_overflow_implicit_constant_conversion
  - What does this warning mean?
    `WARNING: overflow in implicit constant conversion`
    - See @ref docs_constant_signedness "Constants, Signed-ness and Overflows"

  @anchor faq_macos_security_warning
  - On macOS, what does `... developer cannot be verified, macOS cannot verify that this app is free from malware` mean?
    - It does not mean that GBDK is malware. It just means the GBDK toolchain binaries are not signed by Apple, so it won't run them without an additional step.

    - For the workaround, see the @ref macos_unsigned_security_workaround "macOS unsigned binary workaround" for details. <!-- -->  

  @anchor faq_sdcc_z80_failed_to_parse
  - What do the following kinds of warnings / errors mean?
    `info 218: z80instructionSize() failed to parse line node, assuming 999 bytes`
    - This is a known issue with SDCC, it should not cause actual problems and you can ignore the warning. <!-- -->  


# Debugging / Compiling / Toolchain
  - What flags should be enabled for debugging?
    - You can use the @ref lcc_debug "lcc debug flag" `-debug`to turn on debug output. It covers most uses and removes the need to specify multiple flags such as `-Wa-l -Wl-m -Wl-j`. Also see @ref tools_debug. <!-- -->  

  - Is it possible to generate a debug symbol file (`.sym`) compatible with an emulator?
    - Yes, turn on `.noi` output (LCC argument: `-Wl-j` or `-debug` and then use `-Wm-yS` with LCC (or `-yS` with makebin directly).
    - Also see additional information about using @ref tools_debug "debugging tools".<!-- -->  

  - How do I move the start of the `DATA` section and the `Shadow OAM` location?
    - The default locations are: `_shadow_OAM=0xC000` and 240 bytes after it `_DATA=0xC0A0`
    - So, for example, if you wanted to move them both to start 256(0x100) bytes later, use these command line arguments for LCC:
      - To change the Shadow OAM address: `-Wl-g_shadow_OAM=0xC100`
      - To change the DATA address (again, 240 bytes after the Shadow OAM): `-Wl-b_DATA=0xc1a0` <!-- -->  

  @anchor faq_sdcc_slow_compiling
  - Why is the compiler so slow, or why did it suddenly get much slower?
    - This may happen if you have large initialized arrays declared without the `const` keyword. It's important to use the const keyword for read-only data. See @ref const_gbtd_gbmb and @ref const_array_data
    - It can also happen if C source files are `#included` into other C source files, or if there is a very large source file.  <!-- -->  

  - Known issue: SDCC may fail on Windows when @ref windows_sdcc_non_c_drive_path_spaces "run from folder names with spaces on non-C drives".
    <!-- -->  

# API / Utilities
  - Is there a list of all functions in the API?
    - [Functions](globals_func.html)
    - [Variables](globals_vars.html) <!-- -->  

  - Can I use the `float` type to do floating point math?
    - There is no support for 'float' in GBDK-2020.
    - Instead consider some form of `fixed point` math (including the @ref fixed_point_type "fixed" type included in GBDK). <!-- -->  

  - Why are 8 bit numbers not printing correctly with printf()?
    - To correctly pass chars/uint8s for printing, they must be explicitly re-cast as such when calling the function. See @ref docs_chars_varargs for more details.  <!-- -->  

  - How can maps larger than 32x32 tiles be scrolled? & Why is the map wrapping around to the left side when setting a map wider than 32 tiles with set_bkg_data()?
    - The hardware Background map is 32 x 32 tiles. The screen viewport that can be scrolled around that map is 20 x 18 tiles. In order to scroll around within a much larger map, new tiles must be loaded at the edges of the screen viewport in the direction that it is being scrolled. @ref set_bkg_submap can be used to load those rows and columns of tiles from the desired sub-region of the large map.
    - See the "Large Map" example program and @ref set_bkg_submap().
    - Writes that exceed coordinate 31 of the Background tile map on the x or y axis will wrap around to the Left and Top edges. <!-- -->  

  - When using gbt_player with music in banks, how can the current bank be restored after calling gbt_update()? (since it changes the currently active bank without restoring it).
    - See @ref banking_current_bank "restoring the current bank" <!-- -->  

  - How can CGB palettes and other sprite properties be used with metasprites?
    - See @ref metasprite_and_sprite_properties "Metasprites and sprite properties" <!-- -->  

  - Weird things are happening to my sprite colors when I use png2asset and metasprites. What's going on and how does it work?
    - See @ref utility_png2asset for details of how the conversion process works.

