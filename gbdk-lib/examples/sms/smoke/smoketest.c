#include <sms/sms.h>

BANKREF_EXTERN(earth_data)
extern const unsigned char earth_data[];

BANKREF_EXTERN(earth_data_size)
extern const unsigned int earth_data_size;

uint16_t banked_func(uint8_t be, uint8_t ef) __banked;

uint8_t joy;
void main() {
    DISPLAY_ON;
    move_bkg(14, 36);

//	vmemcpy(0x4000, earth_data, sizeof(earth_data));

    SWITCH_ROM(BANK(earth_data));
    if (banked_func(0xBE, 0xEF) == 0xBEEF) {
        set_bkg_2bpp_data(0, earth_data_size >> 4, earth_data);
    }


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
