;--------------------------------------------------------------------------
;  bcd.s
;
;  Copyright (C) 2020, Tony Pavlov
;
;  This library is free software; you can redistribute it and/or modify it
;  under the terms of the GNU General Public License as published by the
;  Free Software Foundation; either version 2, or (at your option) any
;  later version.
;
;  This library is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
;  GNU General Public License for more details.
;
;  You should have received a copy of the GNU General Public License 
;  along with this library; see the file COPYING. If not, write to the
;  Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
;   MA 02110-1301, USA.
;
;  As a special exception, if you link this library with other files,
;  some of which are compiled with SDCC, to produce an executable,
;  this library does not by itself cause the resulting executable to
;  be covered by the GNU General Public License. This exception does
;  not however invalidate any other reasons why the executable file
;   might be covered by the GNU General Public License.
;--------------------------------------------------------------------------

        .module bcd

        .area   _HOME
        .ez80

; void uint2bcd(uint16_t i, BCD * value) __naked
_uint2bcd::
        ld      iyh, d
        ld      iyl, e
        
        ex      DE, HL

        ; clear value
        xor     A
        ld      0(iy), A
        ld      1(iy), A
        ld      2(iy), A
        ld      3(iy), A

        ld      B, #16
1$:        
        sla     E
        rl      D
        
        ld      A, 0(iy)
        adc     A
        daa
        ld      0(iy), A
        ld      A, 1(iy)
        adc     A
        daa
        ld      1(iy), A
        ld      A, 2(iy)
        adc     A
        daa
        ld      2(iy), A
        
        dec     B
        jr      NZ, 1$

        ret

;void bcd_add(BCD * sour, BCD * value) __naked
_bcd_add::
        or      A               ; clear C, HC

        ld      A,(DE)
        add     (HL)
        daa
        ld      (HL), A
        inc     HL
        inc     DE
        
        ld      A,(DE)
        adc     (HL)
        daa
        ld      (HL), A
        inc     HL
        inc     DE
        
        ld      A,(DE)
        adc     (HL)
        daa
        ld      (HL), A
        inc     HL
        inc     DE
        
        ld      A,(DE)
        adc     (HL)
        daa
        ld      (HL), A
        
        ret

; void bcd_sub(BCD * sour, BCD * value) __naked
_bcd_sub::
        ex      DE, HL
        or      A               ; clear C, HC

        ld      A,(DE)
        sub     (HL)
        daa
        ld      (DE), A
        inc     DE
        inc     HL
        
        ld      A,(DE)
        sbc     (HL)
        daa
        ld      (DE), A
        inc     DE
        inc     HL
        
        ld      A,(DE)
        sbc     (HL)
        daa
        ld      (DE), A
        inc     DE
        inc     HL
        
        ld      A,(DE)
        sbc     (HL)
        daa
        ld      (DE), A
        
        ret

;uint8_t bcd2text(BCD * bcd, uint8_t tile_offset, uint8_t * buffer) __naked
_bcd2text::
        ex      DE, HL          ; DE: bcd
        pop     IY
        dec     SP
        pop     BC
        ld      C, B            ; C: digit offset
        ex      (SP), IY        ; HL: buffer

        inc     DE
        inc     DE
        inc     DE

        ld      B, #0x0f

        ld      A, (DE)
        rlca
        rlca
        rlca
        rlca
        and     B
        add     C
        ld      0(IY), A
        ld      A, (DE)
        and     B
        add     C
        ld      1(IY), A
        dec     DE
        
        ld      A, (DE)
        rlca
        rlca
        rlca
        rlca
        and     B
        add     C
        ld      2(IY), A
        ld      A, (DE)
        and     B
        add     C
        ld      3(IY), A
        dec     DE

        ld      A, (DE)
        rlca
        rlca
        rlca
        rlca
        and     B
        add     C
        ld      4(IY), A
        ld      A, (DE)
        and     B
        add     C
        ld      5(IY), A
        dec     DE
        
        ld      A, (DE)
        rlca
        rlca
        rlca
        rlca
        and     B
        add     C
        ld      6(IY), A
        ld      A, (DE)
        and     B
        add     C
        ld      7(IY), A
        dec     DE
        
        xor     A
        ld      8(IY), A
        
        ld      A, #0x08
        
        ret
