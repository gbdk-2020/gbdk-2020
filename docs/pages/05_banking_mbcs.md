@page docs_rombanking_mbcs ROM/RAM Banking and MBCs



# ROM/RAM Banking and MBCs (Memory Bank Controllers)
The standard Game Boy cartridge with no MBC has a fixed 32K bytes of ROM. In order to make cartridges with larger ROM sizes (to store more code and graphics) MBCs can be used. They allow switching between multiple ROM banks that use the same memory region. Only one of the banks can be selected as active at a given time, while all the other banks are inactive (and so, inaccessible).


## Non-banked cartridges
Cartridges with no MBC controller are non-banked, they have 32K bytes of fixed ROM space and no switchable banks. For these cartridges the ROM space between `0000h and 7FFFh` can be treated as a single large bank of 32K bytes, or as two contiguous banks of 16K bytes in Bank 0 at `0000h - 3FFFh` and Bank 1 at `4000h to 7FFFh`.


## MBC Banked cartridges (Memory Bank Controllers)
@anchor MBC
@anchor MBCS
Cartridges with MBCs allow the the Game Boy to work with ROMS up to 8MB in size and with RAM up to 128kB. Each bank is 16K Bytes.
  - Bank 0 of the ROM is located in the region at `0000h - 3FFFh`. It is _usually_ fixed (non-banked) and cannot be switched out for another bank.
  - The higher region at `4000h to 7FFFh` is used for switching between different ROM banks.

See the @ref Pandocs for more details about the individual MBCs and their capabilities.


# Working with Banks
To assign code and constant data (such as graphics) to a ROM bank and use it:
  - Place the code for your ROM bank in one or several source files.
  - Specify the ROM bank to use, either in the source file or at compile/link time.
  - Specify the number of banks and MBC type during link time.
  - When the program is running and wants to use data or call a function that is in a given bank, manually or automatically set the desired bank to active.


## Setting the ROM bank for a Source file
The ROM and RAM bank for a source file can be set in a couple different ways. Multiple different banks cannot be assigned inside the same source file (unless the `__addressmod` method is used), but multiple source files can share the same bank.

If no ROM and RAM bank are speciied for a file then the default _CODE, _BSS and _DATA segments are used.

Ways to set the ROM bank for a Source file
  - `#pragma bank <N>` at the start of a source file. Example (ROM bank 2): `#pragma bank 2`
  - The lcc switch for ROM bank `-Wf-bo<N>`. Example (ROM bank 2): `-Wf-bo2`
  - Using @ref rom_autobanking

Note: You can use the `NONBANKED` keyword to define a function as non-banked if it resides in a source file which has been assigned a bank.


## Setting the RAM bank for a Source file
  - Using the lcc switch for RAM bank `-Wf-ba<N>`. Example (ROM bank 3): `-Wf-bo3`


@anchor setting_mbc_and_rom_ram_banks
## Setting the MBC and number of ROM & RAM banks available

At the link stage this is done with @ref lcc using pass-through switches for @ref makebin.
  - `-Wl-yo<N>` where `<N>` is the number of ROM banks. 2, 4, 8, 16, 32, 64, 128, 256, 512
    - `-Wl-yoA` may be used for automatic bank size.
  - `-Wl-ya<N>` where `<N>` is the number of RAM banks. 2, 4, 8, 16, 32
  - `-Wl-yt<N>` where `<N>` is the type of MBC cartridge (see below).
    - Example: `Wl-yt0x1A`

The MBC settings below are available when using the makebin MBC switch.

