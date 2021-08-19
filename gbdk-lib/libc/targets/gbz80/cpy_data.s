        .include	"global.s"

        ;; BANKED: checked
        .area	_BASE
        ;; Copy part (size = DE) of the VRAM from (BC) to (HL)
.copy_vram::
        INC     D
        INC     E
        JR      2$
1$:
        WAIT_STAT

        LD	A,(BC)
        LD	(HL+),A
        INC	BC
2$:
        DEC     E
        JR      NZ, 1$
        DEC     D
        JR      NZ, 1$
        RET

_vmemcpy::
_set_data::
_get_data::
        PUSH	BC

        LDA	HL,9(SP)	; Skip return address and registers
        LD	A,(HL-)		; DE = len
        LD	D, A
        LD	A,(HL-)
        LD	E, A
        LD	A,(HL-)		; BC = src
        LD	B, A
        LD	A,(HL-)
        LD	C, A
        LD	A,(HL-)		; HL = dst
        LD	L,(HL)
        LD	H,A

        CALL	.copy_vram

        POP	BC
        RET
