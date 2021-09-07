        .include        "global.s"

        .title  "Runtime"
        .module Runtime
        .area   _HEADER (ABS)
        
        .globl  _set_default_palette

        .org    0x00            ; Reset 00h
        di                      ; disable interrupt
        im 1                    ; interrupt mode 1 (this won't change)
        jp .init

;        .org    0x08            ; --profile handler 

        .org    0x10            ; RST 0x10: SMS_WRITE_VDP_CMD

_WRITE_VDP_CMD::        
        SMS_WRITE_VDP_CMD h, l
        ret

;        .org    0x18            ; unusable

        .org    0x20            ; RST 0x20: SMS_WRITE_VDP_DATA

_WRITE_VDP_DATA::        
        SMS_WRITE_VDP_DATA h, l
        ret

;       .org    0x28             ; unusable

        .org    0x30            ; RST 0x30: call HL
.call_hl::
        jp      (HL)

        .org    0x38            ; handle IRQ
        jp _INT_ISR

        .org    0x66            ; handle NMI
        jp _NMI_ISR

        .org    0x80

.init::
        ld sp, #.STACK          ; set stack pointer at end of RAM

        ld a, (#.BIOS)
        ld (#__BIOS), a         ; save BIOS value

        ld hl, #_shadow_OAM
        ld de, #(_shadow_OAM + 1)
        ld bc, #64
        ld (hl), #0xc0
        ldir
        ld (hl), #0
        ld bc, #(128 - 1)
        ldir

        ld hl,#0x0000           ; initialize mappers
        ld (#.RAM_CONTROL),hl   ; [.RAM_CONTROL]=$00, [.MAP_FRAME0]=$00
        ld hl,#0x0201
        ld (#.MAP_FRAME1),hl    ; [.MAP_FRAME1]=$01, [.MAP_FRAME2]=$02

        ;; Initialise global variables
        call .gsinit

        ;; Initialize VDP
        ld c, #.VDP_CMD
        ld b, #(.shadow_VDP_end - .shadow_VDP)
        ld hl,#(.shadow_VDP_end - 1)
1$:
        outd

        ld a, b
        or #.VDP_REG_MASK
        out (c), a
            
        ld a, b
        or a
        jr nz, 1$

        ;; detect PAL/NTSC
        
        ld c, #.VDP_VCOUNTER
2$:     in a, (c)
        cp #0x80
        jr nz, 2$
3$:     ld b, a
        in a, (c)
        cp b
        jr nc, 3$

        ld a, b
        cp #0xE8
        ld a, #.SYSTEM_NTSC
        jr c, 4$
        ld a, #.SYSTEM_PAL
4$:
        ld (#__SYSTEM), a

        ld a, #<.VDP_VRAM
        out (#.VDP_CMD), a
        ld a, #>.VDP_VRAM
        out (#.VDP_CMD), a
        xor a
        ld bc, #0x4101
        jr 6$
5$:
        out (.VDP_DATA), a
6$:
        dec c
        jr nz, 5$
        dec b
        jr nz, 5$

        call _set_default_palette

        VDP_CANCEL_INT

        ei                      ; re-enable interrupts before going to main()
        call _main
10$:
        halt
        jr 10$

        ;; Ordering of segments for the linker.
        .area   _HOME
        .area   _BASE
        .area   _CODE
        .area   _CODE_0
        .area   _LIT
        .area   _INITIALIZER
        .area   _GSINIT
        .area   _GSFINAL

        .area   _DATA
        .area   _INITIALIZED
        .area   _BSEG
        .area   _BSS
        .area   _HEAP

        .area   _CODE
        .area   _GSINIT
.gsinit::
        ld bc, #l__INITIALIZER
        ld a, b
        or a, c
        jr Z, 1$
        ld de, #s__INITIALIZED
        ld hl, #s__INITIALIZER
        ldir
1$:

        .area   _GSFINAL
        ret

        .area   _HOME
        ;; Wait for VBL interrupt to be finished
.wait_vbl_done::
_wait_vbl_done::
        ld  a, (_shadow_VDP_R1)
        and #.R1_DISP_ON
        ret z
        
        xor a
        ld (.vbl_done), a
1$:
        halt
        ld a, (.vbl_done)
        or a
        jr z, 1$
        ret

        .area   _BSS
        
.start_crt_globals:
__BIOS::
        .ds     0x01            ; GB type (GB, PGB, CGB)
__SYSTEM::
        .ds     0x01            ; PAL/NTSC
.end_crt_globals:

        .area _INITIALIZED
.shadow_VDP:
_shadow_VDP_R0::
        .ds     0x01
_shadow_VDP_R1::
        .ds     0x01
_shadow_VDP_R2::
        .ds     0x01
_shadow_VDP_R3::
        .ds     0x01
_shadow_VDP_R4::
        .ds     0x01
_shadow_VDP_R5::
        .ds     0x01
_shadow_VDP_R6::
        .ds     0x01
_shadow_VDP_R7::
_shadow_VDP_RBORDER::
        .ds     0x01
_shadow_VDP_R8::
_shadow_VDP_RSCX::
        .ds     0x01
_shadow_VDP_R9::
_shadow_VDP_RSCY::
        .ds     0x01
_shadow_VDP_R10::
        .ds     0x01
.shadow_VDP_end::   

.sys_time::
_sys_time::
        .ds     0x02
.vbl_done::
        .ds     0x01
_VDP_ATTR_SHIFT::
.vdp_shift::
        .ds     0x01
__shadow_OAM_base::
        .ds     0x01
__shadow_OAM_OFF::
        .ds     0x01
.mode::
        .ds     0x01            ; Current mode
    
        .area _INITIALIZER

        .db .R0_DEFAULT
        .db #(.R1_DEFAULT | .R1_DISP_ON | .R1_IE)     ; VBLANK
        .db .R2_MAP_0x3800
        .db 0xFF 
        .db 0xFF
        .db .R5_SAT_0x3F00
        .db .R6_DATA_0x2000
        .db #(0 | .R7_COLOR_MASK)
        .db 0                   ; SCX
        .db 0                   ; SCY
        .db .R10_INT_OFF
        .dw 0x0000              ; .sys_time
        .db 0                   ; .vbl_done
        .db 0                   ; _VDP_ATTR_SHIFT
        .db #>_shadow_OAM       ; __shadow_OAM_base
        .db 0                   ; __shadow_OAM_OFF
        .db .T_MODE_INOUT       ; .mode