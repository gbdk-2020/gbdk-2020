#include <sms/sms.h>

uint8_t joy;
void main() {
    DISPLAY_ON;
    move_bkg(14, 36);

    while(TRUE) {
        joy = joypad();
        if (joy & J_LEFT) {
            scroll_bkg(-1, 0);
        } else if (joy & J_RIGHT) {
            scroll_bkg(1, 0);
        }
        if (joy & J_UP) {
            scroll_bkg(0, -1);
        } else if (joy & J_DOWN) {
            scroll_bkg(0, 1);
        }
        
        wait_vbl_done();
    }
}
