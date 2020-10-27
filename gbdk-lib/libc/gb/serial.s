	.include	"global.s"

	.globl	.int

	.globl	__io_out
	.globl	__io_in
	.globl	__io_status

	.area	_HEADER_SIO (ABS)

	.org	0x58		; SIO
.int_SIO:
	PUSH	AF
	PUSH	HL
	LD	HL,#.int_0x58
	JP	.int

	.area	_GSINIT

	;; Clear SIO global variables
	XOR	A
	LD	HL,#.start_sio_globals
	LD 	C,#(.end_sio_globals - .start_sio_globals)
	RST	0x28

	;; initialize SIO
	LD	BC,#.serial_IO
	CALL	.add_SIO
	
	XOR	A
	LDH	(.IF),A
	
	LDH	A,(.IE)
	OR	A,#0b00001000	; Serial I/O	=   On
	LDH	(.IE),A

	LDH	(.SC),A		; Use external clock
	LD	A,#.DT_IDLE
	LDH	(.SB),A		; Send IDLE byte
	LD	A,#0x80
	LDH	(.SC),A		; Use external clock

	.area	_BASE

_add_SIO::
	PUSH	BC
	LDA	HL,4(SP)	; Skip return address and registers
	LD	C,(HL)
	INC	HL
	LD	B,(HL)
	CALL	.add_SIO
	POP	BC
	RET

.add_SIO::
	LD	HL,#.int_0x58
	JP	.add_int

_remove_SIO::
	PUSH	BC
	LDA	HL,4(SP)	; Skip return address and registers
	LD	C,(HL)
	INC	HL
	LD	B,(HL)
	CALL	.remove_SIO
	POP	BC
	RET

.remove_SIO::
	LD	HL,#.int_0x58
	JP	.remove_int

	;; Serial interrupt
.serial_IO::
	LD	A,(__io_status) ; Get status

	CP	#.IO_RECEIVING
	JR	NZ, 1$

	;; Receiving data
	LDH	A,(.SB)		; Get data byte
	LD	(__io_in),A	; Store it

2$:
	LD	A,#.IO_IDLE
3$:
	LD	(__io_status),A ; Store status

	XOR	A
	LDH	(.SC),A		; Use external clock
	LD	A,#.DT_IDLE
	LDH	(.SB),A		; Reply with IDLE byte
4$:
	LD	A,#0x80
	LDH	(.SC),A		; Enable transfer with external clock
	RET

1$:
	CP	#.IO_SENDING
	JR	NZ, 4$

	;; Sending data
	LDH	A,(.SB)		; Get data byte
	CP	#.DT_RECEIVING
	JR	Z, 2$
	LD	A,#.IO_ERROR
	JR	3$

	.area	_BSS
.start_sio_globals:

__io_out::
	.ds	0x01		; Byte to send
__io_in::
	.ds	0x01		; Received byte
__io_status::
	.ds	0x01		; Status of serial IO
.int_0x58::
	.blkw	0x08

.end_sio_globals:

	.area	_CODE

	;; Send byte in __io_out to the serial port
.send_byte:
_send_byte::			; Banked
	LD	A,#.IO_SENDING
	LD	(__io_status),A ; Store status
	LD	A,#0x01
	LDH	(.SC),A		; Use internal clock
	LD	A,(__io_out)
	LDH	(.SB),A		; Send data byte
	LD	A,#0x81
	LDH	(.SC),A		; Use internal clock
	RET

	;; Receive byte from the serial port in __io_in
.receive_byte:
_receive_byte::			; Banked
	LD	A,#.IO_RECEIVING
	LD	(__io_status),A ; Store status
	XOR	A
	LDH	(.SC),A		; Use external clock
	LD	A,#.DT_RECEIVING
	LDH	(.SB),A		; Send RECEIVING byte
	LD	A,#0x80
	LDH	(.SC),A		; Use external clock
	RET