Additional details available at [Pandocs](https://gbdev.io/pandocs/The_Cartridge_Header.html#0147---cartridge-type "Pandocs")

```
# From Makebin source:
#
#-Wl-yt<NN> where <NN> is one of the numbers below
#
# 0147: Cartridge type:
# 0-ROM ONLY            12-ROM+MBC3+RAM
# 1-ROM+MBC1            13-ROM+MBC3+RAM+BATT
# 2-ROM+MBC1+RAM        19-ROM+MBC5
# 3-ROM+MBC1+RAM+BATT   1A-ROM+MBC5+RAM
# 5-ROM+MBC2            1B-ROM+MBC5+RAM+BATT
# 6-ROM+MBC2+BATTERY    1C-ROM+MBC5+RUMBLE
# 8-ROM+RAM             1D-ROM+MBC5+RUMBLE+SRAM
# 9-ROM+RAM+BATTERY     1E-ROM+MBC5+RUMBLE+SRAM+BATT
# B-ROM+MMM01           1F-Pocket Camera
# C-ROM+MMM01+SRAM      FD-Bandai TAMA5
# D-ROM+MMM01+SRAM+BATT FE - Hudson HuC-3
# F-ROM+MBC3+TIMER+BATT FF - Hudson HuC-1
# 10-ROM+MBC3+TIMER+RAM+BATT
# 11-ROM+MBC3
```


## Getting Bank Numbers
The bank number for a banked function, variable or source file can be stored and retrieved using the following macros:
  - @ref BANKREF(): Create a reference for retrieving the bank number of a variable or function
  - @ref BANK(): Retrieve a bank number using a reference created with @ref BANKREF()
  - @ref BANKREF_EXTERN() - Make a @ref BANKREF() reference residing in another source
    file accessible in the current file for use with @ref BANK().


## Banking and Functions

@anchor banked_keywords
### BANKED/NONBANKED keywords
- `BANKED`:
  - The function will use banked sdcc calls
  - Placed in the bank selected by it's source file (or compiler switches)
- `NONBANKED`:
  - Placed in the non-banked lower 16K region (bank 0), regardless of the bank selected by it's source file.
- `<not-specified>`:
  - The function does not use sdcc banked calls (`near` instead of `far`)
  - Placed in the bank selected by it's source file (or compiler switches)

@anchor banked_calls
### Banked Function Calls
Banked functions can be called as follows.
  - When defined with the `BANKED` keyword. Example: `void my_function() BANKED { do stuff }` in a source file which has had it's bank set (see above).
  - Using @ref far_pointers
  - When defined with an area set up using the `__addressmod` keyword (See the `banks_new` example project and the SDCC manual for details)
  - Using @ref SWITCH_ROM() (and related functions for other MBCs) to manually switch in the required bank and then call the function.

Non-banked functions (either in fixed Bank 0, or in an non-banked ROM with no MBC)
  - May call functions in any bank: __YES__
  - May use data in any bank: __YES__

@todo Fill in this info for Banked Functions
Banked functions (located in a switchable ROM bank)
  - May call functions in any bank: ?
  - May use data in any bank: __NO__ (may only use data from currently active banks)

Limitations:
  - SDCC banked calls and far_pointers in GBDK only save one byte for the ROM bank. So, for example, they are limtied to __bank 31__ max for MBC1 and __bank 255__ max for MBC5. This is due to the bank switching for those MBCs requiring a second, additional write to select the upper bits for more banks (banks 32+ in MBC1 and banks 256+ in MBC5).


## Const Data (Variables in ROM)
@todo Const Data (Variables in ROM)


## Variables in RAM
@todo Variables in RAM


## Far Pointers
@anchor far_pointers
Far pointers include a segment (bank) selector so they are able to point to addresses (functions or data) outside of the current bank (unlike normal pointers which are not bank-aware). A set of macros is provided by GBDK 2020 for working with far pointers.

__Warning:__ Do not call the far pointer function macros from inside interrupt routines (ISRs). The far pointer function macros use a global variable that would not get restored properly if a function called that way was interrupted by another one called the same way. However, they may be called recursively.

See @ref FAR_CALL, @ref TO_FAR_PTR and the `banks_farptr` example project.


## Bank switching
You can manually switch banks using the @ref SWITCH_ROM(), @ref SWITCH_RAM(), and other related macros. See `banks.c` project for an example.

Note: You can only do a switch_rom_bank call from non-banked `_CODE` since otherwise you would switch out the code that was executing. Global routines that will be called without an expectation of bank switching should fit within the limited 16k of non-banked `_CODE`.


## Restoring the current bank (after calling functions which change it without restoring)
@anchor banking_current_bank
If a function call is made (for example inside an ISR) which changes the bank *without* restoring it, then the @ref _current_bank variable should be saved and then restored.

For example, __instead__ of this code:
```
void vbl_music_isr(void)
{
    // A function which changes the bank and
    // *doesn't* restore it after changing.
    some_function();
}
```
It should be:
```
void vbl_music_isr(void)
{
    // Save the current bank
    uint8_t _saved_bank = _current_bank;

    // A function which changes the bank and
    // *doesn't* restore it after changing.
    some_function();

    // Now restore the current bank
    SWITCH_ROM(_saved_bank);
}
```

## Currently active bank: _current_bank
The global variable @ref _current_bank is updated automatically when calling @ref SWITCH_ROM(), @ref SWITCH_ROM_MBC1() and @ref SWITCH_ROM_MBC5, or when a `BANKED` function is called.



# Auto-Banking
@anchor rom_autobanking
A ROM bank auto-assignment feature was added in GBDK 2020 4.0.2.

Instead of having to manually specify which bank a source file will reside it, the banks can be assigned automatically to make the best use of space. The bank assignment operates on object files, after compiling/assembling and before linking.

To turn on auto-banking, use the `-autobank` argument with lcc

For a source example see the `banks_autobank` project.

In the source files you want auto-banked, do the following:
  - Set the source file to be autobanked `#pragma bank 255` (this sets the temporary bank to `255`, which @ref bankpack then updates when repacking)
  - Create a reference to store the bank number for that source file: `BANKREF(<some-bank-reference-name>)`.
    - More than one `BANKREF()` may be created per file, but they should always have unique names.

In the other source files you want to access the banked data from, do the following:
  - Create an extern so the bank reference in another file is accessible: `BANKREF_EXTERN(<some-bank-reference-name>)`.
  - Obtain the bank number using  `BANK(<some-bank-reference-name>)`.

Example: level_1_map.c

        #pragma bank 255
        BANKREF(level_1_map)
        ...
        const uint8_t level_1_map[] = {... some map data here ...};

Accessing that data: main.c

      BANKREF_EXTERN(level_1_map)
      ...
      SWITCH_ROM( BANK(level_1_map) );
      // Do something with level_1_map[]

Features and Notes:
  - Fixed banked source files can be used in the same project as auto-banked source files. The bankpack tool will attempt to pack the auto-banked source files as efficiently as possible around the fixed-bank ones.

Making sure bankpack checks all files:
  - In order to correctly calculate the bank for all files every time, it is best to use the `-ext=` flag and save the auto-banked output to a different extension (such as `.rel`) and then pass the modified files to the linker. That way all object files will be processed each time the program is compiled.

        Recommended: 
        .c and .s -> (compiler) .o -> (bankpack) -> .rel -> (linker) ... -> .gb

  - It is important because when bankpack assigns a bank for an autobanked (bank=255) object file (.o) it rewrites the bank and will then no longer see the file as one that needs to be auto-banked. That file will then remain in it's previously assigned bank until a source change causes the compiler to rebuild it to an object file again which resets it's bank to 255.

  - For example consider a fixed-bank source file growing too large to share a bank with an auto-banked source file that was previously assigned to it. To avoid a bank overflow it would be important to have the auto-banked file check every time whether it can share that bank or not.

  - See @ref bankpack for more options and settings



# Errors related to banking (overflow, multiple writes to same location)
A _bank overflow_ during compile/link time (in @ref makebin) is when more code and data are allocated to a ROM bank than it has capacity for. The address for any overflowed data will be incorrect and the data is potentially unreachable since it now resides at the start of a different bank instead of the end of the expected bank.

See the @ref faq_bank_overflow_errors "FAQ entry about bank overflow errors".

The current toolchain can only detect and warn (using @ref ihxcheck) when one bank overflows into another bank that has data at its start. It cannot warn if a bank overflows into an empty one. For more complete detection, you can use the third-party @ref romusage tool.



# Bank space usage
In order to see how much space is used or remains available in a bank, you can use the third-party @ref romusage tool.


## Other important notes
  - The @ref SWITCH_ROM_MBC5 macro is not interrupt-safe. If using less than 256 banks you may always use SWITCH_ROM - that is faster. Even if you use mbc5 hardware chip in the cart.


# Banking example projects
There are several projects in the GBDK 2020 examples folder which demonstrate different ways to use banking.
  - `Banks`: A basic banking example
  - `Banks_new`: Examples of using new bank assignment and calling conventions available in GBDK 2020 and it's updated SDCC version.
  - `Banks_farptr`: Using far pointers which have the bank number built into the pointer.
  - `Banks_autobank`: Shows how to use the bank auto-assignment feature of in GBDK 2020 4.0.2 or later, instead of having to manually specify which bank a source file will reside it.

