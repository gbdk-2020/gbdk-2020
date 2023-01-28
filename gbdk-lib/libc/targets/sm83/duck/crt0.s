        .include        "global.s"

        ;; ****************************************
        ;; Beginning of module
        ;; BANKED: checked
        .title  "Runtime"
        .module Runtime
        .area   _HEADER (ABS)

        ;; RST vectors
        .org    0x00            ; MEGADUCK entry point/reset
        JP      .code_start

;       .org    0x08            ; --profile handler utilized by emu_debug.h

;       .org    0x10            ; empty

;       .org    0x18            ; empty

        .org    0x20            ; RST 0x20 == call HL
.call_hl::
        JP      (HL)

        .org    0x28            ; zero up to 256 bytes in C pointed by HL
.MemsetSmall::
        LD      (HL+),A
        DEC     C
        JR      NZ,.MemsetSmall
        ret

        .org    0x30            ; copy up to 256 bytes in C from DE to HL
.MemcpySmall::
        LD      A, (DE)
        LD      (HL+), A
        INC     DE
        DEC     C
        JR      NZ,.MemcpySmall
        RET

;       .org    0x38            ; crash handler utilized by crash_handler.h

        ;; Hardware interrupt vectors
        .org    0x40            ; VBL
.int_VBL:
        PUSH    AF
        PUSH    HL
        LD      HL,#.int_0x40
        JP      .int

;       .org    0x48            ; LCD

;       .org    0x50            ; TIM

;       .org    0x58            ; SIO

;       .org    0x60            ; JOY

;       .org    0x70
        ;; space for drawing.s bit table

        .org    0x80
.int::
        PUSH    BC
        PUSH    DE
1$:
        LD      A,(HL+)
        OR      (HL)
        JR      Z,.int_tail
        PUSH    HL
        LD      A,(HL-)
        LD      L,(HL)
        LD      H,A
        RST     0x20            ; .call_hl
        POP     HL
        INC     HL
        JR      1$
_wait_int_handler::
        ADD     SP,#4
.int_tail:
        POP     DE
        POP     BC
        POP     HL

        ;; we return at least at the beginning of mode 2
        WAIT_STAT

        POP     AF
        RETI

        ;; VBlank default interrupt routine
__standard_VBL_handler::
.std_vbl:
        LD      HL,#.sys_time
        INC     (HL)
        JR      NZ,2$
        INC     HL
        INC     (HL)
2$:
        CALL    .refresh_OAM

        LD      A, #1
        LDH     (.vbl_done),A
        RET

_refresh_OAM::
        WAIT_STAT
        LD      A, #>_shadow_OAM
        JP      .refresh_OAM + (.refresh_OAM_DMA - .start_refresh_OAM)

.clear_WRAM:
        XOR     A
        LD      BC, #l__DATA
        LD      HL, #s__DATA
        CALL    .memset_simple

        LD      A, #>_shadow_OAM
        LDH     (__shadow_OAM_base), A
        LD      H, A
        XOR     A
        LD      L, A
        LD      C, #(40 << 2)   ; 40 entries 4 bytes each
        RST     0x28
        RET

        .org    0x150

        ;; soft reset: falldown to .code_start
.reset::
_reset::

        ;; Initialization code
.code_start::
        DI                      ; Disable interrupts
        ;; Initialize the stack
        LD      SP, #.STACK

        PUSH    DE
        ;; Turn the screen off
        CALL    .display_off
        ;; Clear the static storage
        CALL    .clear_WRAM
        POP     DE

