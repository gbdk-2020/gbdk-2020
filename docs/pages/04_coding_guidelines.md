@page docs_coding_guidelines Coding Guidelines

# Learning C / C fundamentals
Writing games and other programs with GBDK will be much easier with a basic understanding of the C language. In particular, understanding how to use C on "Embedded Platforms" (small computing systems, such as the Game Boy) can help you write better code (smaller, faster, less error prone) and avoid common pitfals.


@anchor docs_c_tutorials
## General C tutorials
  - https://www.learn-c.org/
  -  https://www.tutorialspoint.com/cprogramming/index.htm

## Embedded C introductions

  - http://dsp-book.narod.ru/CPES.pdf
  - https://www.phaedsys.com/principals/bytecraft/bytecraftdata/bcfirststeps.pdf

## Game Boy games in C

  - https://gbdev.io/list.html#c

# Understanding the hardware
In addition to understanding the C language it's important to learn how the Game Boy hardware works. What it is capable of doing, what it isn't able to do, and what resources are available to work with. A good way to do this is by reading the @ref Pandocs and checking out the @ref awesome_gb list.


# Writing optimal C code for the Game Boy and SDCC
The following guidelines can result in better code for the Game Boy, even though some of the guidance may be contrary to typical advice for general purpose computers that have more resources and speed.


## Tools

@anchor const_gbtd_gbmb
### GBTD / GBMB, Arrays and the "const" keyword
__Important__: The old @ref gbtd_gbmb "GBTD/GBMB" fails to include the `const` keyword when exporting to C source files for GBDK. That causes arrays to be created in RAM instead of ROM, which wastes RAM, uses a lot of ROM to initialize the RAM arrays and slows the compiler down a lot.

__Use of @ref toxa_gbtd_gbmb "toxa's updated GBTD/GBMB" is highly recommended.__

If you wish to use the original tools, you must add the `const` keyword every time the graphics are re-exported to C source files.


## Variables
  - Use 8-bit values as much as possible. They will be much more efficient and compact than 16 and 32 bit types.

  - Prefer unsigned variables to signed ones: The code generated will be generally more efficient, especially when comparing two values.

  - Use explicit types so you always know the size of your variables. `int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t` and `bool`.
  These are standard types defined in `stdint.h` (`#include <stdint.h>`) and `stdbool.h` (`#include <stdbool.h>`).

  - Global and local static variables are generally more efficient than local non-static variables (which go on the stack and are slower and can result in slower code).

  - @anchor const_array_data
  `const` keyword: Use const for arrays, structs and variables with read-only (constant) data. It will reduce ROM, RAM and CPU usage significantly. Non-`const` values are loaded from ROM into RAM inefficiently, and there is no benefit in loading them into the limited available RAM if they aren't going to be changed.

  - Here is how to delcare `const` pointers and variables:
    - non-const pointer to a const variable: `const uint8_t * some_pointer;`
    - const pointer to a non-const variable: `uint8_t * const some_pointer;`
    - const pointer to a const variable: `const uint8_t * const some_pointer;`
    - https://codeforwin.org/2017/11/constant-pointer-and-pointer-to-constant-in-c.html
    - https://stackoverflow.com/questions/21476869/constant-pointer-vs-pointer-to-constant

  - For calculated values that don't change, pre-compute results once and store the result. Using lookup-tables and the like can improve speed and reduce code size. Macros can sometimes help. It may be beneficial to do the calculations with an outside tool and then include the result as C code in a const array.

  - Use an advancing pointer (`someStruct->var = x; someStruct++`) to loop through arrays of structs instead of using indexing each time in the loop `someStruct[i].var = x`.

  - When modifying variables that are also changed in an Interrupt Service Routine (ISR), wrap them the relevant code block in a `__critical { }` block. See http://sdcc.sourceforge.net/doc/sdccman.pdf#section.3.9

  - When using constants and literals the `U`, `L` and `UL` postfixes can be used.
    - `U` specifies that the constant is unsigned
    - `L` specifies that the constant is long.

    - NOTE: In SDCC 3.6.0, the default for char changed from signed to unsigned. The manual says to use `--fsigned-char` for the old behavior, this option flag is included by default when compiling through @ref lcc. 

    @anchor fixed_point_type
  - A fixed point type (`fixed`) is included with GBDK when precision greater than whole numbers is required for 8 bit range values (since floating point is not included in GBDK). For example:
    
        fixed player[2];
        ...
        // Modify player position using it's 16 bit representation
        player[0].w += player_speed_x;
        player[1].w += player_speed_y;
        ...
        // Use only the upper 8 bits for setting the sprite position
        move_sprite(0, player[0].h ,player[1].h);


