;--------------------------------------------------------------------------
;  itoa.s
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

        .module itoa

        .area   _HOME

_uitoa::
        push    BC
        lda     HL, 4(SP)
        ld      A, (HL+)
        ld      E, A
        ld      A, (HL+)
        ld      D, A            ; DE: uint
        ld      A, (HL+)
        ld      C, A
        ld      B, (HL)         ; BC: dest
        call    .utoa
        pop     BC
        ret

_itoa::
        push    BC
        lda     HL, 4(SP)
        ld      A, (HL+)
        ld      E, A
        ld      A, (HL+)
        ld      D, A            ; DE: int
        ld      A, (HL+)
        ld      C, A
        ld      B, (HL)         ; BC: dest
        call    .itoa
        pop     BC
        ret
        
.itoa::                         ; convert int into ascii
        ld      A, D
        add     A, A
        jr      NC, .utoa

        rra                     ; DE = abs(DE)
        cpl
        ld      D, A
        ld      A, E
        cpl
        ld      E, A
        inc     DE
        
        ld      A, #'-'
        ld      (BC), A
        inc     BC
        
        call    .utoa
        dec     DE
        ret

.utoa::                         ; convert unsigned int into ascii
        add     SP, #-3
        lda     HL, 2(SP)
        
        xor     A               ; clear value
        ld      (HL-), A
        ld      (HL-), A
        ld      (HL), A

        push    BC
        ld      B, #8
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
        push    BC

        ld      DE, #'0'
        lda     HL, 4(SP)
        
        ld      A, (HL-)
        and     #0x0f
        or      A
        jr      Z, 3$
        add     A, E
        ld      D, #1           ; make D nonzero
        ld      (BC), A
        inc     BC      
3$:
        ld      A, (HL)
        swap    A
        and     #0x0f
        add     D
        jr      Z, 4$
        sub     D
        add     A, E
        ld      D, #1           ; make D nonzero
        ld      (BC), A
        inc     BC
4$:
        ld      A, (HL-)
        and     #0x0f
        add     D
        jr      Z, 5$
        sub     D
        add     A, E
        ld      D, #1           ; make D nonzero
        ld      (BC), A
        inc     BC      
5$:
        ld      A, (HL)
        swap    A
        and     #0x0f
        add     D
        jr      Z, 6$
        sub     D
        add     A, E
        ld      (BC), A
        inc     BC
6$:
        ld      A, (HL)
        and     #0x0f
        add     A, E
        ld      (BC), A
        inc     BC
        
        xor     A
        ld      (BC), A         ; write trailing #0

        pop     DE

        add     sp, #3

        ret
