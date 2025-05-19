#include <gbdk/platform.h>
#include <stdint.h>

#include <duck/model.h>
#include <duck/laptop_keycodes.h>

#include "megaduck_key2ascii.h"
#include "megaduck_keyboard.h"


// TODO: Not a very efficient use of space, lots of null entries
const char scancode_to_ascii_LUT_spanish[] = {

    // == Start of shift adjusted scan code range (0x00 through 0x7F) ==
    //
    // Caps lock is handled separate by manually adjusting a-z

    NO_KEY,     // 0x80
    NO_KEY,     // 0x81
    NO_KEY,     // 0x82
    NO_KEY,     // 0x83
    NO_KEY,     // 0x84
    '!',        // 0x85 Shift alt: !
    'Q',        // 0x86 Shift alt: Q
    'A',        // 0x87 Shift alt: A
    NO_KEY,     // 0x88
    '"',        // 0x89 Shift alt:  "
    'W',        // 0x8A Shift alt: W
    'S',        // 0x8B Shift alt: S
    NO_KEY,     // 0x8C
    '·',        // 0x8D Shift alt: · (Spanish, mid-dot) | § (German, legal section)
    'E',        // 0x8E Shift alt: E
    'D',        // 0x8F Shift alt: D

    NO_KEY,     // 0x90
    '$',        // 0x91 Shift alt: $
    'R',        // 0x92
    'F',        // 0x93
    NO_KEY,     // 0x94
    '%',        // 0x95 Shift alt: %
    'T',        // 0x96
    'G',        // 0x97
    NO_KEY,     // 0x98
    '&',        // 0x99 Shift alt: &
    'Y',        // 0x9A
    'H',        // 0x9B
    NO_KEY,     // 0x9C
    '/',        // 0x9D Shift alt: /
    'U',        // 0x9E
    'J',        // 0x9F

    NO_KEY,     // 0xA0
    '(',        // 0xA1 Shift alt: (
    'I',        // 0xA2
    'K',        // 0xA3
    NO_KEY,     // 0xA4
    ')',        // 0xA5 Shift alt: )
    'O',        // 0xA6
    'L',        // 0xA7
    NO_KEY,     // 0xA8
    '\\',       // 0xA9 Shift alt: "\"
    'P',        // 0xAA
    'Ñ',        // 0xAB
    NO_KEY,     // 0xAC
    '?',        // 0xAD Shift alt: ?
    '[',        // 0xAE Shift alt: [ (Spanish, only shift mode works) | German version: Ü
    'Ü',        // 0xAF

    NO_KEY,     // 0xB0
    '¿',        // 0xB1 Shift alt: ¿ (Spanish) | ` (German)  ; German version: ' (single quote?)
    '*',        // 0xB2 Shift alt: * | German version: · (mid-dot)
    'ª',        // 0xB3 Shift alt: Feminine Ordinal [A over line] (Spanish) | ^ (German)
    NO_KEY,     // 0xB4
    NO_KEY,     // 0xB5
    NO_KEY,     // 0xB6
    NO_KEY,     // 0xB7
    'Z',        // 0xB8  // German version : 'Y'
    NO_KEY,     // 0xB9
    NO_KEY,     // 0xBA
    NO_KEY,     // 0xBB
    'X',        // 0xBC
    '>',        // 0xBD Shift alt: >
    NO_KEY,     // 0xBE
    NO_KEY,     // 0xBF

    'C',        // 0xC0
    NO_KEY,     // 0xC1
    NO_KEY,     // 0xC2
    NO_KEY,     // 0xC3
    'V',        // 0xC4
    NO_KEY,     // 0xC5
    NO_KEY,     // 0xC6
    NO_KEY,     // 0xC7
    'B',        // 0xC8
    NO_KEY,     // 0xC9
    NO_KEY,     // 0xCA
    NO_KEY,     // 0xCB
    'N',        // 0xCC
    NO_KEY,     // 0xCD
    NO_KEY,     // 0xCE
    NO_KEY,     // 0xCF

    'M',        // 0xD0
    NO_KEY,     // 0xD1
    NO_KEY,     // 0xD2
    NO_KEY,     // 0xD3
    ';',        // 0xD4  ; Shift alt: ;
    NO_KEY,     // 0xD5
    NO_KEY,     // 0xD6
    NO_KEY,     // 0xD7
    ':',        // 0xD8  ; Shift alt: :
    NO_KEY,     // 0xD9
    NO_KEY,     // 0xDA
    NO_KEY,     // 0xDB
    '_',        // 0xDC  ; Shift alt: _ | German version: @
    NO_KEY,     // 0xDD
    NO_KEY,     // 0xDE
    NO_KEY,     // 0xDF

    NO_KEY,     // 0xE0
    NO_KEY,     // 0xE1
    NO_KEY,     // 0xE2
    NO_KEY,     // 0xE3
    NO_KEY,     // 0xE4
    NO_KEY,     // 0xE5
    NO_KEY,     // 0xE6
    NO_KEY,     // 0xE7
    NO_KEY,     // 0xE8
    NO_KEY,     // 0xE9
    NO_KEY,     // 0xEA
    NO_KEY,     // 0xEB
    NO_KEY,     // 0xEC
    NO_KEY,     // 0xED
    NO_KEY,     // 0xEE
    NO_KEY,     // 0xEF

    NO_KEY,     // 0xF0
    NO_KEY,     // 0xF1
    NO_KEY,     // 0xF2
    NO_KEY,     // 0xF3
    NO_KEY,     // 0xF4
    NO_KEY,     // 0xF5
    NO_KEY,     // 0xF6
    NO_KEY,     // 0xF7
    NO_KEY,     // 0xF8
    NO_KEY,     // 0xF9
    NO_KEY,     // 0xFA
    NO_KEY,     // 0xFB
    NO_KEY,     // 0xFC
    NO_KEY,     // 0xFD
    NO_KEY,     // 0xFE
    NO_KEY,     // 0xFF



    // == Start of actual scan code range (0x80 through 0xFF) ==

    NO_KEY,     // 0x80 SYS_KBD_CODE_F1
    KEY_ESCAPE, // 0x81 SYS_KBD_CODE_ESCAPE
    KEY_HELP,   // 0x82 SYS_KBD_CODE_HELP
    NO_KEY,     // 0x83
    NO_KEY,     // 0x84 SYS_KBD_CODE_F2
    '1',        // 0x85 SYS_KBD_CODE_1
    'q',        // 0x86 SYS_KBD_CODE_Q
    'a',        // 0x87 SYS_KBD_CODE_A
    NO_KEY,     // 0x88 SYS_KBD_CODE_F3
    '2',        // 0x89 SYS_KBD_CODE_2
    'w',        // 0x8A SYS_KBD_CODE_W
    's',        // 0x8B SYS_KBD_CODE_S
    NO_KEY,     // 0x8C SYS_KBD_CODE_F4
    '3',        // 0x8D SYS_KBD_CODE_3
    'e',        // 0x8E SYS_KBD_CODE_E
    'd',        // 0x8F SYS_KBD_CODE_D

    NO_KEY,     // 0x90 SYS_KBD_CODE_F5
    '4',        // 0x91 SYS_KBD_CODE_4
    'r',        // 0x92 SYS_KBD_CODE_R
    'f',        // 0x93 SYS_KBD_CODE_F
    NO_KEY,     // 0x94 SYS_KBD_CODE_F6
    '5',        // 0x95 SYS_KBD_CODE_5
    't',        // 0x96 SYS_KBD_CODE_T
    'g',        // 0x97 SYS_KBD_CODE_G
    NO_KEY,     // 0x98 SYS_KBD_CODE_F7
    '6',        // 0x99 SYS_KBD_CODE_6
    'y',        // 0x9A SYS_KBD_CODE_Y
    'h',        // 0x9B SYS_KBD_CODE_H
    NO_KEY,     // 0x9C SYS_KBD_CODE_F8
    '7',        // 0x9D SYS_KBD_CODE_7
    'u',        // 0x9E SYS_KBD_CODE_U
    'j',        // 0x9F SYS_KBD_CODE_J

    NO_KEY,     // 0xA0 SYS_KBD_CODE_F9
    '8',        // 0xA1 SYS_KBD_CODE_8
    'i',        // 0xA2 SYS_KBD_CODE_I
    'k',        // 0xA3 SYS_KBD_CODE_K
    NO_KEY,     // 0xA4 SYS_KBD_CODE_F10
    '9',        // 0xA5 SYS_KBD_CODE_9
    'o',        // 0xA6 SYS_KBD_CODE_O
    'l',        // 0xA7 SYS_KBD_CODE_L
    NO_KEY,     // 0xA8 SYS_KBD_CODE_F11
    '0',        // 0xA9 SYS_KBD_CODE_0
    'p',        // 0xAA SYS_KBD_CODE_P
    'ñ',        // 0xAB SYS_KBD_CODE_N_TILDE
    NO_KEY,     // 0xAC SYS_KBD_CODE_F12
    '\'',       // 0xAD SYS_KBD_CODE_SINGLE_QUOTE
    '`',        // 0xAE SYS_KBD_CODE_BACKTICK
    'ü',        // 0xAF SYS_KBD_CODE_U_UMLAUT

    NO_KEY,     // 0xB0
    '¡',        // 0xB1 SYS_KBD_CODE_EXCLAMATION_FLIPPED
    ']',        // 0xB2 SYS_KBD_CODE_RIGHT_SQ_BRACKET
    'º',     // 0xB3 SYS_KBD_CODE_O_OVER_LINE Masculine Ordinal
    NO_KEY,     // 0xB4
    KEY_BACKSPACE, // 0xB5 SYS_KBD_CODE_BACKSPACE
    KEY_ENTER,     // 0xB6 SYS_KBD_CODE_ENTER
    NO_KEY,     // 0xB7
    'z',        // 0xB8 SYS_KBD_CODE_Z     // German version : 'y'
    ' ',        // 0xB9 SYS_KBD_CODE_SPACE
    NO_KEY,     // 0xBA SYS_KBD_CODE_PIANO_DO_SHARP
    NO_KEY,     // 0xBB SYS_KBD_CODE_PIANO_DO
    'x',        // 0xBC SYS_KBD_CODE_X
    '<',        // 0xBD SYS_KBD_CODE_LESS_THAN
    NO_KEY,     // 0xBE SYS_KBD_CODE_PIANO_RE_SHARP
    NO_KEY,     // 0xBF SYS_KBD_CODE_PIANO_RE

    'c',        // 0xC0 SYS_KBD_CODE_C
    NO_KEY,     // 0xC1 SYS_KBD_CODE_PAGE_UP
    NO_KEY,     // 0xC2
    NO_KEY,     // 0xC3 SYS_KBD_CODE_PIANO_MI
    'v',        // 0xC4 SYS_KBD_CODE_V
    NO_KEY,     // 0xC5 SYS_KBD_CODE_PAGE_DOWN
    NO_KEY,     // 0xC6 SYS_KBD_CODE_PIANO_FA_SHARP
    NO_KEY,     // 0xC7 SYS_KBD_CODE_PIANO_FA
    'b',        // 0xC8 SYS_KBD_CODE_B
    NO_KEY,     // 0xC9 SYS_KBD_CODE_MEMORY_MINUS
    NO_KEY,     // 0xCA SYS_KBD_CODE_PIANO_SOL_SHARP
    NO_KEY,     // 0xCB SYS_KBD_CODE_PIANO_SOL
    'n',        // 0xCC SYS_KBD_CODE_N
    NO_KEY,     // 0xCD SYS_KBD_CODE_MEMORY_PLUS
    NO_KEY,     // 0xCE SYS_KBD_CODE_PIANO_LA_SHARP
    NO_KEY,     // 0xCF SYS_KBD_CODE_PIANO_LA

    'm',        // 0xD0 SYS_KBD_CODE_M
    NO_KEY,     // 0xD1 SYS_KBD_CODE_MEMORY_RECALL
    NO_KEY,     // 0xD2
    NO_KEY,     // 0xD3 SYS_KBD_CODE_PIANO_SI
    ',',        // 0xD4 SYS_KBD_CODE_COMMA
    NO_KEY,     // 0xD5 SYS_KBD_CODE_SQUAREROOT
    NO_KEY,     // 0xD6 SYS_KBD_CODE_PIANO_DO_2_SHARP
    NO_KEY,     // 0xD7 SYS_KBD_CODE_PIANO_DO_2
    '.',        // 0xD8 SYS_KBD_CODE_PERIOD
    '*',        // 0xD9 SYS_KBD_CODE_MULTIPLY
    NO_KEY,     // 0xDA SYS_KBD_CODE_PIANO_RE_2_SHARP
    NO_KEY,     // 0xDB SYS_KBD_CODE_PIANO_RE_2
    '-',        // 0xDC SYS_KBD_CODE_DASH
    KEY_ARROW_DOWN,     // 0xDD SYS_KBD_CODE_ARROW_DOWN    // TODO: Temporary
    KEY_PRINTSCREEN,    // 0xDE SYS_KBD_CODE_PRINTSCREEN_RIGHT // TODO: Temporary
    NO_KEY,     // 0xDF

    KEY_DELETE, // 0xE0 SYS_KBD_CODE_DELETE
    '-',        // 0xE1 SYS_KBD_CODE_MINUS
    NO_KEY,     // 0xE2 SYS_KBD_CODE_PIANO_FA_2_SHARP
    NO_KEY,     // 0xE3 SYS_KBD_CODE_PIANO_FA_2
    '÷',        // 0xE4 SYS_KBD_CODE_DIVIDE
    KEY_ARROW_LEFT,     // 0xE5 SYS_KBD_CODE_ARROW_LEFT    // TODO: Temporary
    NO_KEY,     // 0xE6 SYS_KBD_CODE_PIANO_SOL_2_SHARP
    NO_KEY,     // 0xE7 SYS_KBD_CODE_PIANO_SOL_2
    KEY_ARROW_UP,     // 0xE8 SYS_KBD_CODE_ARROW_UP    // TODO: Temporary
    '=',        // 0xE9 SYS_KBD_CODE_EQUALS
    NO_KEY,     // 0xEA SYS_KBD_CODE_PIANO_LA_2_SHARP
    NO_KEY,     // 0xEB SYS_KBD_CODE_PIANO_LA_2
    '+',        // 0xEC SYS_KBD_CODE_PLUS
    KEY_ARROW_RIGHT,     // 0xED SYS_KBD_CODE_ARROW_RIGHT    // TODO: Temporary
    NO_KEY,     // 0xEE
    NO_KEY,     // 0xEF SYS_KBD_CODE_PIANO_MI_2

    't',        // 0xF0 SYS_KBD_CODE_MAYBE_SYST_CODES_START
    NO_KEY,     // 0xF1
    NO_KEY,     // 0xF2
    NO_KEY,     // 0xF3
    NO_KEY,     // 0xF4
    NO_KEY,     // 0xF5
    NO_KEY,     // 0xF6 SYS_KBD_CODE_MAYBE_RX_NOT_A_KEY
    NO_KEY,     // 0xF7
    NO_KEY,     // 0xF8
    NO_KEY,     // 0xF9
    NO_KEY,     // 0xFA
    NO_KEY,     // 0xFB
    NO_KEY,     // 0xFC
    NO_KEY,     // 0xFD
    NO_KEY,     // 0xFE
    NO_KEY      // 0xFF
};