## Code structure

  - Do not `#include` `.c` source files into other `.c` source files. Instead create `.h` header files for them and include those.
    https://www.tutorialspoint.com/cprogramming/c_header_files.htm

  - Instead of using a blocking @ref delay() for things such as sprite animations/etc (which can prevent the rest of the game from continuing) many times it's better to use a counter which performs an action once every N frames. @ref sys_time may be useful in these cases.

  - When processing for a given frame is done and it is time to wait before starting the next frame, @ref wait_vbl_done() can be used. It uses HALT to put the CPU into a low power state until processing resumes. The CPU will wake up and resume processing at the end of the current frame when the Vertical Blanking interrupt is triggered.

  - Minimize use of multiplication, modulo with non-powers of 2, and division with non-powers of 2. These operations have no corresponding CPU instructions (software functions), and hence are time costly.
      - SDCC has some optimizations for:
        - Division by powers of 2. For example `n /= 4u` will be optimized to `n >>= 2`.
        - Modulo by powers of 2. For example: `(n % 8)` will be optimized to `(n & 0x7)`.
      - If you need decimal numbers to count or display a score, you can use the GBDK BCD ([binary coded decimal](https://en.wikipedia.org/wiki/Binary-coded_decimal)) number functions. See: @ref bcd.h and the `BCD` example project included with GBDK.

  - Avoid long lists of function parameters. Passing many parameters can add overhead, especially if the function is called often. When applicable globals and local static vars can be used instead.

  - Use inline functions if the function is short. (with the `inline` keyword, such as `inline uint8_t myFunction() { ... }`)

  - Do not use recursive functions
<!-- This entry needs re-work. Signed vs unsigned, current SDCC optimizations...

  - Prefer `==` and `!=` comparison operators to `<`, `<=`, `>`, and `>=`. The code will be shorter and quicker.

    It is even faster to check if a variable is 0 than if it is equal to some other value, so looping from _N down to zero_ is faster than looping _from zero up to N_.

  For instance:

        for(i = 0; i < 10; i++)

  is less efficient than:

        for(i = 0; i != 10; i++)

  and if possible, even better:

        for(i = 10; i != 0; i--)
-->

## GBDK API/Library

  - stdio.h: If you have other ways of printing text, avoid including @ref stdio.h and using functions such as @ref printf(). Including it will use a large number of the background tiles for font characters. If stdio.h is not included then that space will be available for use with other tiles instead.

  - drawing.h: The Game Boy graphics hardware is not well suited to frame-buffer style graphics such as the kind provided in @ref drawing.h. Due to that, most drawing functions (rectangles, circles, etc) will be slow . When possible it's much faster and more efficient to work with the tiles and tile maps that the Game Boy hardware is built around.

  - @ref waitpad() and @ref waitpadup check for input in a loop that doesn't HALT at all, so the CPU will be maxed out until it returns. One alternative is to write a function with a loop that checks input with @ref joypad() and then waits a frame using @ref wait_vbl_done() (which idles the CPU while waiting) before checking input again.

  - @ref joypad(): When testing for multiple different buttons, it's best to read the joypad state *once* into a variable and then test using that variable (instead of making multiple calls).


## Toolchain

  - See SDCC optimizations: http://sdcc.sourceforge.net/doc/sdccman.pdf#section.8.1

  - Use profiling. Look at the ASM generated by the compiler, write several versions of a function, compare them and choose the faster one.

  - Use the SDCC `--max-allocs-per-node` flag with large values, such as `50000`. `--opt-code-speed` has a much smaller effect.
    - GBDK-2020 (after v4.0.1) compiles the library with `--max-allocs-per-node 50000`, but it must be turned on for your own code.  
    (example: `lcc ... -Wf--max-allocs-per-node50000` or `sdcc ... --max-allocs-per-node 50000`).

    - The other code/speed flags are `--opt-code-speed` or `--opt-code-size `.

  - Use current SDCC builds from http://sdcc.sourceforge.net/snap.php  
    The minimum required version of SDCC will depend on the GBDK-2020 release. See @ref docs_releases

  - Learn some ASM and inspect the compiler output to understand what the compiler is doing and how your code gets translated. This can help with writing better C code and with debugging.


@anchor docs_chars_varargs
## chars and vararg functions

In standard C when `chars` are passed to a function with variadic arguments (varargs, those delcared with `...` as a parameter), such as @ref printf(), those `chars` get automatically promoted to `ints`. For an 8 bit cpu such as the Game Boy's, this is not as efficient or desireable in most cases. So the default SDCC behavior, which GBDK-2020 expects, is that chars will remain chars and _not_ get promoted to ints when **explicitly cast as chars while calling a varargs function**.

  - They must be explicitly re-cast when passing them to a varargs function, even though they are already declared as chars.

  - Discussion in SDCC manual:  
    http://sdcc.sourceforge.net/doc/sdccman.pdf#section.1.5  
    http://sdcc.sourceforge.net/doc/sdccman.pdf#subsection.3.5.10

  - If SDCC is invoked with -std-cxx (--std-c89, --std-c99, --std-c11, etc) then it will conform to standard C behavior and calling functions such as @ref printf() with chars may not work as expected.

For example:

    unsigned char i = 0x5A;
    
    // NO:
    // The char will get promoted to an int, producing incorrect printf output
    // The output will be: 5A 00
    printf("%hx %hx", i, i);
    
    // YES:
    // The char will remain a char and printf output will be as expected
    // The output will be: 5A 5A
    printf("%hx %hx", (unsigned char)i, (unsigned char)i);

Some functions that accept varargs:
 - @ref BGB_MESSAGE_FMT, @ref gprintf(), @ref printf(), @ref sprintf()

Also See:
 - Other cases of char to int promotion: http://sdcc.sourceforge.net/doc/sdccman.pdf#chapter.6


#  When C isn't fast enough
@todo Update and verify this section for the modernized SDCC and toolchain

For many applications C is fast enough but in intensive functions are sometimes better written in assembler. This section deals with interfacing your core C program with fast assembly sub routines. 


## Calling convention
sdcc in common with almost all C compilers prepends a '_' to any function names. For example the function printf(...) begins at the label _printf::. Note that all functions are declared global.

The parameters to a function are pushed in right to left order with no aligning - so a byte takes up a byte on the stack instead of the more natural word. So for example the function int store_byte( uint16_t addr, uint8_t byte) would push 'byte' onto the stack first then addr using a total of three bytes. As the return address is also pushed, the stack would contain:

      At SP+0 - the return address

      At SP+2 - addr

      At SP+4 - byte 

Note that the arguments that are pushed first are highest in the stack due to how the Game Boy's stack grows downwards.

The function returns in DE.


## Variables and registers
C normally expects registers to be preserved across a function call. However in the case above as DE is used as the return value and HL is used for anything, only BC needs to be preserved.

Getting at C variables is slightly tricky due to how local variables are allocated on the stack. However you shouldn't be using the local variables of a calling function in any case. Global variables can be accessed by name by adding an underscore. 


## Segments
The use of segments for code, data and variables is more noticeable in assembler. GBDK and SDCC define a number of default segments - `_CODE`, `_DATA` and `_BSS`. Two extra segments `_HEADER` and `_HEAP` exist for the Game Boy header and malloc heap respectively.

The order these segments are linked together is determined by crt0.s and is currently `_CODE` in ROM, then `_DATA`, `_BSS`, `_HEAP` in WRAM, with `STACK` at the top of WRAM. `_HEAP` is placed after `_BSS` so that all spare memory is available for the malloc routines. To place code in other than the first two banks, use the segments `_CODE_x` where x is the 16kB bank number.

As the `_BSS` segment occurs outside the ROM area you can only use .ds to reserve space in it.

While you don't have to use the `_CODE` and `_DATA` distinctions in assembler you may wish to do so consistancy. 

