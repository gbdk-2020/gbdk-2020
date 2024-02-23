@page docs_rombanking_mbcs ROM/RAM Banking and MBCs



# ROM/RAM Banking and MBCs (Memory Bank Controllers)
The standard Game Boy cartridge with no MBC has a fixed 32K bytes of ROM. In order to make cartridges with larger ROM sizes (to store more code and graphics) MBCs can be used. They allow switching between multiple ROM banks that use the same memory region. Only one of the banks can be selected as active at a given time, while all the other banks are inactive (and so, inaccessible).


## Non-banked cartridges
Cartridges with no MBC controller are non-banked, they have 32K bytes of fixed ROM space and no switchable banks. For these cartridges the ROM space between `0000h and 7FFFh` can be treated as a single large bank of 32K bytes, or as two contiguous banks of 16K bytes in Bank `0` at `0000h - 3FFFh` and Bank `1` at `4000h to 7FFFh`.


## MBC Banked cartridges (Memory Bank Controllers)
@anchor MBC
@anchor MBCS
Cartridges with MBCs allow the the Game Boy to work with ROMS up to 8MB in size and with RAM up to 128kB. Each bank is 16K Bytes. The following are _usually_ true, with some exceptions:
  - Bank `0` of the ROM is located in the region at `0000h - 3FFFh`. It is fixed (non-banked) and cannot be switched out for another bank.
  - Banks `1 .. N` can be switched into the upper region at `4000h - 7FFFh`. The upper limit for `N` is determined by the MBC used and available cartridge space.
  - It is not necessary to manually assign Bank `0` for source files, that will happen by default if no bank is specified.

See the @ref Pandocs for more details about the individual MBCs and their capabilities.

## Recommended MBC type
@anchor Recommended_MBC

For most projects we recommend __MBC5__.
  - The @ref SWITCH_ROM() / ref  SWITCH_RAM() macros work with MBC5 (up to ROM bank 255, @ref SWITCH_ROM_MBC5_8M may be used if a larger size is needed).

  - __MBC1 is not recommended__. Some banks in it's range are unavailable. See pandocs for more details. https://gbdev.io/pandocs/MBC1

  
### Bank 0 Size Limit and Overflows When Using MBCs
When using MBCs and bank switching the space used in the lower fixed Bank `0` **must be <= 16K bytes**. Otherwise it's data will overflow into Bank `1` and may be overwriten or overwrite other data, and can get switched out when banks are changed.

See the @ref faq_bank_overflow_errors "FAQ entry about bank overflow errors".


### Conserving Bank 0 for Important Functions and Data
When using MBCs, Bank `0` is the only bank which is always active and it's code can run regardless of what other banks are active. This means it is a limited resource and should be prioritized for data and functions which must be accessible regardless of which bank is currently active.


# Working with Banks
To assign code and constant data (such as graphics) to a ROM bank and use it:
  - Place the code for your ROM bank in one or several source files.
  - Specify the ROM bank to use, either in the source file or at compile/link time.
  - Specify the number of banks and MBC type during link time.
  - When the program is running and wants to use data or call a function that is in a given bank, manually or automatically set the desired bank to active.


## Setting the ROM bank for a Source file
The ROM and RAM bank for a source file can be set in a couple different ways. Multiple different banks cannot be assigned inside the same source file (unless the `__addressmod` method is used), but multiple source files can share the same bank.

If no ROM and RAM bank are specified for a file then the default _CODE, _BSS and _DATA segments are used.

Ways to set the ROM bank for a Source file:
  - `#pragma bank <N>` at the start of a source file. Example (ROM bank 2): `#pragma bank 2`
  - The lcc switch for ROM bank `-Wf-bo<N>`. Example (ROM bank 2): `-Wf-bo2`
  - Using @ref rom_autobanking

Note: You can use the `NONBANKED` keyword to define a function as non-banked if it resides in a source file which has been assigned a bank.


## Setting the RAM bank for a Source file
  - Using the lcc switch for Cartridge SRAM bank `-Wf-ba<N>`. Example (Cartridge SRAM bank 3): `-Wf-ba3`


@anchor setting_mbc_and_rom_ram_banks
## Setting the MBC and number of ROM & RAM banks available

At the link stage this is done with @ref lcc using pass-through switches for @ref makebin.
  - `-Wm-yo<N>` where `<N>` is the number of ROM banks. 2, 4, 8, 16, 32, 64, 128, 256, 512
    - `-Wm-yoA` may be used for automatic bank size.
  - `-Wm-ya<N>` where `<N>` is the number of RAM banks. 2, 4, 8, 16, 32
  - `-Wm-yt<N>` where `<N>` is the type of MBC cartridge (see chart below).
    - Example: `Wm-yt0x1A`
  - If passing the above arguments to @ref makebin directly **without** using @ref lcc, then the `-Wm` part should be omitted.
    - Note: Some makebin switches (such as `-yo A`) require a space when passed directly. See @ref makebin-settings for details.

