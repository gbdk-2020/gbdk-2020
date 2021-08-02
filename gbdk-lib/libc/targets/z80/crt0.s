        .include        "global.s"

        .title  "Runtime"
        .module Runtime
        .area   _HEADER (ABS)

        .org    0x00            ; Reset 00h
        di                      ; disable interrupt
        im 1                    ; interrupt mode 1 (this won't change)
        jp .init

;        .org    0x08            ; --profile handler 

        .org    0x10            ; write HL to VDP Control Port
_SMS_READ_VDP_DATA::
        ld c, #.VDP_STAT        ; set VDP Control Port
        di                      ; make it interrupt SAFE
        out (c),l
        ei
        out (c),h
        ret

        .org    0x18            ; RST 0x18 - write HL to VDP Data Port
_SMS_WRITE_VDP_DATA::
        ld a,l                  ; (respecting VRAM time costraints)
        di
        out (#.VDP_DATA),a      ; 11
        ld a,h                  ; 4
        jp .finalize_vdp        ; 10

        .org    0x20            ; RST 0x20 == call HL
.call_hl::
        JP      (HL)

;       .org    0x28            ; empty

;       .org    0x30            ; empty

        .org    0x38            ; handle IRQ
        jp _INT_ISR

        .org    0x66            ; handle NMI
        jp _NMI_ISR

.finalize_vdp:
        ei                      ; 4 = 29 (VRAM SAFE)
        out (#.VDP_DATA),a
        ret


get_bank::                      ; get current code bank num into A
        ld a,(#.MAP_FRAME1)     ; (read current page from mapper)
        ret
set_bank::                      ; set current code bank num to A
        ld (#.MAP_FRAME1),a     ; (restore caller page)
        ret

.init::
        ld sp, #.STACK          ; set stack pointer at end of RAM

        ld a, (#.BIOS)
        ld (#__BIOS), a         ; save BIOS value

        ld hl,#0x0000           ; initialize mappers
        ld (#.RAM_CONTROL),hl   ; [.RAM_CONTROL]=$00, [.MAP_FRAME0]=$00
        ld hl,#0x0201
        ld (#.MAP_FRAME1),hl    ; [.MAP_FRAME1]=$01, [.MAP_FRAME2]=$02

        ld (_SMS_Port3EBIOSvalue),a     ; restore contents of $c000 to SMS_Port3EBIOSvalue var

        ;; Initialise global variables
        call gsinit
;        call _SMS_init
        ei                              ; re-enable interrupts before going to main()
        call _main
1$:
        halt
        jr 1$

_OUTI128::                              ; _OUTI128 label points to a block of 128 OUTI and a RET
        .rept 64
        outi
        .endm
_OUTI64::                               ; _OUTI64 label points to a block of 64 OUTI and a RET
        .rept 32
        outi
        .endm
_OUTI32::                               ; _OUTI32 label points to a block of 32 OUTI and a RET
        .rept 32
        outi
        .endm
_outi_block::                           ; _outi_block label points to END of OUTI block
        ret

        ;; Ordering of segments for the linker.
        .area   _HOME
        .area   _CODE
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
gsinit::
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

        .area   _BSS
.start_crt_globals:

__BIOS::
        .ds     0x01            ; GB type (GB, PGB, CGB)

.end_crt_globals:
