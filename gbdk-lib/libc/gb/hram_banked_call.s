	.include	"global.s"

	;; ****************************************
	;; Beginning of module
	;; BANKED: checked
	.title	"HRAM_banked_call"
	.module	HRAM_banked_call


	.globl  .hram_tramp
	.globl	__tramp_proc
	.globl	__tramp_bank

	.globl	banked_call
	.globl	banked_ret
	.globl  __banked_call	
	.globl	__banked_ret
	.globl	__hram_banked_call

	.area	_GSINIT

	call	.create_hram_trampoline

	.area	_HOME
    
__hram_banked_call::
	pop	de      
	ldh	a,(__current_bank)
	push	af		; push the current bank onto the stack

	push	de
    	
	ldh	a, (__tramp_bank)
	ldh	(__current_bank),a
	ld	(.MBC1_ROM_PAGE),a	; Perform the switch

	ld	hl,#banked_ret	; push the fake return address
	push	hl
	ldh	a,(__tramp_proc)
	ld	l,a
	ldh     a,(__tramp_proc+1)
	ld	h,a
	jp	(hl)

.create_hram_trampoline:
	;; Create hirem_trampoline stub in HIRAM
	ld	a, #0xC3
	ldh	(#.hram_tramp), a
	ld	A, #<__hram_banked_call
	ldh	(#.hram_tramp+1), a
	ld	A, #>__hram_banked_call
	ldh	(#.hram_tramp+2), a
	ret

	.area	_HRAM (ABS)

	.org	0xFF91
.hram_tramp::
	.ds	0x03	; call
__tramp_proc::
	.ds	0x02
__tramp_bank::
	.ds	0x02