The MBC settings below are available when using the makebin `-Wl-yt<N>` switch.

Source: Pandocs. Additional details available at [Pandocs](https://gbdev.io/pandocs/The_Cartridge_Header.html#0147---cartridge-type "Pandocs")

For SMS/GG, the ROM file size must be at least 64K to enable mapper support for RAM banks in emulators.
  - If the generated ROM is too small then `-yo 4` for makebin (or `-Wm-yo4` for LCC) can be used to set the size to 64K.


## MBC Type Chart
```
0147: Cartridge type:
0x00: ROM ONLY                  0x12: ROM+MBC3+RAM
0x01: ROM+MBC1                  0x13: ROM+MBC3+RAM+BATT
0x02: ROM+MBC1+RAM              0x19: ROM+MBC5
0x03: ROM+MBC1+RAM+BATT         0x1A: ROM+MBC5+RAM
0x05: ROM+MBC2                  0x1B: ROM+MBC5+RAM+BATT
0x06: ROM+MBC2+BATTERY          0x1C: ROM+MBC5+RUMBLE
0x08: ROM+RAM                   0x1D: ROM+MBC5+RUMBLE+SRAM
0x09: ROM+RAM+BATTERY           0x1E: ROM+MBC5+RUMBLE+SRAM+BATT
0x0B: ROM+MMM01                 0x1F: Pocket Camera
0x0C: ROM+MMM01+SRAM            0xFD: Bandai TAMA5
0x0D: ROM+MMM01+SRAM+BATT       0xFE: Hudson HuC-3
0x0F: ROM+MBC3+TIMER+BATT       0xFF: Hudson HuC-1
0x10: ROM+MBC3+TIMER+RAM+BATT
0x11: ROM+MBC3
```

@anchor mbc_type_chart

| Hex Code | MBC Type      | SRAM | Battery | RTC | Rumble | Extra  | Max ROM Size (1)|
| -------- | ------------- | ---- | ------- | --- | ------ | ------ | --------------- |
| 0x00     | ROM ONLY      |      |         |     |        |        | 32 K            |
| 0x01     | MBC-1 (2)     |      |         |     |        |        | 2 MB            |
| 0x02     | MBC-1 (2)     | SRAM |         |     |        |        | 2 MB            |
| 0x03     | MBC-1 (2)     | SRAM | BATTERY |     |        |        | 2 MB            |
| 0x05     | MBC-2         |      |         |     |        |        | 256 K           |
| 0x06     | MBC-2         |      | BATTERY |     |        |        | 256 K           |
| 0x08     | ROM (3)       | SRAM |         |     |        |        | 32 K            |
| 0x09     | ROM (3)       | SRAM | BATTERY |     |        |        | 32 K            |
| 0x0B     | MMM01         |      |         |     |        |        | 8 MB / N        |
| 0x0C     | MMM01         | SRAM |         |     |        |        | 8 MB / N        |
| 0x0D     | MMM01         | SRAM | BATTERY |     |        |        | 8 MB / N        |
| 0x0F     | MBC-3         |      | BATTERY | RTC |        |        | 2 MB            |
| 0x10     | MBC-3 (4)     | SRAM | BATTERY | RTC |        |        | 2 MB            |
| 0x11     | MBC-3         |      |         |     |        |        | 2 MB            |
| 0x12     | MBC-3 (4)     | SRAM |         |     |        |        | 2 MB            |
| 0x13     | MBC-3 (4)     | SRAM | BATTERY |     |        |        | 2 MB            |
| 0x19     | MBC-5         |      |         |     |        |        | 8 MB            |
| 0x1A     | MBC-5         | SRAM |         |     |        |        | 8 MB            |
| 0x1B     | MBC-5         | SRAM | BATTERY |     |        |        | 8 MB            |
| 0x1C     | MBC-5         |      |         |     | RUMBLE |        | 8 MB            |
| 0x1D     | MBC-5         | SRAM |         |     | RUMBLE |        | 8 MB            |
| 0x1E     | MBC-5         | SRAM | BATTERY |     | RUMBLE |        | 8 MB            |
| 0x20     | MBC-6         |      |         |     |        |        | ~2MB            |
| 0x22     | MBC-7         | SRAM | BATTERY |     | RUMBLE | SENSOR | 2MB             |
| 0xFC     | POCKET CAMERA |      |         |     |        |        | To Do           |
| 0xFD     | BANDAI TAMA5  |      |         |     |        |        | To Do           |
| 0xFE     | HuC3          |      |         | RTC |        |        | To Do           |
| 0xFF     | HuC1          | SRAM | BATTERY |     |        | IR     | To Do           |


1: Max possible size for MBC is shown. When used with generic @ref SWITCH_ROM() the max size may be smaller. For example:
  - The max for MBC1 becomes __Bank 31__ (512K)
  - The max for MBC5 becomes __Bank 255__ (4MB). To use the full 8MB size of MBC5 see @ref SWITCH_ROM_MBC5_8M().

2: For MBC1 some banks in it's range are unavailable. See pandocs for more details https://gbdev.io/pandocs/MBC1

3: No licensed cartridge makes use of this option. Exact behaviour is unknown.

4: MBC3 with RAM size 64 KByte refers to MBC30, used only in Pocket Monsters Crystal Version for Japan.



## Getting Bank Numbers
The bank number for a banked function, variable or source file can be stored and retrieved using the following macros:
  - @ref BANKREF(): create a reference for retrieving the bank number of a variable or function
  - @ref BANK(): retrieve a bank number using a reference created with @ref BANKREF()
  - @ref BANKREF_EXTERN(): Make a @ref BANKREF() reference residing in another source file accessible in the current file for use with @ref BANK().


## Banking and Functions

@anchor banked_keywords
### BANKED/NONBANKED Keywords for Functions
- `BANKED` (is a calling convention):
  - The function will use banked sdcc calls.
  - Placed in the bank selected by its source file (or compiler switches).
  - This keyword only specifies the __calling convention__ for the function, it does not set a bank itself.
- `NONBANKED` (is a storage attribute):
  - Placed in the non-banked lower 16K region (bank 0), regardless of the bank selected by its source file.
  - Forces the .area to `_HOME`.
- `<not-specified>`:
  - The function does not use sdcc banked calls (`near` instead of `far`).
  - Placed in the bank selected by its source file (or compiler switches).

@anchor banked_calls
### Banked Function Calls
Functions in banks can be called as follows:
  - When defined with the `BANKED` keyword. Example: `void my_function() BANKED { do stuff }` in a source file which has had its bank set (see above).
  - Using @ref far_pointers
  - When defined with an area set up using the `__addressmod` keyword (see the `banks_new` example project and the SDCC manual for details).
  - Using @ref SWITCH_ROM() (and related functions for other MBCs) to manually switch in the required bank and then call the function.

Non-banked functions (either in fixed Bank 0, or in an non-banked ROM with no MBC):
  - May call functions in any bank: __YES__
  - May use data in any bank: __YES__

Banked functions (located in a switchable ROM bank)
  - May call functions in fixed Bank 0: __YES__
  - May call `BANKED` functions in any bank: __YES__
    - The compiler and library will manage the bank switching automatically using the bank switching trampoline.
  - May use data in any bank: __NO__
    - May only use data from fixed Bank 0 and the currently active bank.
    - A @ref wrapped_function_for_banked_data "NONBANKED wrapper function" may be used to access data in other banks.
    - Banks cannot be switched manually from inside a BANKED function (otherwise it will switch out it's own function code as it is executing it, likely leading to a crash).

Limitations:
  - SDCC banked calls and far_pointers in GBDK only save one byte for the ROM bank. So, for example, they are limited to __bank 31__ max for MBC1 and __bank 255__ max for MBC5. This is due to the bank switching for those MBCs requiring a second, additional write to select the upper bits for more banks (banks 32+ in MBC1 and banks 256+ in MBC5).

Calling Convention:
  - For details see @ref banked_calling_convention "Banked Calling Convention"

## Const Data (Variables in ROM)
Data declared as `const` (read only) will be stored in ROM in the bank associated with it's source file (if none is specified it defaults to Bank 0). If that bank is a switchable bank then the data is only accesible while the given bank is active.


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


@anchor wrapped_function_for_banked_data
## Wrapper Function for Accessing Banked Data
In order to load Data in one bank from code running in another bank a `NONBANKED` wrapper function can be used. It can save the current bank, switch to another bank, operate on some data, restore the original bank and then return.

An example function which can :
- Load background data from any bank
- And which can be called from code residing in any bank

```{.c}
// This function is NONBANKED so it resides in fixed Bank 0
void set_banked_bkg_data(uint8_t first_tile, uint8_t nb_tiles, const uint8_t *data, uint8_t bank) NONBANKED 
{
    uint8_t save = CURRENT_BANK;
    SWITCH_ROM(bank);
    set_bkg_data(first_tile, nb_tiles, data);
    SWITCH_ROM(save);
}

// And then it can be called from any bank:
set_banked_bkg_data(<first tile>, <num tiles>, tile_data, BANK(tile_data));
```


@anchor banking_current_bank
## Currently active bank: CURRENT_BANK
The global variable @ref CURRENT_BANK (a macro for @ref _current_bank) is updated automatically when calling @ref SWITCH_ROM(), @ref SWITCH_ROM_MBC1() and @ref SWITCH_ROM_MBC5, or when a `BANKED` function is called.

Normaly banked calls are used and the active bank does not need to be directly managed, but in the case that it does the following shows how to save and restore it.

```{.c}
// The current bank can be saved
uint8_t _saved_bank = CURRENT_BANK;

// Call some function which changes the bank but does not restore it
// ...

// And then restored if needed
SWITCH_ROM(_saved_bank);
```


@anchor rom_autobanking
# Auto-Banking
A ROM bank auto-assignment feature was added in GBDK 2020 4.0.2.

Instead of having to manually specify which bank a source file will reside in, the banks can be assigned automatically to make the best use of space. The bank assignment operates on object files, after compiling/assembling and before linking.

To turn on auto-banking, use the `-autobank` argument with lcc.

For a source example see the `banks_autobank` project.

In the source files you want auto-banked, do the following:
  - Set the source file to be autobanked `#pragma bank 255` (this sets the temporary bank to `255`, which @ref bankpack then updates when repacking).
  - Create a reference to store the bank number for that source file: `BANKREF(<some-bank-reference-name>)`.
    - More than one `BANKREF()` may be created per file, but they should always have unique names.

In the other source files you want to access the banked data from, do the following:
  - Create an extern so the bank reference in another file is accessible: `BANKREF_EXTERN(<some-bank-reference-name>)`.
  - Obtain the bank number using  `BANK(<some-bank-reference-name>)`.

Example: level_1_map.c

```{.c}
#pragma bank 255
BANKREF(level_1_map)
...
const uint8_t level_1_map[] = {... some map data here ...};
```

Accessing that data: main.c

```{.c}
BANKREF_EXTERN(level_1_map)
...
SWITCH_ROM( BANK(level_1_map) );
// Do something with level_1_map[]
```

Features and Notes:
  - Fixed banked source files can be used in the same project as auto-banked source files. The bankpack tool will attempt to pack the auto-banked source files as efficiently as possible around the fixed-bank ones.

Making sure bankpack checks all files:
  - In order to correctly calculate the bank for all files every time, it is best to use the `-ext=` flag and save the auto-banked output to a different extension (such as `.rel`) and then pass the modified files to the linker. That way all object files will be processed each time the program is compiled.

        Recommended: 
        .c and .s -> (compiler) .o -> (bankpack) -> .rel -> (linker) ... -> .gb

  - It is important because when bankpack assigns a bank for an autobanked (bank=255) object file (.o) it rewrites the bank and will then no longer see the file as one that needs to be auto-banked. That file will then remain in its previously assigned bank until a source change causes the compiler to rebuild it to an object file again which resets its bank to 255.

  - For example consider a fixed-bank source file growing too large to share a bank with an auto-banked source file that was previously assigned to it. To avoid a bank overflow it would be important to have the auto-banked file check every time whether it can share that bank or not.

  - See @ref bankpack for more options and settings.



# Errors related to banking (overflow, multiple writes to same location)
A _bank overflow_ during compile/link time (in @ref makebin) is when more code and data are allocated to a ROM bank than it has capacity for. The address for any overflowed data will be incorrect and the data is potentially unreachable since it now resides at the start of a different bank instead of the end of the expected bank.

See the @ref faq_bank_overflow_errors "FAQ entry about bank overflow errors".

The current toolchain can only detect and warn (using @ref ihxcheck) when one bank overflows into another bank that has data at its start. It cannot warn if a bank overflows into an empty one. For more complete detection, you can use the @ref romusage tool.



# Bank space usage
In order to see how much space is used or remains available in a bank you can use the @ref romusage tool.


## Other important notes
  - The @ref SWITCH_ROM_MBC5 macro is not interrupt-safe. If using less than 256 banks you may always use SWITCH_ROM - that is faster. Even if you use mbc5 hardware chip in the cart.


# Banking example projects
There are several projects in the GBDK 2020 examples folder which demonstrate different ways to use banking.
  - `Banks`: a basic banking example
  - `Banks_new`: examples of using new bank assignment and calling conventions available in GBDK 2020 and its updated SDCC version.
  - `Banks_farptr`: using far pointers which have the bank number built into the pointer.
  - `Banks_autobank`: shows how to use the bank auto-assignment feature in GBDK 2020 4.0.2 or later, instead of having to manually specify which bank a source file will reside it.

