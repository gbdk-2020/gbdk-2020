        .include        "global.s"

        .globl .image_tile_width, .set_xy_win_submap

        .area   _BASE

_set_win_submap::
        ldhl    sp, #2
        ld      a, (hl+)        ; b = x
        ld      b, a
        ld      c, (hl)         ; c = y
        
        ldhl    sp, #8
        ld      a, (hl)
        ldhl    sp, #4
        sub     (hl)
        ld      (.image_tile_width), a ; .image_tile_width contains corrected width map width
        add     (hl)

        ld      d, #0
        ld      e, a
        ld      a, c
        MUL_DE_BY_A_RET_HL
        ld      a, b
        ADD_A_REG16 h, l
        ld      d, h
        ld      e, l

        ldhl    sp, #6
        ld      a,(hl+)         
        ld      h,(hl)          
        ld      l,a             
        add     hl, de
        ld      b, h
        ld      c, l

        ldhl    sp, #2
        ld      a, (hl+)        ; d = x
        and     #0x1f
        ld      d, a
        ld      a, (hl)         ; e = y
        and     #0x1f
        ld      e, a

        ldhl    sp, #5
        ld      a,(hl-)         ; a = h
        ld      h,(hl)          ; h = w
        ld      l,a             ; l = h

        jp      .set_xy_win_submap
