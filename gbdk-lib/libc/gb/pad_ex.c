#include <gb/gb.h>
#include <gb/sgb.h>

UINT8 joypad_init(UINT8 npads, joypads_t * joypads) {
    UINT8 np = npads;
    if (sgb_check()) {
        switch (np) {
            case 2: 
                sgb_transfer("\x89\x01"); 
                break;
            case 4:
                sgb_transfer("\x89\x03"); 
                break;
            default:
                sgb_transfer("\x89\x00");
                np = 1;
                break;
        }
    } else np = 1;
    return joypads->npads = np;
}

void joypad_ex(joypads_t * joypads) __naked {
    joypads;
    __asm
        lda hl, 2(sp)
        ld a, (hl+)
        ld e, a
        ld d, (hl)

        ld a, (de)
        inc de
        or a
        jr z, 2$

        dec a
        and #3
        inc a
2$:
        push bc
        ld b, a
1$:        
        ld a, #0x30
        ldh (#_P1_REG), a
        ldh a, (#_P1_REG)
        and #0x0f
        sub #0x0f
        cpl
        inc a       ; A contains joypad number

        and #3      ; buffer overrun protection

        add e       ; HL = DE + A
        ld l, a
        adc d
        sub l
        ld h, a

        ld a, #0x20
        ldh (#_P1_REG), a
        ldh a, (#_P1_REG)
        ldh a, (#_P1_REG)
        and #0x0f
        ld c, a
        ld a, #0x10
        ldh (#_P1_REG), a
        ldh a, (#_P1_REG)
        ldh a, (#_P1_REG)
        ldh a, (#_P1_REG)
        ldh a, (#_P1_REG)
        ldh a, (#_P1_REG)
        ldh a, (#_P1_REG)
        and #0x0f
        swap a
        or c
        cpl
        ld (hl), a

        dec b
        jr nz, 1$
        pop bc

        ret 
    __endasm;
}
