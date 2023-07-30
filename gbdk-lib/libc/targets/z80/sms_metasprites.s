        .include    "global.s"
        .include    "sms_metasprites_macro.s"

        .title  "Metasprites"
        .module Metasprites

        .area   _DATA

___current_metasprite:: 
        .ds     0x02
___current_base_tile::
        .ds     0x01

        .area   _INITIALIZED
___render_shadow_OAM::
        .ds     0x01
        
        .area   _INITIALIZER
        .db     #>_shadow_OAM

        .area   _CODE

; uint8_t __move_metasprite(uint8_t id, uint8_t x, uint8_t y) __z88dk_callee __preserves_regs(iyh,iyl);

___move_metasprite::
    MOVE_METASPRITE_BODY 0,0
