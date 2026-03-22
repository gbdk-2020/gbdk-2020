/** file stdlib.h
    'Standard library' functions, for whatever that means.
*/
#ifndef STD_STDLIB_INCLUDE
#define STD_STDLIB_INCLUDE

#if defined(__PORT_sm83)
  #include <asm/sm83/stdlib.h>
#elif defined(__PORT_z80)
  #include <asm/z80/stdlib.h>
#elif defined(__PORT_mos6502)
  #include <asm/mos6502/stdlib.h>
#else
  #error Unrecognized port
#endif

#endif
