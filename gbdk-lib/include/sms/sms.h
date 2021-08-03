/** @file sms/sms.h
    SMS/GG specific functions.
*/
#ifndef _SMS_H
#define _SMS_H

#define __GBDK_VERSION 405

#include <types.h>
#include <stdint.h>
#include <sms/hardware.h>

#define __WRITE_VDP_REG(REG, v) shadow_##REG=(v);__critical{VDP_CMD=(v),VDP_CMD=REG;}
#define __READ_VDP_REG(REG) shadow_##REG

void vmemcpy (uint16_t dst, const void *src, uint16_t size) __z88dk_callee __preserves_regs(iyh, iyl);

inline void move_bkg(uint8_t x, uint8_t y) {
	__WRITE_VDP_REG(VDP_RSCX, x);
	__WRITE_VDP_REG(VDP_RSCY, y);
}

inline void scroll_bkg(int8_t x, int8_t y) {
	__WRITE_VDP_REG(VDP_RSCX, __READ_VDP_REG(VDP_RSCX) + x);
	__WRITE_VDP_REG(VDP_RSCY, __READ_VDP_REG(VDP_RSCY) + y);
}

/** Turns the display off.

    @see DISPLAY_ON
*/
inline void display_off(void) {
	__WRITE_VDP_REG(VDP_R1, __READ_VDP_REG(VDP_R1) &= (~R1_DISP_ON));
}

/** Turns the display back on.
    @see display_off, DISPLAY_OFF
*/
#define DISPLAY_ON \
	__WRITE_VDP_REG(VDP_R1, __READ_VDP_REG(VDP_R1) |= R1_DISP_ON)

/** Turns the display off immediately.
    @see display_off, DISPLAY_ON
*/
#define DISPLAY_OFF \
	display_off();


#endif /* _SMS_H */