char duck_io_scancode_to_ascii(const uint8_t key_code, const uint8_t megaduck_model) {

    char ascii_char = scancode_to_ascii_LUT_spanish[key_code];

    // Handle alternate German keyboard layout
    if (megaduck_model == MEGADUCK_LAPTOP_GERMAN)
        switch (ascii_char) {
            // Row 1
            // case '·':  ascii_char = '§'; break;  // TODO: handling for these
            case '\'': ascii_char = 'ß'; break;
            // case '¿':  ascii_char = '`'; break;  // TODO: handling for these
            // case '¡':  ascii_char = '\''; break; // TODO: handling for these
            case '÷':  ascii_char = ':'; break;
            // Row 2
            case '[':  // maps to same char below
            case '`':  ascii_char = 'Ü'; break;
            case ']':  ascii_char = '·'; break;
            case 'y':  ascii_char = 'z'; break;
            case 'Y':  ascii_char = 'Z'; break;
            // Row 3
            // case 'ñ':  ascii_char = 'ö'; break; // TODO: handling for these
            // case 'Ñ':  ascii_char = 'Ö'; break; // TODO: handling for these
            // case 'ü':  ascii_char = 'ä'; break; // TODO: handling for these
            // case 'Ü':  ascii_char = 'Ä'; break; // TODO: handling for these
            // case 'ª':  ascii_char = '^'; break; // TODO: handling for these
            // case 'º':  ascii_char = '#'; break; // TODO: handling for these
            // Row 4
            case 'z':  ascii_char = 'y'; break;
            case 'Z':  ascii_char = 'Y'; break;
            case '-':  ascii_char = '@'; break;
        }

    return ascii_char;
}