;       LD      (.mode),A       ; Clearing (.mode) is performed when clearing RAM

        ;; Store CPU type
        LD      A, #.DMG_TYPE   ; MEGADUCK is always a DMG
        LD      (__cpu), A
        XOR     A
        LD      (__is_GBA), A   ; and it is never GBA

        XOR     A
        ;; Initialize the display
        LDH     (.SCY),A
        LDH     (.SCX),A
        LDH     (.STAT),A
        LDH     (.WY),A
        LD      A,#0x07
        LDH     (.WX),A

        ;; Copy refresh_OAM routine to HRAM
        LD      DE,#.start_refresh_OAM                          ; source
        LD      HL,#.refresh_OAM                                ; dest
        LD      C,#(.end_refresh_OAM - .start_refresh_OAM)      ; size
        RST     0x30                                            ; call .MemcpySmall

        ;; Clear the OAM by calling refresh_OAM
        CALL    .refresh_OAM

        ;; Install interrupt routines
        LD      DE,#.std_vbl
        CALL    .add_VBL

        ;; Standard color palettes
        LD      A,#0b11100100   ; Grey 3 = 11 (Black)
                                ; Grey 2 = 10 (Dark grey)
                                ; Grey 1 = 01 (Light grey)
                                ; Grey 0 = 00 (Transparent)
        LDH     (.BGP),A
        LDH     (.OBP0),A
        LD      A,#0b00011011
        LDH     (.OBP1),A

        ;; Turn the screen on
        LD      A,#(LCDCF_ON | LCDCF_WIN9C00 | LCDCF_WINOFF | LCDCF_BG8800 | LCDCF_BG9800 | LCDCF_OBJ8 | LCDCF_OBJOFF | LCDCF_BGOFF)
        LDH     (.LCDC),A
        XOR     A
        LDH     (.IF),A
        LD      A,#.VBL_IFLAG   ; switch on VBlank interrupt only
        LDH     (.IE),A

        LDH     (__current_bank),A      ; current bank is 1 at startup

        XOR     A

        LD      HL,#.sys_time
        LD      (HL+),A
        LD      (HL),A

        LDH     (.NR52),A       ; Turn sound off

        CALL    gsinit

        EI                      ; Enable interrupts

        ;; Call the main function
        CALL    _main
_exit::
99$:
        HALT
        NOP
        JR      99$             ; Wait forever

_set_interrupts::
        DI
        LDH     (.IE),A
        XOR     A
        EI
        LDH     (.IF),A         ; Clear pending interrupts
        RET

        ;; Copy OAM data to OAM RAM
.start_refresh_OAM:
        LDH     A,(__shadow_OAM_base)
        OR      A
        RET     Z
.refresh_OAM_DMA:
        LDH     (.DMA),A        ; Put A into DMA registers
        LD      A,#0x28         ; We need to wait 160 ns
1$:
        DEC     A
        JR      NZ,1$
        RET
.end_refresh_OAM:

        .org    .MODE_TABLE
        ;; Jump table for modes
        RET

        ;; ****************************************

        ;; Ordering of segments for the linker
        ;; Code that really needs to be in bank 0
        .area   _HOME
        ;; Similar to _HOME
        .area   _BASE
        ;; Code
        .area   _CODE
        ;; #pragma bank 0 workaround
        .area   _CODE_0
        ;; Constant data
        .area   _LIT
;       ;; since _CODE_1 area base address is pre-defined in the linker from 0x4000,
;       ;; that moves initializer code and tables out of bank 0
;       .area   _CODE_1
        ;; Constant data, used to init _DATA
        .area   _INITIALIZER
        ;; Code, used to init _DATA
        .area   _GSINIT
        .area   _GSFINAL
        ;; Uninitialised ram data
        .area   _DATA
        .area   _BSS
        ;; Initialised in ram data
        .area   _INITIALIZED
        ;; For malloc
        .area   _HEAP
        .area   _HEAP_END

        .area   _DATA
__cpu::
        .ds     0x01            ; GB type (GB, PGB, CGB)
__is_GBA::
        .ds     0x01            ; detect GBA
.mode::
        .ds     0x01            ; Current mode
.sys_time::
_sys_time::
        .ds     0x02            ; System time in VBL units
.int_0x40::
        .blkw   0x06            ; 5 interrupt handlers: 1 built-in + 4 user-defined

        .area   _HRAM (ABS)

        .org    0xFF90
__current_bank::        ; Current bank
        .ds     0x01
.vbl_done:
        .ds     0x01            ; Is VBL interrupt finished?
