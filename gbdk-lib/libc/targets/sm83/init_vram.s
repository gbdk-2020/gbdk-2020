	.include	"global.s"

	.area	_HOME

	.globl	.init_wtt, .init_btt, .init_vram


	;; Initialize window tile table
	;; void init_win(char c);
_init_win::
	PUSH	BC
	LDA	HL, 4(SP)
	LD	B, (HL)
	CALL	.init_wtt
	POP	BC
	RET
	
	;; Initialize background tile table
	;; void init_bkg(char c);
_init_bkg::
	PUSH	BC
	LDA	HL, 4(SP)
	LD	B, (HL)
	CALL	.init_btt
	POP	BC
	RET

	;; Fills the VRAM memory region s of size n with c 
	;; void _vmemset (void *s, char c, size_t n);
_vmemset::
	PUSH	BC
	LDA	HL, 8(SP)
	LD	A, (HL-)
	LD	D, A
	LD	A, (HL-)
	LD	E, A
	LD	A, (HL-)
	LD	B, A
	LD	A, (HL-)
	LD	L, (HL)
	LD	H, A
	CALL	.init_vram
	POP	BC
	RET