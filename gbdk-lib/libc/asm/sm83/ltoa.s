;--------------------------------------------------------------------------
;  ltoa.s
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

        .module ltoa

        .area   _HOME

_ultoa::
        push    BC
        
        lda     HL, 8(SP)
        ld      A, (HL+)
        ld      C, A
        ld      B, (HL)         ; BC: dest

        lda     HL, 4(SP)
        ld      E, L
        ld      D, H            ; DE : ulong *
        
        call    .ultoa
        
        pop     BC
        ret

_ltoa::
        push    BC
        
        lda     HL, 8(SP)
        ld      A, (HL+)
        ld      C, A
        ld      B, (HL)         ; BC: dest

        lda     HL, 4(SP)
        ld      E, L
        ld      D, H            ; DE : ulong *

        call    .ltoa

        pop     BC
        ret

.ltoa::
        push    DE
        
        ld      A, #3
        add     E
        ld      E, A
        adc     D
        sub     E
        ld      D, A
        
        ld      A, (DE)
        add     A, A
        pop     DE
        jr      NC, .ultoa

1$:
        push    DE
        ld      A, (DE)
        cpl
        add     #1
        ld      (DE), A
        inc     DE

        ld      A, (DE)
        cpl
        adc     #0
        ld      (DE), A
        inc     DE

        ld      A, (DE)
        cpl
        adc     #0
        ld      (DE), A
        inc     DE

        ld      A, (DE)
        cpl
        adc     #0
        ld      (DE), A
        
        pop     DE
        
        ld      A, #'-'
        ld      (BC), A
        inc     BC

        call    .ultoa
        dec     DE
        ret

.ultoa::                        ; convert unsigned int into ascii
        add     SP, #-5
        lda     HL, 4(SP)
        
        xor     A               ; clear value
        ld      (HL-), A
        ld      (HL-), A
        ld      (HL-), A
        ld      (HL-), A
        ld      (HL), A

        push    BC
        ld      B, #32
1$:
        ld      H, D
        ld      L, E

        sla     (HL)
        inc     HL
        rl      (HL)
        inc     HL
        rl      (HL)
        inc     HL
        rl      (HL)

        rra
        lda     HL, 2(SP)
        rla

        ld      A, (HL)
        adc     A
        daa
        ld      (HL+), A        ; #0
        ld      A, (HL)
        adc     A
        daa
        ld      (HL+), A        ; #1
        ld      A, (HL)
        adc     A
        daa
        ld      (HL+), A        ; #2
        ld      A, (HL)
        adc     A
        daa
        ld      (HL+), A        ; #3
        ld      A, (HL)
        adc     A
        daa
        ld      (HL), A         ; #4

        dec     B
        jr      NZ, 1$

        pop     BC
        push    BC

        ld      DE, #((5 << 8) | '0')
        lda     HL, 6(SP)

        scf
        ccf

3$:
        rl      D
        ld      A, (HL)
        swap    A
        and     #0x0f
        bit     0, D
        jr      NZ, 6$
        or      A
        jr      Z, 4$
6$:
        add     A, E
        ld      (BC), A
        set     0, D
        inc     BC
4$:
        ld      A, (HL-)
        and     #0x0f
        bit     0, D
        jr      NZ, 7$
        or      A
        jr      Z, 5$
7$:     
        add     A, E
        ld      (BC), A
        set     0, D
        inc     BC
5$:
        rr      D
        dec     D
        jr      NZ, 3$
        jr      C, 8$

        ld      A, #'0'         ; n == 0
        ld      (BC), A
        inc     BC
8$:
        xor     A
        ld      (BC), A         ; write trailing #0

        pop     DE

        add     sp, #5

        ret
