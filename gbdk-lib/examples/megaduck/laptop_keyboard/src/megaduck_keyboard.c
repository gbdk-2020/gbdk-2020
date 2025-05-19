#include <gbdk/platform.h>
#include <stdint.h>
#include <stdbool.h>

#include <duck/laptop_io.h>
#include <duck/laptop_keycodes.h>

#include "megaduck_key2ascii.h"
#include "megaduck_keyboard.h"


#define REPEAT_OFF                 0u
#define REPEAT_FIRST_THRESHOLD     8u
#define REPEAT_CONTINUE_THRESHOLD  4u

bool    key_repeat_allowed  = false;
uint8_t key_repeat_timeout  = REPEAT_OFF;
uint8_t key_pressed         = NO_KEY;
uint8_t key_repeat_previous = NO_KEY;
uint8_t key_previous        = NO_KEY;


// RX Bytes for Keyboard Serial Reply Packet
// - 1st:
//   -  Always 0x04 (Length)
// - 2nd:
//    - KEY REPEAT : |= 0x01  (so far looks like with no key value set in 3rd Byte)
//    - CAPS_LOCK: |= 0x02
//    - SHIFT: |= 0x04
//    - LEFT_PRINTSCREEN: |= 0x08
// - 3rd:
//    - Carries the keyboard key scan code
//    - 0x00 when no key pressed
// - 4th:
//     - Two's complement checksum byte
//     - It should be: #4 == (((#1 + #2 + #3) XOR 0xFF) + 1) [two's complement]
//     - I.E: (#4 + #1 + #2 + #3) == 0x100 -> unsigned overflow -> 0x00


// Request Keyboard data and handle the response
//
// Returns success or failure (Keyboard struct data not updated if polling failed)
bool duck_io_poll_keyboard(duck_keyboard_data_t * key_data) {

    if (duck_io_send_cmd_and_receive_buffer(DUCK_IO_CMD_GET_KEYS)) {
        if (duck_io_rx_buf_len == DUCK_IO_LEN_KBD_GET) {
            key_data->flags     = duck_io_rx_buf[DUCK_IO_KBD_FLAGS];
            key_data->scancode  = duck_io_rx_buf[DUCK_IO_KBD_KEYCODE];
            return true;
        }
    }
    return false;
}


// Translates key scancodes to ascii
// Handles Shift/Caps Lock and Repeat flags
//
// Returns translated key (if no key pressed, invalid, etc value will be NO_KEY)
char duck_io_process_key_data(duck_keyboard_data_t * key_data, uint8_t megaduck_model) {

    key_previous = key_pressed;

    // Optional program layer of key repeat on top of
    // hardware key repeat (which is too fast, mostly)
    //
    // The hardware repeat works like this:
    // - First  packet after key press: Repeat Flag *NOT* set, Scan code matches key
    // - N+1   packets after key press: Repeat Flag *IS*  set, Scan code set to 0x00 (None) remains this way until key released
    if ((key_data->flags & DUCK_IO_KEY_FLAG_KEY_REPEAT) && (key_repeat_allowed)) {

        // Default to no key repeat
        key_pressed = NO_KEY;

        if (key_repeat_timeout) {
            key_repeat_timeout--;
        } else {
            // If there is a repeat then send the previous pressed key
            // and set a small delay until next repeat
            key_pressed = key_repeat_previous;
            key_repeat_timeout = REPEAT_CONTINUE_THRESHOLD;
        }
    }
    else {
        key_data->flags = key_data->flags;
        uint8_t temp_key_scancode = key_data->scancode;

        // If only shift is enabled, use keycode translation for shift alternate keys (-= 0x80u)
        if ((key_data->flags & (DUCK_IO_KEY_FLAG_CAPSLOCK | DUCK_IO_KEY_FLAG_SHIFT)) == DUCK_IO_KEY_FLAG_SHIFT)
            temp_key_scancode -= DUCK_IO_KEY_BASE;

        key_pressed = duck_io_scancode_to_ascii(temp_key_scancode, megaduck_model);

        // If only caps lock is enabled, just translate a-z -> A-Z with no other shift alternates
        if ((key_data->flags & (DUCK_IO_KEY_FLAG_CAPSLOCK | DUCK_IO_KEY_FLAG_SHIFT)) == DUCK_IO_KEY_FLAG_CAPSLOCK)
            if ((key_pressed >= 'a') && (key_pressed <= 'z'))
                key_pressed -= ('a' - 'A');

        // Allow PrintScreen flag to override any other key pressed
        // (Left printscreen is a flag, Right one is an actual key)
        if (key_data->flags & DUCK_IO_KEY_FLAG_PRINTSCREEN_LEFT)
            key_pressed = KEY_PRINTSCREEN;

        // Only allow repeat for the range from:
        // - ASCII 32 (space) and higher
        // - As well as arrow keys
        if ((key_pressed >= ' ') ||
            ((key_pressed >= KEY_ARROW_UP) && (key_pressed <= KEY_ARROW_LEFT))) {
            key_repeat_allowed = true;
            key_repeat_timeout = REPEAT_FIRST_THRESHOLD;
        } else
            key_repeat_allowed = false;

        // Save key for repeat
        key_repeat_previous = key_pressed;
    }

    return key_pressed;
}
