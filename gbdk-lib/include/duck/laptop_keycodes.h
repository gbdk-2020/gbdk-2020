#include <gbdk/platform.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef _MEGADUCK_LAPTOP_KEYCODES_H
#define _MEGADUCK_LAPTOP_KEYCODES_H


// - Left /right shift are shared
//
// Keyboard serial reply scan codes have different ordering than System ROM character codes
// - They go diagonal down from upper left for the first *4* rows
// - The bottom 4 rows (including piano keys) are more varied

// Modifier Keys / Flags
//
// See input_key_modifier_flags__RAM_D027_
#define DUCK_IO_KEY_FLAG_KEY_REPEAT           0x01u
#define DUCK_IO_KEY_FLAG_KEY_REPEAT_BIT       0x0u
#define DUCK_IO_KEY_FLAG_CAPSLOCK             0x02u
#define DUCK_IO_KEY_FLAG_CAPSLOCK_BIT         0x1u
#define DUCK_IO_KEY_FLAG_SHIFT                0x04u
#define DUCK_IO_KEY_FLAG_SHIFT_BIT            0x2u
// Right Print Screen has actual scancode vs Left being in a flag
#define DUCK_IO_KEY_FLAG_PRINTSCREEN_LEFT     0x08u
#define DUCK_IO_KEY_FLAG_PRINTSCREEN_LEFT_BIT 0x3u

// Keyboard scancodes
// All valid keys seem to have bit 7 set (0x80+)
#define DUCK_IO_KEY_BASE_BIT                  0x7u
#define DUCK_IO_KEY_BASE                      0x80u


// First 4 rows (top of keyboard)  ~ 0x80 - 0xB7
//
// - For each row, most chars are +4 vs char to immediate left
//   due to the diagonal down-right scancode ordering
//
// Starting values
// - Row 1: 0x80
// - Row 2: 0x81
// - Row 3: 0x82
// - Row 4: 0x83

// Row 1
#define DUCK_IO_KEY_F1                  0x80u
#define DUCK_IO_KEY_F2                  0x84u
#define DUCK_IO_KEY_F3                  0x88u
#define DUCK_IO_KEY_F4                  0x8Cu
#define DUCK_IO_KEY_F5                  0x90u
#define DUCK_IO_KEY_F6                  0x94u
#define DUCK_IO_KEY_F7                  0x98u
#define DUCK_IO_KEY_F8                  0x9Cu
#define DUCK_IO_KEY_F9                  0xA0u
#define DUCK_IO_KEY_F10                 0xA4u
#define DUCK_IO_KEY_F11                 0xA8u
#define DUCK_IO_KEY_F12                 0xACu
// GAP at 0xB0 maybe Blank spot where F13 would be
// GAP at 0xB4 maybe ON Key?

// Row 2
#define DUCK_IO_KEY_ESCAPE              0x81u  // Spanish label: Salida | German label: Esc
#define DUCK_IO_KEY_1                   0x85u  // Shift alt: !
#define DUCK_IO_KEY_2                   0x89u  // Shift alt: "
#define DUCK_IO_KEY_3                   0x8Du  // Shift alt: · (Spanish, mid-dot) | § (German, legal section)
#define DUCK_IO_KEY_4                   0x91u  // Shift alt: $
#define DUCK_IO_KEY_5                   0x95u  // Shift alt: %
#define DUCK_IO_KEY_6                   0x99u  // Shift alt: &
#define DUCK_IO_KEY_7                   0x9Du  // Shift alt: /
#define DUCK_IO_KEY_8                   0xA1u  // Shift alt: (
#define DUCK_IO_KEY_9                   0xA5u  // Shift alt: )
#define DUCK_IO_KEY_0                   0xA9u  // Shift alt: "\"
#define DUCK_IO_KEY_SINGLE_QUOTE        0xADu  // Shift alt: ?  | German version: ß (eszett)
#define DUCK_IO_KEY_EXCLAMATION_FLIPPED 0xB1u  // Shift alt: ¿ (Spanish) | ` (German)  // German version: ' (single quote?)
#define DUCK_IO_KEY_BACKSPACE           0xB5u  // German label: Lösch
// See Continued Row 2 below

