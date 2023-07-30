        .include    "global.s"
        .include    "sms_metasprites_macro.s"

        .title  "Metasprites"
        .module Metasprites

        .area   _DATA

        .globl ___current_metasprite, ___current_base_tile, ___render_shadow_OAM

        .area   _CODE

___move_metasprite_flipx::
    MOVE_METASPRITE_BODY 1,0
