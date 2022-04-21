#ifndef STDARG_INCLUDE
#define STDARG_INCLUDE

#if defined(__PORT_sm83)
#include <asm/sm83/stdarg.h>
#elif defined(__PORT_z80)
#include <asm/z80/stdarg.h>
#else
#error Unrecognised port.
#endif

#endif
