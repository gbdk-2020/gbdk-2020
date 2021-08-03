#include <sms/sms.h>

void main() {
    DISPLAY_ON;
    move_bkg(14, 36);
    scroll_bkg(-4, 4);
    while(TRUE) {
        scroll_bkg(1, 1);
        wait_vbl_done();
    }
}
