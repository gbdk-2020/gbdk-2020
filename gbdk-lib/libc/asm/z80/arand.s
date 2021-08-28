        .module Random

        .globl	.initrand
        .globl	_rand

        .area	_BSS
.start_arand_vars:

.randarr:
        .ds	55
.raxj:
        .ds	0x01
.raxk:
        .ds	0x01
        
.end_arand_vars:

        .area	_GSINIT

        ld hl, #.start_arand_vars
        ld de, #(.start_arand_vars + 1)
        ld (hl), #0
        ld bc, #(.end_arand_vars - .start_arand_vars)
        ldir

        .area	_CODE

_arand::                        ; Banked
        PUSH BC
        LD D, #0
        LD HL, #(.randarr - 1)
        LD A, (.raxj)
        LD E, A
        DEC A		        ; Decrease the pointer
        JR NZ, 1$
        LD A, #55
1$:
        LD (.raxj), A
        ADD HL, DE
        LD B, (HL)

        LD HL, #(.randarr - 1)  ; Ooh...
        LD A, (.raxk)
        LD E, A
        DEC A                   ; Decrease the pointer
        JR NZ, 2$
        LD A, #55
2$:
        LD (.raxk), A
        ADD HL, DE
        LD A, (HL)

        ADD B
        LD (HL), A              ; Store new value

        POP BC

        LD H, #0
        LD L, A

        RET

_initarand::                    ; Banked
        CALL .initrand

        LD A, #55
        LD HL, #.randarr
1$:
        DEC A
        LD (.raxj), A
        LD B, H
        LD C, L
        CALL _rand
        LD H, B
        LD L, C

        LD (HL), E
        INC HL
        
        LD A, (.raxj)
        CP #0
        JP NZ, 1$

        LD A, #24               ; Now the array has been filled,set the pointers
        LD (.raxj), A
        LD A, #55
        LD (.raxk), A

        RET
