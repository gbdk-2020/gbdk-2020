@page docs_using_gbdk Using GBDK


# Interrupts
Interrupts allow execution to jump to a different part of your code as soon as an external event occurs - for example the LCD entering the vertical blank period, serial data arriving or the timer reaching its end count. For an example see the irq.c sample project.

Interrupts in GBDK are handled using the functions @ref disable_interrupts(), @ref enable_interrupts(), @ref set_interrupts(uint8_t ier) and the interrupt service routine (ISR) linkers @ref add_VBL(), @ref add_TIM, @ref add_low_priority_TIM, @ref add_LCD, @ref add_SIO and @ref add_JOY which add interrupt handlers for the vertical blank, timer, LCD, serial link and joypad interrupts respectively.

Since an interrupt can occur at any time an Interrupt Service Request (ISR) cannot take any arguments or return anything. Its only way of communicating with the greater program is through the global variables. When interacting with those shared ISR global variables from main code outside the interrupt, it is a good idea to wrap them in a `critical {}` section in case the interrupt occurs and modifies the variable while it is being used.

Interrupts should be disabled before adding ISRs. To use multiple interrupts, _logical OR_ the relevant IFLAGs together.

ISRs should be kept as small and short as possible, do not write an ISR so long that the Game Boy hardware spends all of its time servicing interrupts and has no time spare for the main code.

For more detail on the Game Boy interrupts consider reading about them in the @ref Pandocs.

## Available Interrupts
The GameBoy hardware can generate 5 types of interrupts. Custom Interrupt Service Routines (ISRs) can be added in addition to the built-in ones available in GBDK.

  - VBL : LCD Vertical Blanking period start
    - The default VBL ISR is installed automatically.
      - See @ref add_VBL() and @ref remove_VBL()

  - LCD : LCDC status (such as the start of a horizontal line)
    - See @ref add_LCD() and @ref remove_LCD()
    - Example project: `lcd_isr_wobble`

  - TIM : Timer overflow
    - See @ref add_TIM() (or @ref add_low_priority_TIM() ) and @ref remove_TIM()
    - Example project: `tim`

  - SIO : Serial Link I/O transfer end
    - The default SIO ISR gets installed automatically if any of the standard SIO calls are used. These calls include add_SIO(), remove_SIO(), send_byte(), receive_byte().
    - The default SIO ISR cannot be removed once installed. Only secondary chained SIO ISRs (added with add_SIO() ) can be removed.
    - See @ref add_SIO() and @ref remove_SIO()
    - Example project: `comm`

  - JOY : Transition from high to low of a joypad button
      - See @ref add_JOY() and @ref remove_JOY()

## Adding your own interrupt handler
It is possible to install your own interrupt handlers (in C or in assembly) for any of these interrupts. Up to 4 chained handlers may be added, with the last added being called last. If the remove_VBL() function is to be called, only three may be added for VBL.

Interrupt handlers are called in sequence. To install a new interrupt handler, do the following:

  1. Write a function (say foo()) that takes no parameters, and that returns nothing. Remember that the code executed in an interrupt handler must be short.
  2. Inside a `__critical { ... }` section, install your interrupt handling routines using the add_XXX() function, where XXX is the interrupt that you want to handle.
  3. Enable interrupts for the IRQ you want to handle, using the set_interrupts() function. Note that the VBL interrupt is already enabled before the main() function is called. If you want to set the interrupts before main() is called, you must install an initialization routine.

See the `irq` example project for additional details for a complete example.

## Using your own Interrupt Dispatcher
If you want to use your own Interrupt Dispatcher instead of the GBDK chained dispatcher (for improved performance), then don't call the `add_...()` function for the respective interrupt and its dispatcher won't be installed.
  - Exception: the VBL dispatcher will always be linked in at compile time.
  - For the SIO interrupt, also do not make any standard SIO calls to avoid having its dispatcher installed.

Then, @ref ISR_VECTOR() or @ref ISR_NESTED_VECTOR() can be used to install a custom ISR handler.

@anchor isr_nowait_info
## Returning from Interrupts and STAT mode
By default when an Interrupt handler completes and is ready to exit it will check STAT_REG and only return at the BEGINNING of either LCD Mode 0 or Mode 1. This helps prevent graphical glitches caused when an ISR interrupts a graphics operation in one mode but returns in a different mode for which that graphics operation is not allowed.

You can change this behavior using @ref nowait_int_handler() which does not check @ref STAT_REG before returning. Also see @ref wait_int_handler().


# What GBDK does automatically and behind the scenes

## OAM (VRAM Sprite Attribute Table)
GBDK sets up a Shadow OAM which gets copied automatically to the hardware OAM by the default V-Blank ISR. The Shadow OAM allows updating sprites without worrying about whether it is safe to write to them or not based on the hardware LCD mode.

## Font tiles when using stdio.h
Including @ref stdio.h and using functions such as @ref printf() will use a large number of the background tiles for font characters. If stdio.h is not included then that space will be available for use with other tiles instead.

