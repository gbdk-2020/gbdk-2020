; Crash handler support
; Original code by ISSOtm
; Adapted by Toxa from gb-starter-kit: https://github.com/ISSOtm/gb-starter-kit

	.include	"global.s"

	.globl	_font_ibm

	SCRN_X = 160		; Width of screen in pixels
	SCRN_Y = 144		; Height of screen in pixels
	SCRN_X_B = 20		; Width of screen in bytes
	SCRN_Y_B = 18		; Height of screen in bytes

	SCRN_VX = 256		; Virtual width of screen in pixels
	SCRN_VY = 256		; Virtual height of screen in pixels
	SCRN_VX_B = 32		; Virtual width of screen in bytes
	SCRN_VY_B = 32		; Virtual height of screen in bytes


	.area _CRASH_HEADER(ABS)

	.org	0x00
	nop
	nop
	rst	0x38

	.org	0x38
	di
	jp	___HandleCrash


	.area _HOME
	
___HandleCrash::

	; We will use VRAM as scratch, since we are going to overwrite it for
	; screen output anyways. The thing is, we need to turn the LCD off
	; *without* affecting flags... fun task, eh?

	; Note: it's assumed that this was jumped to with IME off.
	; Don't call this directly, use `rst Crash`.

	ld	(wCrashA), a	; We need to have at least one working register, so...
	ldh 	a, (.IE)	; We're also going to overwrite this
	ld 	(wCrashIE), a
	ldh 	a, (.LCDC)
	ld 	(wCrashLCDC), a
	ld 	a, #0b10000000	; LCDCF_ON Make sure the LCD is turned on to avoid waiting infinitely
	ldh 	(.LCDC), a
	ld 	a, #0b00000001	; IEF_VBLANK
	ld 	(.IE), a
	ld 	a, #0		; `xor a` would overwrite flags
	ld 	(.IF), a	; No point in backing up that register, it's always changing
	halt			; With interrupts disabled, this will exit when `IE & IF != 0`
	nop			; Handle hardware bug if it becomes true *before* starting to execute the instruction (1-cycle window)

	; We're now in VBlank! So we can now use VRAM as scratch for some cycles
	ld	a, #0
	ldh	(.LCDC), a ; Turn off LCD so VRAM can always be safely accessed
	; Save regs
	ld 	(vCrashSP), sp
	ld 	sp, #vCrashSP
	push	hl
	push	de
	push	bc
	ld	a, (wCrashA)
	push	af

	; We need to have all the data in bank 0, but we can't guarantee we were there
	ldh	a, (.VBK)
	ld	e, a
	bit	#0, a
	jr	z, .bank0
	; Oh noes. We need to copy the data across banks!
	ld	hl, #vCrashAF
	ld	c, #(5 * 2)
.copyAcross:
	ld 	b, (hl)
	xor	a
	ldh 	(.VBK), a
	ld 	(hl), b
	inc	l		; inc hl
	inc	a		; ld a, 1
	ldh 	(.VBK), a
	dec	c
	jr	nz, .copyAcross
.bank0:
	xor	a
	ldh	(.NR52), a	; Kill sound for this screen

	ldh	(.VBK), a
	ld	a, e
	ld	(vCrashVBK), a	; copy vCrashVBK across banks

	ld	a, #1
	ldh	(.VBK), a
	ld	hl, #vCrashDumpScreen
	ld	b, #SCRN_Y_B
.writeAttrRow:
	xor 	a
	ld 	c, #(SCRN_X_B + 1)
	rst	#0x28 		; .MemsetSmall
	ld 	a, l
	add 	a, #(SCRN_VX_B - SCRN_X_B - 1)
	ld 	l, a
	dec 	b
	jr 	nz, .writeAttrRow
	xor 	a
	ldh 	(.VBK), a

	; Load palettes
	ld 	a, #0x03
	ldh 	(.BGP), a
	ld 	a, #0x80
	ldh 	(.BCPS), a
	xor 	a
	ld 	c, #.BCPD
	ldh 	(c), a
	ldh 	(c), a
	dec 	a ; ld a, $FF
	ldh 	(c), a
	ldh 	(c), a
	ldh 	(c), a
	ldh 	(c), a
	ldh 	(c), a
	ldh 	(c), a

	ld	a, #(SCRN_VY - SCRN_Y)
	ldh 	(.SCY), a
	ld	a, #(SCRN_VX - SCRN_X - 4)
	ldh	(.SCX), a

	call	loadfont

	; Copy the registers to the dump viewers
	ld	hl, #vDumpHL
	ld	de, #vCrashHL
	ld	c, #4
	rst	#0x30		; .MemcpySmall

	; We're now going to draw the screen, top to bottom
	ld	hl, #vCrashDumpScreen

	; First 3 lines of text
	ld	de, #.header
	ld	b, #3
