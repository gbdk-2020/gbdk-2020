#include <gbdk/platform.h>
#include <gbdk/console.h>
#include <gb/isr.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <duck/laptop_io.h>


// Send a request for the Laptop Hardware to play one of 6 pre-recorded Speech Phrases
//
// The input range is 1-6 (DUCK_IO_SPEECH_CMD_MIN - DUCK_IO_SPEECH_CMD_MAX)
// Playback of one sample can be interrupted by request for playback of another sample
//
// 
// # Spanish Model Phrases
// - 1. "Genial" (brilliant)
// - 2. "Estupendo" (great)
// - 3. "Fantástico" (fantastic)
// - 4. "No, inténtalo otra vez." (no, try it again)
// - 5. "No, prueba una vez más" (no, try once again)
// - 6. "Vuelve a intentarlo" (try again)
//
// # German Model Phrases
// - 1. "Ja, sehr gut" (yes, very good)
// - 2. "Gut gemacht" (well done)
// - 3. "Super" (great, awesome)
// - 4. "Uh uh, probier's noch mal" (uh oh, try again)
// - 5. "Uh uh, versuch's noch einmal" (give it another try)
// - 6. "Oooh, schade" (ohh, what a pity)
//
// Returns success or failure
bool duck_io_send_speech_cmd(uint8_t speech_sample_num) {

    duck_io_tx_buf[0] = speech_sample_num;
    duck_io_tx_buf_len = DUCK_IO_LEN_PLAY_SPEECH;

    if (duck_io_send_cmd_and_buffer(DUCK_IO_CMD_PLAY_SPEECH)) {
        return true;
    } else {
        return false;
    }
}



void main(void) {

    uint8_t gamepad;
    uint8_t speech_num = 1u;

    bool megaduck_laptop_init_ok = duck_io_laptop_init();

    SPRITES_8x8;
    SHOW_SPRITES;
    SHOW_BKG;
    printf("Initializing..\n");

    if (!megaduck_laptop_init_ok) {
        // If laptop hardware is not present then there isn't anything
        // for this program to do, so just idle in a loop
        printf("Laptop not detected\n"
               "or Failed to Initialize");
        while(1) {
            vsync();
        }
    }

    // Otherwise laptop init succeeded
    printf("Laptop Detected!\n");

    printf("\n"
           "Play built-in speech\n"
           "\n"
           "*START:start speech\n"
           "*UP/DOWN change num\n");

    gotoxy(0,10); printf("Play Num:%d  ", speech_num);

    while(1) {
        vsync();
        gamepad = joypad();

        // It is not necessary to turn on audio to play the pre-recorded Speech Phrases

        // Laptop serial command intervals below 20 msec may cause laptop hardware lockup
        // Poll for RTC every other frame (~32 msec)
        if (sys_time & 0x01u) {

            if (gamepad) {

                switch (gamepad) {
                    case J_UP:
                        if (speech_num < DUCK_IO_SPEECH_CMD_MAX) speech_num++;
                        gotoxy(0,10); printf("Play Num:%d  ", speech_num);
                        break;

                    case J_DOWN:
                        if (speech_num > DUCK_IO_SPEECH_CMD_MIN) speech_num--;
                        gotoxy(0,10); printf("Play Num:%d  ", speech_num);
                        break;

                    case J_START:
                        if (duck_io_send_speech_cmd(speech_num)) {
                            gotoxy(0,11);
                            printf("Speech OK  \n");
                        } else {
                            gotoxy(0,11);
                            printf("Speech FAIL\n");
                        }
                        break;
                }

                // Wait for all buttons to be released before continuing
                waitpadup();
            }
        }
    }
}

