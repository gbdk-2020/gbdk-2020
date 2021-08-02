/** @file sms/hardware.h
    Defines that let the SMS/GG hardware registers be accessed
    from C.
*/
#ifndef _HARDWARE_H
#define _HARDWARE_H

#include <types.h>

#define __BYTES extern UBYTE
#define __BYTE_REG extern volatile UBYTE
#define __REG extern volatile __sfr


#endif
