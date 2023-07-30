;
; Helper macro to remove code duplication in sms_metasprites[_flipx|_flipy_flipxy]
; While still allowing them to be linked separately.
;
.macro MOVE_METASPRITE_BODY neg_dx neg_dy
        ld      hl, #4
        add     hl, sp

        ld      b, (hl)
        dec     hl
        ld      c, (hl)
        dec     hl
        ld      e, (hl)

        ld      hl, (___current_metasprite)

        ld      a, (___render_shadow_OAM)
        ld      d, a
1$:
        ld      a, (hl)         ; dy
        inc     hl
        cp      #0x80
        jp      z, 2$
.ifne neg_dy
        neg                     ; apply flipy (or no-op)
.endif
        add     b        
        ld      b, a
        cp      #0xD0
        jp      nz, 3$
        ld      a, #0xC0
3$:
        ld      (de), a

        push    de

        ld      a, e
        add     a
        add     #0x40
        ld      e, a

        ld      a, (hl)         ; dx
        inc     hl
.ifne neg_dx
        neg                     ; apply flipx (or no-op)
.endif
        add     c
        ld      c, a
        ld      (de), a
        inc     e

        ld      a, (___current_base_tile)
        add     (hl)            ; tile
        inc     hl
        ld      (de), a

        pop     de
        inc     e

        jp      1$
2$:
        pop     hl
        pop     bc
        inc     sp
        push    hl
        ld      a, e
        sub     c
        ld      l, a
        ret
.endm