## Default Interrupt Service Handlers (ISRs)
  - V-Blank: A default V-Blank ISR is installed on startup which copies the Shadow OAM to the hardware OAM and increments the global @ref sys_time variable once per frame.
  - Serial Link I/O: If any of the GBDK serial link functions are used such as @ref send_byte() and @ref receive_byte(), the default SIO serial link handler will be installed automatically at compile-time.
  - APA Graphics Mode: When this mode is used (via @ref drawing.h) custom VBL and LCD ISRs handlers will be installed (`drawing_vbl` and `drawing_lcd`). Changing the mode to (`mode(M_TEXT_OUT);`) will cause them to be de-installed. These handlers are used to change the tile data source at start-of-frame and mid-frame so that 384 background tiles can be used instead of the typical 256.


## Ensuring Safe Access to Graphics Memory
There are certain times during each video frame when memory and registers relating to graphics are "busy" and should not be read or written to (otherwise there may be corrupt or dropped data). GBDK handles this automatically for most graphics related API calls. It also ensures that ISR handlers return in such a way that if they interrupted a graphics access then it will only resume when access is allowed.

The ISR return behavior @ref isr_nowait_info "can be turned off" using the @ref nowait_int_handler.

For more details see the related Pandocs section: https://gbdev.io/pandocs/Accessing_VRAM_and_OAM.html


# Copying Functions to RAM and HIRAM
See the `ram_function` example project included with GBDK which demonstrates copying functions to RAM and HIRAM.

`Warning!` Copying of functions is generally not safe since they may contain jumps to absolute addresses that will not be converted to match the new location.

It is possible to copy functions to RAM and HIRAM (using the memcpy() and hiramcpy() functions), and execute them from C. Ensure you have enough free space in RAM or HIRAM for copying a function.

There are basically two ways for calling a function located in RAM, HIRAM, or ROM:

  - Declare a pointer-to-function variable, and set it to the address of the function to call.
  - Declare the function as extern, and set its address at link time using the -Wl-gXXX=# flag (where XXX is the name of the function, and # is its address).

The second approach is slightly more efficient. Both approaches are demonstrated in the `ram_function.c` example.


# Mixing C and Assembly
_The following is primarily oriented toward the Game Boy and related clones (sm83 devices), other targets such as sms/gg may vary._

You can mix C and assembly (ASM) in two ways as described below.
- For additional detail see the @ref links_sdcc_docs and @ref sdcc_calling_convention "SDCC Calling Conventions".

## Inline ASM within C source files
- The optional `NAKED` keyword may be used to indicate that the funtion setup and return should have no handling done by the compiler, and will instead be handled entirely by user code.
- If the entire function preserves some registers the optional `PRESERVES_REGS` keyword may be used as additional hinting for the compiler. For example `PRESERVES_REGS(b, c)`. By default it is assumed by the compiler that no registers are preserved.

Example:  

       __asm__("nop");

Another Example:  

      void some_c_function() 
      {
        // Optionally do something
        __asm
            (ASM code goes here)
        __endasm;
      }


## In Separate ASM files

It is possible to assemble and link files written in ASM alongside files written in C.

  - A C identifier `i` will be called `_i` in assembly.
  - Parameters will be passed, registers saved and results returned in a manner based on the @ref sdcc_calling_convention "SDCC Calling Convention" used and how the function is declared.
  - Assembly identifiers are exported using the `.globl` directive.
  - See global.s for examples of hardware register deginitions.

Here is an example of how to mix assembly with C:

`main.c`

    uint16_t add(uint16_t, uint16_t);

    main()
    {
      uint16_t i;

      i = add(1, 3);
    }

`add.s`

    .globl _add

    .area _CODE    
    _add:         ; uint16_t add(uint16_t First, uint16_t Second)
                  ;
                  ; In this particular example there is no use and modification of the stack
                  ; No need to save and restore registers
                  ;
                  ; For calling convention __sdcccall(1)
                  ; - first 16 bit param is passed in DE
                  ; - second 16 bit param is passed in BC
             
    ; Load Second Parameter ("Second") into HL
    ld  l,  c
    ld  h,  b

    ; Add Parameters "Second" + "First"
    add hl, de

    ; Return result in BC
    ld  c,  l
    ld  b,  h
    ret           ; 16 bit values are returned in BC


# Including binary files in C source with incbin
Data from binary files can be included in C source files as a const array using the @ref INCBIN() macro.

See the `incbin` example project for a demo of how to use it.


# Known Issues and Limitations

## SDCC
  - Const arrays declared with `somevar[n] = {x}` will __NOT__ get initialized with value `x`. This may change when the SDCC RLE initializer is fixed. Use memset for now if you need it.

  - SDCC banked calls and @ref far_pointers in GBDK only save one byte for the ROM bank, so for example they are limited to __bank 15__ max for MBC1 and __bank 255__ max for MBC5. See @ref banked_calls for more details.
  - In SDCC __pre-initializing a variable__ assigned to SRAM with `-Wf-ba*` will force that variable to be in WRAM instead.
    - The following is a workaround for initializing a variable in SRAM. It assignes value `0xA5` to a variable in `bank 0` and assigned to address `0xA000` using the AT() directive:


            // Workaround for initializing variable in SRAM
            // (MBC RAM and Bank needs to get enabled during GSINIT loading)
            static uint8_t AT(0x0000) __rRAMG = 0x0a; // Enable SRAM
            static uint8_t AT(0x4000) __rRAMB = 0x00; // Set SRAM bank 0
            // Now SRAM is enabled so the variable can get initialized
            uint8_t AT(0xA000)      initialized_sram_var = 0xA5u;


