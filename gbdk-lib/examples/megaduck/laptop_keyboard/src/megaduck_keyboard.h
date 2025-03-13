#include <gbdk/platform.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef _MEGADUCK_KEYBOARD_H
#define _MEGADUCK_KEYBOARD_H


#define NO_KEY        0u

// Arrow keys are arbitrarily chosen to not clash with common
// ascii chars, though they do match the GBDK ibm_fixed font
#define KEY_ARROW_UP    1u
#define KEY_ARROW_DOWN  2u
#define KEY_ARROW_RIGHT 3u
#define KEY_ARROW_LEFT  4u
#define KEY_HELP        5u
#define KEY_PRINTSCREEN 6u


// Use ascii values for these keys
#define KEY_BACKSPACE   8u
#define KEY_ENTER       13u
#define KEY_ESCAPE      27u
#define KEY_DELETE      127u


// Keyboard data
typedef struct duck_keyboard_data_t {
    uint8_t flags;
    uint8_t scancode;
} duck_keyboard_data_t;


// Post-Processed key data
extern uint8_t key_pressed;
extern uint8_t key_previous;


bool duck_io_poll_keyboard(duck_keyboard_data_t * key_data);
char duck_io_process_key_data(duck_keyboard_data_t * key_data, uint8_t megaduck_model);


#endif // _MEGADUCK_KEYBOARD_H
