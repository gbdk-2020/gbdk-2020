#include <gbdk/platform.h>


// Declaring a variable as SFR means:
//
// It will be allocated in the High RAM (_HRAM area) address space
// of the Game Boy sm83 CPU (0xFF80 - 0xFFFE). This address space
// is eight bit addressable using the "ldh" opcodes, which allow
// for more efficient read and write access.
//
// The variable will be treated as 8 bit unsigned.

SFR my_hram_variable;