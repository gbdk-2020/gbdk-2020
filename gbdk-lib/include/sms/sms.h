/** @file sms/sms.h
    SMS/GG specific functions.
*/
#ifndef _SMS_H
#define _SMS_H

#define __GBDK_VERSION 404

#include <types.h>
#include <stdint.h>
#include <sms/hardware.h>


void vmemcpy (uint16_t dst, const void *src, uint16_t size) __z88dk_callee __preserves_regs(iyh, iyl);

#endif /* _SMS_H */