// Row 3
#define DUCK_IO_KEY_HELP                0x82u  // Spanish label: Ayuda | German label: Hilfe
#define DUCK_IO_KEY_Q                   0x86u
#define DUCK_IO_KEY_W                   0x8Au
#define DUCK_IO_KEY_E                   0x8Eu
#define DUCK_IO_KEY_R                   0x92u
#define DUCK_IO_KEY_T                   0x96u
#define DUCK_IO_KEY_Y                   0x9Au  // German version: z
#define DUCK_IO_KEY_U                   0x9Eu
#define DUCK_IO_KEY_I                   0xA2u
#define DUCK_IO_KEY_O                   0xA6u
#define DUCK_IO_KEY_P                   0xAAu
#define DUCK_IO_KEY_BACKTICK            0xAEu  // Shift alt: [ (Spanish, only shift mode works) | German version: Ü
#define DUCK_IO_KEY_RIGHT_SQ_BRACKET    0xB2u  // Shift alt: * | German version: · (mid-dot)
#define DUCK_IO_KEY_ENTER               0xB6u  // Spanish label: Entra | German label: Ein-gabe
// See Continued Row 3 below

// Row 4
// GAP at 0x83 maybe CAPS LOCK  (Spanish label: Mayuscula, German label: Groß)
#define DUCK_IO_KEY_A                   0x87u
#define DUCK_IO_KEY_S                   0x8Bu
#define DUCK_IO_KEY_D                   0x8Fu
#define DUCK_IO_KEY_F                   0x93u
#define DUCK_IO_KEY_G                   0x97u
#define DUCK_IO_KEY_H                   0x9Bu
#define DUCK_IO_KEY_J                   0x9Fu
#define DUCK_IO_KEY_K                   0xA3u
#define DUCK_IO_KEY_L                   0xA7u
#define DUCK_IO_KEY_N_TILDE             0xABu  // German version: ö
#define DUCK_IO_KEY_U_UMLAUT            0xAFu  // German version: ä
#define DUCK_IO_KEY_O_OVER_LINE         0xB3u  // Shift alt: [A over line] (Spanish) | ^ (German) | German version: #
// ? GAP at 0x87 ?


// Second 4 rows (bottom of keyboard) ~ 0x80 - 0xB7
//
// - For each row, most chars are +4 vs char to immediate left
//
// Starting values
// - Row 5: 0xB8
// - Row 6: 0xB9
// - Row 7: 0xBA
// - Row 8: 0xBB

// Row 5
#define DUCK_IO_KEY_Z                   0xB8u  // German version: y
#define DUCK_IO_KEY_X                   0xBCu
#define DUCK_IO_KEY_C                   0xC0u
#define DUCK_IO_KEY_V                   0xC4u
#define DUCK_IO_KEY_B                   0xC8u
#define DUCK_IO_KEY_N                   0xCCu
#define DUCK_IO_KEY_M                   0xD0u
#define DUCK_IO_KEY_COMMA               0xD4u
#define DUCK_IO_KEY_PERIOD              0xD8u
#define DUCK_IO_KEY_DASH                0xDCu  // Shift alt: _ | German version: @
// See Continued Row 5 below
// Row 6 Continued (from below)
#define DUCK_IO_KEY_DELETE              0xE0u  // *  Spanish label: Borrar | German label: Entf.



// Encoding is less orderly below

