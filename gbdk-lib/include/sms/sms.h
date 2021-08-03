/** @file sms/sms.h
    SMS/GG specific functions.
*/
#ifndef _SMS_H
#define _SMS_H

#define __GBDK_VERSION 405

#include <types.h>
#include <stdint.h>
#include <sms/hardware.h>

/** Joypad bits.
    A logical OR of these is used in the wait_pad and joypad
    functions.  For example, to see if the B button is pressed
    try

    uint8_t keys;
    keys = joypad();
    if (keys & J_B) {
    	...
    }

    @see joypad
 */
#define	J_START      0b01000000
#define	J_SELECT     0b01000000
#define	J_B          0b00100000
#define	J_A          0b00010000
#define	J_DOWN       0b00000010
#define	J_UP         0b00000001
#define	J_LEFT       0b00000100
#define	J_RIGHT      0b00001000

#define __WRITE_VDP_REG(REG, v) shadow_##REG=(v);__critical{VDP_CMD=(shadow_##REG),VDP_CMD=REG;}
#define __READ_VDP_REG(REG) shadow_##REG

void WRITE_VDP_REG(UBYTE value, UBYTE reg) __z88dk_callee __preserves_regs(iyh, iyl);

/** Interrupt handlers
 */
typedef void (*int_handler)(void) NONBANKED;

/** Removes the VBL interrupt handler. 
    @see add_VBL()
*/
void remove_VBL(int_handler h) NONBANKED __z88dk_callee __preserves_regs(iyh, iyl);

/** Removes the LCD interrupt handler.
    @see add_LCD(), remove_VBL()
*/
void remove_LCD(int_handler h) NONBANKED __z88dk_callee __preserves_regs(b, c, iyh, iyl);

void remove_TIM(int_handler h) NONBANKED;
void remove_SIO(int_handler h) NONBANKED;
void remove_JOY(int_handler h) NONBANKED;

/** Adds a V-blank interrupt handler.
*/
void add_VBL(int_handler h) NONBANKED __z88dk_callee __preserves_regs(d, e, iyh, iyl);

/** Adds a LCD interrupt handler.
*/
void add_LCD(int_handler h) NONBANKED __z88dk_callee __preserves_regs(b, c, iyh, iyl);

void add_TIM(int_handler h) NONBANKED;
void add_SIO(int_handler h) NONBANKED;
void add_JOY(int_handler h) NONBANKED;


void vmemcpy (uint16_t dst, const void *src, uint16_t size) __z88dk_callee __preserves_regs(iyh, iyl);

inline void move_bkg(uint8_t x, uint8_t y) {
	__WRITE_VDP_REG(VDP_RSCX, x);
	__WRITE_VDP_REG(VDP_RSCY, y);
}

inline void scroll_bkg(int8_t x, int8_t y) {
	__WRITE_VDP_REG(VDP_RSCX, __READ_VDP_REG(VDP_RSCX) + x);
	__WRITE_VDP_REG(VDP_RSCY, __READ_VDP_REG(VDP_RSCY) + y);
}

/** HALTs the CPU and waits for the vertical blank interrupt (VBL) to finish.

    This is often used in main loops to idle the CPU at low power
    until it's time to start the next frame. It's also useful for
    syncing animation with the screen re-draw.

    Warning: If the VBL interrupt is disabled, this function will
    never return. If the screen is off this function returns
    immediately.
*/
void wait_vbl_done(void) NONBANKED __preserves_regs(b, c, d, e, h, l, iyh, iyl);

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

/** Reads and returns the current state of the joypad.
*/
uint8_t joypad(void) NONBANKED __preserves_regs(b, c, d, e, h, iyh, iyl);

/** Waits until at least one of the buttons given in mask are pressed.
*/
uint8_t waitpad(uint8_t mask) NONBANKED __preserves_regs(b, c, iyh, iyl);

/** Waits for the directional pad and all buttons to be released.

    Note: Checks in a loop that doesn't HALT at all, so the CPU
    will be maxed out until this call returns.
*/
void waitpadup(void) NONBANKED __preserves_regs(b, c, d, e, h, l, iyh, iyl);


#endif /* _SMS_H */
