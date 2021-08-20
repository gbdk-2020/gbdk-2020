;--------------------------------------------------------------------------
;  far_ptr.s
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

        .module far_ptr

        .include        "global.s"
        
        .area   _HOME
                
___call__banked::

        ld      a, (#.MAP_FRAME1)
        push    af
        inc     sp
        ld      a, (___call_banked_bank)
        ld      (#.MAP_FRAME1), a
        ld      hl, (___call_banked_addr)
        CALL_HL
        dec     sp
        pop     af
        ld      (#.MAP_FRAME1), a
        ret

_to_far_ptr::
        pop bc
        pop hl
        pop de
        push de
        push hl
        push bc
        ret

        .area   _BSS
        
___call_banked_ptr::
___call_banked_addr::
        .ds     0x02            ; far pointer offset
___call_banked_bank::
        .ds     0x02            ; far pointer segment
