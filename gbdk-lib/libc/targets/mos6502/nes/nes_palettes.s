    .include    "global.s"

    .area   GBDKOVR (PAG, OVR)
    _set_bkg_palette_PARM_3::
    _set_sprite_palette_PARM_3::
    _set_bkg_palette_entry_PARM_3::
    _set_sprite_palette_entry_PARM_3::
    .ds 2
    .num_entries:               .ds 1
    .define .src "_set_bkg_palette_PARM_3"

    .area   _HOME

; void set_bkg_palette(uint8_t first_palette, uint8_t nb_palettes, palette_color_t *rgb_data) OLDCALL;
_set_bkg_palette::
    pha
    txa
    asl
    asl
    sta *.num_entries
    pla
    asl
    asl
    tax
    jmp .set_palette_impl

; void set_sprite_palette(uint8_t first_palette, uint8_t nb_palettes, palette_color_t *rgb_data) OLDCALL;
_set_sprite_palette::
    pha
    txa
    asl
    asl
    sta *.num_entries
    pla
    asl
    asl
    tax
    adc #16
    ; jmp .set_palette_impl

.set_palette_impl:
    ldy #0
2$:
    ; skip mirror entries
    lda .paletteShadowLUT,x
    bmi 1$
    stx *.tmp
    tax
    lda [*.src],y
    sta __crt0_paletteShadow,x
    ldx *.tmp
1$:
    inx
    iny
    cpy *.num_entries
    bne 2$
    rts

; void set_bkg_palette_entry(uint8_t palette, uint8_t entry, palette_color_t rgb_data) OLDCALL;
_set_bkg_palette_entry::
    ; Ignore entry 0 (unified background color) for all palettes except first
    cpx #0
    bne 1$
    cmp #0
    beq 2$
    rts
1$:
    dex
    asl
    asl
    clc
    adc .identity,x
    tax
    lda *.src
    sta __crt0_paletteShadow+1,x
    rts
2$:
    ; Set UBC
    lda *.src
    sta __crt0_paletteShadow
    rts

; void set_sprite_palette_entry(uint8_t palette, uint8_t entry, palette_color_t rgb_data) OLDCALL;
_set_sprite_palette_entry::
    ; Ignore entry 0 (transparency encoding)
    cpx #0
    bne 1$
    rts
1$:
    dex
    asl
    asl
    clc
    adc .identity,x
    tax
    lda *.src
    sta __crt0_paletteShadow+13,x
    rts

.paletteShadowLUT:
    .db 0x00    ; UBC
    .db 0x01
    .db 0x02
    .db 0x03
    ;
    .db 0xFF    ; Skip BG1 entry 0
    .db 0x04
    .db 0x05
    .db 0x06
    ;
    .db 0xFF    ; Skip BG2 entry 0
    .db 0x07
    .db 0x08
    .db 0x09
    ;
    .db 0xFF    ; Skip BG3 entry 0
    .db 0x0A
    .db 0x0B
    .db 0x0C
    ;
    .db 0xFF    ; Skip SPR0 entry 0
    .db 0x0D
    .db 0x0E
    .db 0x0F
    ;
    .db 0xFF    ; Skip SPR1 entry 0
    .db 0x10
    .db 0x11
    .db 0x12
    ;
    .db 0xFF    ; Skip SPR2 entry 0
    .db 0x13
    .db 0x14
    .db 0x15
    ;
    .db 0xFF    ; Skip SPR3 entry 0
    .db 0x16
    .db 0x17
    .db 0x18