// Row 6
#define DUCK_IO_KEY_SPACE               0xB9u  // Spanish label: Espacio | German label (blank)
// Continued Row 5
#define DUCK_IO_KEY_LESS_THAN           0xBDu  // Shift alt: >
// Continued Row 6
#define DUCK_IO_KEY_PAGE_UP             0xC1u  // Spanish label: Pg Arriba | German label: Zu-rück
#define DUCK_IO_KEY_PAGE_DOWN           0xC5u  // Spanish label: Pg Abajo | German label: Wei-ter
#define DUCK_IO_KEY_MEMORY_MINUS        0xC9u
// Continued Row 5
#define DUCK_IO_KEY_MEMORY_PLUS         0xCDu
#define DUCK_IO_KEY_MEMORY_RECALL       0xD1u
#define DUCK_IO_KEY_SQUAREROOT          0xD5u
// ** 3x3 Arrow and Math Key area **
// Continued Row 6
#define DUCK_IO_KEY_MULTIPLY            0xD9u
#define DUCK_IO_KEY_ARROW_DOWN          0xDDu
#define DUCK_IO_KEY_MINUS               0xE1u
// Continued Row 3
#define DUCK_IO_KEY_ARROW_LEFT          0xE5u
#define DUCK_IO_KEY_EQUALS              0xE9u
#define DUCK_IO_KEY_ARROW_RIGHT         0xEDu
// Continued Row 2
#define DUCK_IO_KEY_DIVIDE              0xE4u  // German version: :
#define DUCK_IO_KEY_ARROW_UP            0xE8u
#define DUCK_IO_KEY_PLUS                0xECu

// Row 7
// Piano Sharp Keys
#define DUCK_IO_KEY_PIANO_DO_SHARP      0xBAu
#define DUCK_IO_KEY_PIANO_RE_SHARP      0xBEu
// GAP at 0xC2 where there is no key
#define DUCK_IO_KEY_PIANO_FA_SHARP      0xC6u
#define DUCK_IO_KEY_PIANO_SOL_SHARP     0xCAu
#define DUCK_IO_KEY_PIANO_LA_SHARP      0xCEu
// GAP at 0xD2 where there is no key
//
// Octave 2 maybe
#define DUCK_IO_KEY_PIANO_DO_2_SHARP    0xD6u
#define DUCK_IO_KEY_PIANO_RE_2_SHARP    0xDAu
// Row 6 Continued
#define DUCK_IO_KEY_PRINTSCREEN_RIGHT   0xDEu  // German label: Druck (* Mixed in with piano keys)
// Row 7 Continued
#define DUCK_IO_KEY_PIANO_FA_2_SHARP    0xE2u
#define DUCK_IO_KEY_PIANO_SOL_2_SHARP   0xE6u
#define DUCK_IO_KEY_PIANO_LA_2_SHARP    0xEAu

// Row 8
// Piano Primary Keys
#define DUCK_IO_KEY_PIANO_DO            0xBBu
#define DUCK_IO_KEY_PIANO_RE            0xBFu
#define DUCK_IO_KEY_PIANO_MI            0xC3u
#define DUCK_IO_KEY_PIANO_FA            0xC7u
#define DUCK_IO_KEY_PIANO_SOL           0xCBu
#define DUCK_IO_KEY_PIANO_LA            0xCFu
#define DUCK_IO_KEY_PIANO_SI            0xD3u
#define DUCK_IO_KEY_PIANO_DO_2          0xD7u
#define DUCK_IO_KEY_PIANO_RE_2          0xDBu
#define DUCK_IO_KEY_PIANO_MI_2          0xDFu
#define DUCK_IO_KEY_PIANO_FA_2          0xE3u
#define DUCK_IO_KEY_PIANO_SOL_2         0xE7u
#define DUCK_IO_KEY_PIANO_LA_2          0xEBu
#define DUCK_IO_KEY_PIANO_SI_2          0xEFu

#define DUCK_IO_KEY_LAST_KEY            (DUCK_IO_KEY_PIANO_SI_2u)

// Special System Codes? 0xF0+
#define DUCK_IO_KEY_MAYBE_SYST_CODES_START 0xF0u
#define DUCK_IO_KEY_MAYBE_RX_NOT_A_KEY     0xF6u

#endif // _MEGADUCK_LAPTOP_KEYCODES_H
