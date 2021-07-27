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

        .area   _BASE

; void uint2bcd(uint16_t i, BCD * value) __naked
uint2bcd::
            lda     HL, 2(SP)
            ld      A, (HL+)        ; DE: uint
            ld      E, A
            ld      A, (HL+)
            ld      D, A
            ld      A, (HL+)
            ld      H, (HL)
            ld      L, A            ; HL: dest

            push    BC

            ; clear value
            xor     A
            ld      BC, #-4
            
            ld      (HL+), A
            ld      (HL+), A
            ld      (HL+), A
            ld      (HL+), A
            add     HL, BC

            ld      B, #16
1$:            
            sla     E
            rl      D
            
            ld      A, (HL)
            adc     A
            daa
            ld      (HL+), A
            ld      A, (HL)
            adc     A
            daa
            ld      (HL+), A
            ld      A, (HL)
            adc     A
            daa
            ld      (HL-), A
            
            dec     HL
            
            dec     B
            jr      NZ, 1$

            pop     BC
            ret

;void bcd_add(BCD * sour, BCD * value) __naked
_bcd_add::
            lda     HL, 5(SP)
            ld      D, (HL)         
            dec     HL
            ld      E, (HL)         ; DE: value
            dec     HL
            ld      A, (HL-)
            ld      L, (HL)
            ld      H, A            ; HL: sour

            or      A               ; clear C, HC

            ld      A,(DE)
            add     (HL)
            daa
            ld      (HL+), A
            inc     DE
            
            ld      A,(DE)
            adc     (HL)
            daa
            ld      (HL+), A
            inc     DE
            
            ld      A,(DE)
            adc     (HL)
            daa
            ld      (HL+), A
            inc     DE
            
            ld      A,(DE)
            adc     (HL)
            daa
            ld      (HL+), A
            
            ret

; void bcd_sub(BCD * sour, BCD * value) __naked
_bcd_sub::
            lda     HL, 2(SP)
            ld      E, (HL)         
            inc     HL
            ld      D, (HL)         ; DE: sour
            inc     HL
            ld      A, (HL+)
            ld      H, (HL)
            ld      L, A            ; HL: value

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
            push    BC
            
            lda     HL, 4(SP)
            ld      E, (HL)         
            inc     HL
            ld      D, (HL)         ; DE: bcd
            inc     HL
            ld      C, (HL)         ; C: digit offset
            inc     HL
            ld      A, (HL+)
            ld      H, (HL)
            ld      L, A            ; HL: buffer

            inc     DE
            inc     DE
            inc     DE

            ld      B, #0x0f

            ld      A, (DE)
            swap    A
            and     B
            add     C
            ld      (HL+), A
            ld      A, (DE)
            and     B
            add     C
            ld      (HL+), A
            dec     DE
            
            ld      A, (DE)
            swap    A
            and     B
            add     C
            ld      (HL+), A
            ld      A, (DE)
            and     B
            add     C
            ld      (HL+), A
            dec     DE

            ld      A, (DE)
            swap    A
            and     B
            add     C
            ld      (HL+), A
            ld      A, (DE)
            and     B
            add     C
            ld      (HL+), A
            dec     DE
            
            ld      A, (DE)
            swap    A
            and     B
            add     C
            ld      (HL+), A
            ld      A, (DE)
            and     B
            add     C
            ld      (HL+), A
            dec     DE
            
            xor     A
            ld      (HL+), A
            
            ld      D, A
            ld      E, #0x08
            
            pop     BC
            ret