__shadow_OAM_base::
        .ds     0x01

        ;; Runtime library
        .area   _GSINIT
gsinit::
        ;; initialize static storage variables
        LD      BC, #l__INITIALIZER
        LD      HL, #s__INITIALIZER
        LD      DE, #s__INITIALIZED
        call    .memcpy_simple

        .area   _GSFINAL
        ret

        .area   _HOME

        ;; fills memory at HL of length BC with A, clobbers DE
.memset_simple::
        LD      E, A
        LD      A, B
        OR      C
        RET     Z
        LD      (HL), E
        DEC     BC
        LD      D, H
        LD      E, L
        INC     DE

        ;; copies BC bytes from HL into DE
.memcpy_simple::
        LD      A, B
        OR      C
        RET     Z

        SRL     B
        RR      C
        JR      NC,3$
        LD      A, (HL+)
        LD      (DE), A
        INC     DE
3$:
        INC     B
        INC     C
        JR      2$
1$:
        LD      A, (HL+)
        LD      (DE), A
        INC     DE
        LD      A, (HL+)
        LD      (DE), A
        INC     DE
2$:
        DEC     C
        JR      NZ,1$
        DEC     B
        JR      NZ,1$
4$:
        RET

        ;; Remove interrupt routine in DE from the VBL interrupt list
        ;; falldown to .remove_int
_remove_VBL::
.remove_VBL::
        LD      HL,#.int_0x40

        ;; Remove interrupt DE from interrupt list HL if it exists
        ;; Abort if a 0000 is found (end of list)
.remove_int::
1$:
        LD      A,(HL+)
        LD      C,A
        LD      A,(HL+)
        LD      B,A
        OR      C
        RET     Z               ; No interrupt found

        LD      A,E
        CP      C
        JR      NZ,1$
        LD      A,D
        CP      B
        JR      NZ,1$

        LD      B,H
        LD      C,L
        DEC     BC
        DEC     BC

        ;; Now do a memcpy from here until the end of the list
2$:
        LD      A,(HL+)
        LD      (BC),A
        INC     BC
        LD      D,A
        LD      A,(HL+)
        LD      (BC),A
        INC     BC
        OR      D
        JR      NZ, 2$
        RET

        ;; Add interrupt routine in BC to the VBL interrupt list
        ;; falldown to .add_int
_add_VBL::
.add_VBL::
        LD      HL,#.int_0x40

        ;; Add interrupt routine in BC to the interrupt list in HL
.add_int::
1$:
        LD      A,(HL+)
        OR      (HL)
        JR      Z,2$
        INC     HL
        JR      1$
2$:
        LD      A,D
        LD      (HL-),A
        LD      (HL),E
        RET

        ;; Wait for VBL interrupt to be finished
.wait_vbl_done::
_wait_vbl_done::
_vsync::
        ;; Check if the screen is on
        LDH     A,(.LCDC)
        AND     #LCDCF_ON
        RET     Z               ; Return if screen is off
        XOR     A
        LDH     (.vbl_done),A   ; Clear any previous sets of vbl_done
1$:
        HALT                    ; Wait for any interrupt
        NOP                     ; HALT sometimes skips the next instruction
        LDH     A,(.vbl_done)   ; Was it a VBlank interrupt?
        ;; Warning: we may lose a VBlank interrupt, if it occurs now
        OR      A
        JR      Z,1$            ; No: back to sleep!
        RET

.display_off::
_display_off::
        ;; Check if the screen is on
        LDH     A,(.LCDC)
        AND     #LCDCF_ON
        RET     Z               ; Return if screen is off
1$:                             ; We wait for the *NEXT* VBL
        LDH     A,(.LY)
        CP      #0x92           ; Smaller than or equal to 0x91?
        JR      NC,1$           ; Loop until smaller than or equal to 0x91
2$:
        LDH     A,(.LY)
        CP      #0x91           ; Bigger than 0x90?
        JR      C,2$            ; Loop until bigger than 0x90

        LDH     A,(.LCDC)
        AND     #~LCDCF_ON
        LDH     (.LCDC),A       ; Turn off screen
        RET
