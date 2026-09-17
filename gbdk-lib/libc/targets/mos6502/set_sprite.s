.include    "global.s"

.title  "SetSprite"
.module SetSprite

.area   GBDKOVR (PAG, OVR)
_move_sprite_PARM_3::
_scroll_sprite_PARM_3::     .ds 1

    .area   _HOME

;
; void set_sprite_tile(uint8_t nb, uint8_t tile)
;
_set_sprite_tile::
    asl
    asl
    tay
    txa
    sta _shadow_OAM+OAM_TILE_INDEX,y
    rts

;
; void set_sprite_prop(uint8_t nb, uint8_t prop)
;
_set_sprite_prop::
    asl
    asl
    tay
    txa
    sta _shadow_OAM+OAM_ATTRIBUTES,y
    rts

;
; void move_sprite(uint8_t nb, uint8_t x, uint8_t y)
;
_move_sprite::
    asl
    asl
    tay
    txa
    sta _shadow_OAM+OAM_POS_X,y
    lda *_move_sprite_PARM_3
    sta _shadow_OAM+OAM_POS_Y,y
    rts

;
; void scroll_sprite(uint8_t nb, uint8_t x, uint8_t y)
;
_scroll_sprite::
    asl
    asl
    tay
    txa
    clc
    adc _shadow_OAM+OAM_POS_X,y
    sta _shadow_OAM+OAM_POS_X,y
    lda *_scroll_sprite_PARM_3
    clc
    adc _shadow_OAM+OAM_POS_Y,y
    sta _shadow_OAM+OAM_POS_Y,y
    rts

;
; hide_sprite(uint8_t nb);
;
_hide_sprite::
    asl
    asl
    tay
    txa
    lda #240
    sta _shadow_OAM+OAM_POS_Y,y
    rts

;
; uint8_t get_sprite_prop(uint8_t nb)
;
_get_sprite_prop::
    asl
    asl
    tay
    lda _shadow_OAM+OAM_ATTRIBUTES,y
    rts
