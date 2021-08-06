        .include        "global.s"

        .title  "VRAM utilities"
        .module VRAMUtils
        .area   _HOME

        ;; Set background tile table from (BC) at XY = DE of size WH = HL
.set_xy_btt::
        push hl
        ld hl, #.VDP_TILEMAP

        ;; Set background tile from (BC) at YX = DE, size HW on stack, to VRAM from address (HL)
.set_xy_tt::
        push bc         ; Store source

        ld a, d
        rlca 
        rlca 
        rlca 
        rlca 
        rlca 
        rlca 
        ld d, a
        and #0x07
        add h
        ld b, a
        ld a, #0xC0
        and d
        sla e
        add e
        ld c, a         ; dest BC = HL + ((0x20 * Y) * 2) + (X * 2)

        pop hl          ; HL = source

        pop de          ; DE = HW
        push de         ; store HW
        push bc         ; store dest

1$:                     ; copy H rows
        SMS_WRITE_VDP_CMD b, c
        ld c, #.VDP_DATA
        ld b, e
        sla b
2$:                     ; copy W tiles
        outi
        jr 3$
3$:
        outi
        jr nz, 2$

        pop bc
        pop de
        dec d
        ret z

        push de

        ld a, #0x40
        add c
        ld c, a
        adc b
        sub c

;        cp #0x7f
;        jr nc, 4$
;        ld a, #0x78
;4$:        
        ld b, a
        push bc
        jr 1$
