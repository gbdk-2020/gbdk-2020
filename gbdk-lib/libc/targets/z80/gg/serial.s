.include "global.s"

.module serial
.area _HEADER_SIO (ABS)

.area _GSINIT

;; initialize SIO
    ; Configure parallel pins consistently as input/output
    LD A,#0xEC
    OUT (#.GG_EXT_CTL),A
    ; Init parallel data
    LD A,#0
    OUT (#.GG_EXT_7BIT),A
    ; Enable serial mode. NMI on received byte
    LD A, #0x38
    OUT (#.GG_SIO_CTL),A

.area _HOME

;; Serial interrupt
.serial_IO::
    ; Read and store received data byte from serial I/O
    IN A,(#.GG_SIO_RECV)
    LD (__io_recv_buf),A

    LD A,(__io_status) ; Get status

    CP #.IO_RECEIVING
    JR NZ, 1$

    ;; Receiving data
    LD A,(__io_recv_buf)
    LD (__io_in),A

2$:
    LD A,#.IO_IDLE
3$:
    ; Store status
    LD (__io_status),A
    ; Reply with RECEIVING byte
    LD A,#.DT_RECEIVING
    OUT (#.GG_SIO_SEND),A

4$:
    RET

1$:
    CP #.IO_SENDING
    JR NZ, 4$

    ; Send data - error if receiver's send was not DT_RECEIVING
    LD A,(__io_recv_buf)
    CP #.DT_RECEIVING
    JR Z, 20$
    LD A,#.IO_ERROR
    JR 30$

20$:
    LD A,#.IO_IDLE
30$:
    ; Store status
    LD (__io_status),A
    RET

    .area _DATA

__io_out::
    .ds 0x01 ; Byte to send
__io_in::
    .ds 0x01 ; Received byte
__io_status::
    .ds 0x01 ; Status of serial IO
__io_recv_buf::
    .ds 0x01 ; Byte read from .GG_SIO_RECV

.area _CODE

;; Send byte in __io_out to the serial port
.send_byte:
_send_byte::
    ; Wait for serial output register to be available
$0:
    IN A,(#.GG_SIO_CTL)
    BIT 0,A
    JR NZ, $0
    ; Set status = IO_SENDING 
    LD A,#.IO_SENDING
    LD (__io_status),A
    ; Send data byte
    LD A,(__io_out)
    OUT (#.GG_SIO_SEND),A
    RET

;; Receive byte from the serial port in __io_in
.receive_byte:
_receive_byte::
    ; Set status = IO_RECEIVING
    LD A,#.IO_RECEIVING
    LD (__io_status),A
    RET