.writeHeaderLine:
	ld	a, #0x20	; " "
	ld	(hl+), a
	ld	c, #19
	rst	#0x30		; .MemcpySmall
	ld	a, #0x20	; " "
	ld	(hl+), a
	ld	a, l
	add	a, #(SCRN_VX_B - SCRN_X_B - 1)
	ld	l, a
	dec	b
	jr	nz, .writeHeaderLine

	; Blank line
	ld	a, #0x20	; " "
	ld	c, #(SCRN_X_B + 1)
	rst	#0x28		; .MemsetSmall

	; AF and console model
	ld	l, #<vCrashDumpScreenRow4
	ld	c, #4
	rst	#0x30		; .MemcpySmall
	pop	bc
	call	.printHexBC
	ld	c, #8
	rst	#0x30		; .MemcpySmall
	ld	a, (__cpu)
	call	.printHexA
	ld	a, #0x20	; " "
	ld	(hl+), a
	ld	(hl+), a
	ld	(hl+), a

	; BC and DE
	ld	l, #<vCrashDumpScreenRow5
	ld	c, #4
	rst	#0x30		; .MemcpySmall
	pop	bc
	call	.printHexBC
	ld	c, #6
	rst	#0x30		; .MemcpySmall
	pop	bc
	call	.printHexBC
	ld	a, #0x20
	ld	(hl+), a
	ld	(hl+), a
	ld	(hl+), a

	; Now, the two memory dumps
.writeDump:
	ld	a, l
	add	a, #(SCRN_VX_B - SCRN_X_B - 1)
	ld	l, a
	ld	c, #4
	rst	#0x30		; .MemcpySmall
	pop	bc
	push	bc
	call	.printHexBC
	ld	de, #.viewStr
	ld	c, #7
	rst	#0x30		; .MemcpySmall
	pop	de
	call	.printDump
	ld	de, #.spStr
	bit	#7, l
	jr	z, .writeDump

	ld	de, #.hwRegsStrs
	ld	l, #<vCrashDumpScreenRow14
	ld	c, #6
	rst	#0x30		; .MemcpySmall
	ld	a, (wCrashLCDC)
	call	.printHexA
	ld	c, #4
	rst	#0x30		; .MemcpySmall
	ldh	a, (.KEY1)
	call	.printHexA
	ld	c, #4
	rst	#0x30		; .MemcpySmall
	ld	a, (wCrashIE)
	call	.printHexA
	ld	(hl), #0x20	; " "

	ld	l, #<vCrashDumpScreenRow15
	ld	c, #7
	rst	#0x30		; .MemcpySmall
.writeBank:
	ld	a, #0x20	; " "
	ld	(hl+), a
	ld	a, (de)
	inc	de
	ld	(hl+), a
	cp	#0x20		; " "
	jr	z, .banksDone
	ld	a, (de)
	inc	de
	ld	c, a
	ld	a, (de)
	inc	de
	ld	b, a
	ld	a, (bc)
	call	.printHexA
	jr	.writeBank
.banksDone:

	; Start displaying
	ld	a, #0b10001001		; LCDCF_ON | LCDCF_BG9C00 | LCDCF_BGON
	ldh	(.LCDC), a

.loop:
	; The code never lags, and IE is equal to IEF_VBLANK
	xor a
	ldh (.IF), a
	halt

	jr	.loop

.printHexBC:
	call	.printHexB
	ld	a, c
.printHexA:
	ld	b, a
.printHexB:
	ld	a, b
	and	#0xF0
	swap	a
	add	a, #0x30
	cp	#0x3a
	jr	c, 1$
	add	a, #(0x41 - 0x3a)
1$:	ld	(hl+), a
	ld	a, b
	and	#0x0F
	add	a, #0x30
	cp	#0x3a
	jr	c, 2$
	add	a, #(0x41 - 0x3a)
2$:	ld	(hl+), a
	ret

.printDump:
	ld	b, d
	ld	c, e
	call	.printHexBC
	ld	a, #0x20	; " "
	ld	(hl+), a
	ld	(hl+), a
	ld	a, e
	sub	#8
	ld	e, a
	ld	a, d
	sbc	#0
	ld	d, a
.writeDumpLine:
	ld	a, l
	add	a, #(SCRN_VX_B - SCRN_X_B - 1)
	ld	l, a
	ld	a, #0x20	; " "
	ld	(hl+), a
.writeDumpWord:
	ld	a, (de)
	inc	de
	call	.printHexA
	ld	a, (de)
	inc	de
	call	.printHexA
	ld	a, #0x20	; " "
	ld	(hl+), a
	bit	4, l
	jr	nz, .writeDumpWord
	ld	a, l
	and	#0x7F
	jr	nz, .writeDumpLine
	ret

loadfont:
	xor	a
	cpl
	ld	hl, #0x9000
	ld	c, #16
	rst	#0x28 		; .MemsetSmall
	ld	hl, #(0x9000 + ' ' * 16)
	ld	c, #16
	rst	#0x28 		; .MemsetSmall

	ld	de, #(_font_ibm + 2 + '0')	; recode table
	ld	hl, #(0x9000 + '0' * 16)	; destination
	push 	hl
	ld	c, #(16 * 3)
1$:	
	ld	a, (de)
	inc	de
	push 	de
	
	swap	a
	ld	l, a
	and 	#0x0f
	ld	h, a
	ld	a, l
	and	#0xf0
	srl	h
	rra
	ld	l, a
	ld	de, #(_font_ibm + 2 + 128)
	add	hl, de
	
	ld	d, h
	ld	e, l
	
	ldhl	sp, #2
	ld	a, (hl+)
	ld	h, (hl)
	ld	l, a
	
	ld	b, #8
2$:
	ld	a, (de)
	cpl
	inc	de
	ld	(hl+), a
	ld	(hl+), a

	dec	b
	jr	nz, 2$
	
	ld	d, h
	ld	a, l
	ldhl	sp, #2
	ld	(hl+), a
	ld	(hl), d
	
	pop	de

	dec	c
	jr	nz, 1$
	
	add	sp, #2
	ret

.header:
	;   0123456789ABCDEFGHI  19 chars
	.ascii	"KERNEL PANIC PLEASE"
	.ascii	"SEND A CLEAR PIC OF"
	.ascii	"THIS SCREEN TO DEVS"
	.ascii	" AF:"
	.ascii	"  MODEL:"
	.ascii	" BC:"
	.ascii	"   DE:"
	.ascii	" HL:"
.viewStr:
	.ascii	"  VIEW:"
.spStr:
	.ascii	" SP:"
.hwRegsStrs:
	.ascii	" LCDC:"
	.ascii	" K1:"
	.ascii	" IE:"
	.ascii	"  BANK:"
	.ascii	"R"
	.dw	__current_bank 
	.ascii	"V" 
	.dw	vCrashVBK 
	.ascii	"W"
	.db	.SVBK, 0xff
	.ascii	" "


	.area _BSS

wCrashA: 
	.ds	1		; We need at least one working register, and A allows accessing memory
wCrashIE: 
	.ds	1
wCrashLCDC: 
	.ds	1


	.area _CRASH_SCRATCH(ABS)

	.org	0x9C00

	; Put the crash dump screen at the bottom-right of the 9C00 tilemap, since that tends to be unused space
	.ds	SCRN_VX_B * (SCRN_VY_B - SCRN_Y_B - 2)	; 2 rows reserved as scratch space

	.ds	SCRN_X_B	; Try not to overwrite the window area
	.ds	2 * 1		; Free stack entries (we spill into the above by 1 entry, though :/)
	; These are the initial values of the registers
	; They are popped off the stack when printed, freeing up stack space

vCrashAF: 
	.ds	2
vCrashBC: 
	.ds	2
vCrashDE: 
	.ds	2
vCrashHL: 
	.ds	2
vCrashSP: 
	.ds	2

	.ds	SCRN_X_B
vHeldKeys: 
	.ds	1		; Keys held on previous frame
vUnlockCounter: 
	.ds 	1		; How many frames until dumps are "unlocked"
vWhichDump: 
	.ds	1
vDumpHL: 
	.ds	2
vDumpSP: 
	.ds	2
vCrashVBK: 
	.ds	1
	.ds	4		; Unused

	.ds	SCRN_VX_B - SCRN_X_B - 1
vCrashDumpScreen:
vCrashDumpScreenRow0  = vCrashDumpScreen +  1 * SCRN_VX_B
vCrashDumpScreenRow1  = vCrashDumpScreen +  2 * SCRN_VX_B
vCrashDumpScreenRow2  = vCrashDumpScreen +  3 * SCRN_VX_B
vCrashDumpScreenRow3  = vCrashDumpScreen +  4 * SCRN_VX_B
vCrashDumpScreenRow4  = vCrashDumpScreen +  5 * SCRN_VX_B
vCrashDumpScreenRow5  = vCrashDumpScreen +  6 * SCRN_VX_B
vCrashDumpScreenRow6  = vCrashDumpScreen +  7 * SCRN_VX_B
vCrashDumpScreenRow7  = vCrashDumpScreen +  8 * SCRN_VX_B
vCrashDumpScreenRow8  = vCrashDumpScreen +  9 * SCRN_VX_B
vCrashDumpScreenRow9  = vCrashDumpScreen + 10 * SCRN_VX_B
vCrashDumpScreenRow10 = vCrashDumpScreen + 11 * SCRN_VX_B
vCrashDumpScreenRow11 = vCrashDumpScreen + 12 * SCRN_VX_B
vCrashDumpScreenRow12 = vCrashDumpScreen + 13 * SCRN_VX_B
vCrashDumpScreenRow13 = vCrashDumpScreen + 14 * SCRN_VX_B
vCrashDumpScreenRow14 = vCrashDumpScreen + 15 * SCRN_VX_B
vCrashDumpScreenRow15 = vCrashDumpScreen + 16 * SCRN_VX_B
vCrashDumpScreenRow16 = vCrashDumpScreen + 17 * SCRN_VX_B
vCrashDumpScreenRow17 = vCrashDumpScreen + 18 * SCRN_VX_B
