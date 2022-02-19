        .BDOS           = 0x0005

        ; MSX-DOS 1
        _TERM0          = 0x00  ; Program terminate
        _CONIN          = 0x01  ; Console input
        _CONOUT         = 0x02  ; Console output
        _AUXIN          = 0x03  ; Auxiliary input
        _AUXOUT         = 0x04  ; Auxiliary output
        _LSTOUT         = 0x05  ; Printer output
        _DIRIO          = 0x06  ; Direct console I/O
        _DIRIN          = 0x07  ; Direct console input
        _INNOE          = 0x08  ; Console input without echo
        _STROUT         = 0x09  ; String output
        _BUFIN          = 0x0A  ; Buffered line input
        _CONST          = 0x0B  ; Console status
        _CPMVER         = 0x0C  ; Return version number
        _DSKRST         = 0x0D  ; Disk reset
        _SELDSK         = 0x0E  ; Select disk
        _FOPEN          = 0x0F  ; Open file (FCB)
        _FCLOSE         = 0x10  ; Close file (FCB)
        _SFIRST         = 0x11  ; Search for first entry (FCB)
        _SNEXT          = 0x12  ; Search for next entry (FCB)
        _FDEL           = 0x13  ; Delete file (FCB)
        _RDSEQ          = 0x14  ; Sequential read (FCB)
        _WRSEQ          = 0x15  ; Sequential write (FCB)
        _FMAKE          = 0x16  ; Create file (FCB)
        _FREN           = 0x17  ; Rename file (FCB)
        _LOGIN          = 0x18  ; Get login vector
        _CURDRV         = 0x19  ; Get current drive
        _SETDTA         = 0x1A  ; Set disk transfer address
        _ALLOC          = 0x1B  ; Get allocation information
        _RDRND          = 0x21  ; Random read (FCB)
        _WRRND          = 0x22  ; Random write (FCB)
        _FSIZE          = 0x23  ; Get file size (FCB)
        _SETRND         = 0x24  ; Set random record (FCB)
        _WRBLK          = 0x26  ; Random block write (FCB)
        _RDBLK          = 0x27  ; Random block read (FCB)
        _WRZER          = 0x28  ; Random write with zero fill (FCB)
        _GDATE          = 0x2A  ; Get date
        _SDATE          = 0x2B  ; Set date
        _GTIME          = 0x2C  ; Get time
        _STIME          = 0x2D  ; Set time
        _VERIFY         = 0x2E  ; Set/reset verify flag
        _RDABS          = 0x2F  ; Absolute sector read
        _WRABS          = 0x30  ; Absolute sector write

        ; MSX-DOS2
        _DPARM          = 0x31  ; Get disk parameters
        _FFIRST         = 0x40  ; Find first entry
        _FNEXT          = 0x41  ; Find next entry
        _FNEW           = 0x42  ; Find new entry
        _OPEN           = 0x43  ; Open file handle
        _CREATE         = 0x44  ; Create file handle
        _CLOSE          = 0x45  ; Close file handle
        _ENSURE         = 0x46  ; Ensure file handle
        _DUP            = 0x47  ; Duplicate file handle
        _READ           = 0x48  ; Read from file handle
        _WRITE          = 0x49  ; Write to file handle
        _SEEK           = 0x4A  ; Move file handle pointer
        _IOCTL          = 0x4B  ; I/O control for devices
        _HTEST          = 0x4C  ; Test file handle
        _DELETE         = 0x4D  ; Delete file or subdirectory
        _RENAME         = 0x4E  ; Rename file or subdirectory
        _MOVE           = 0x4F  ; Move file or subdirectory
        _ATTR           = 0x50  ; Get/set file attributes
        _FTIME          = 0x51  ; Get/set file date and time
        _HDELETE        = 0x52  ; Delete file handle
        _HRENAME        = 0x53  ; Rename file handle
        _HMOVE          = 0x54  ; Move file handle
        _HATTR          = 0x55  ; Get/set file handle attributes
        _HFTIME         = 0x56  ; Get/set file handle date and time
        _GETDTA         = 0x57  ; Get disk transfer address
        _GETVFY         = 0x58  ; Get verify flag setting
        _GETCD          = 0x59  ; Get current directory
        _CHDIR          = 0x5A  ; Change current directory
        _PARSE          = 0x5B  ; Parse pathname
        _PFILE          = 0x5C  ; Parse filename
        _CHKCHR         = 0x5D  ; Check character
        _WPATH          = 0x5E  ; Get whole path string
        _FLUSH          = 0x5F  ; Flush disk buffers
        _FORK           = 0x60  ; Fork a child process
        _JOIN           = 0x61  ; Rejoin parent process
        _TERM           = 0x62  ; Terminate with error code
        _DEFAB          = 0x63  ; Define abort exit routine
        _DEFER          = 0x64  ; Define disk error handler routine
        _ERROR          = 0x65  ; Get previous error code
        _EXPLAIN        = 0x66  ; Explain error code
        _FORMAT         = 0x67  ; Format a disk
        _RAMD           = 0x68  ; Create or destroy RAM disk
        _BUFFER         = 0x69  ; Allocate sector buffers
        _ASSIGN         = 0x6A  ; Logical drive assignment
        _GENV           = 0x6B  ; Get environment item
        _SENV           = 0x6C  ; Set environment item
        _FENV           = 0x6D  ; Find environment item
        _DSKCHK         = 0x6E  ; Get/set disk check status
        _DOSVER         = 0x6F  ; Get MSX-DOS version number
        _REDIR          = 0x70  ; Get/set redirection status

        ; MSX_DOS ERROR CODES
        __INTER         = 0xDF  ; Internal error
        _NORAM          = 0xDE  ; Not Enough Memory
        __IBDOS         = 0xDC  ; Invalid MSX-DOS Call  
        __IDRV          = 0xDB  ; Invalid drive
        __IFNM          = 0xDA  ; Invalid filename 
        __IPATH         = 0xD9  ; Invalid pathname 
        __PLONG         = 0xD8  ; Pathname too long
        __NOFIL         = 0xD7  ; File not found
        __NODIR         = 0xD6  ; Directory not found 
        __DRFUL         = 0xD5  ; Root directory full
        __DKFUL         = 0xD4  ; Disk full
        __DUPF          = 0xD3  ; Duplicate filename 
        __DIRE          = 0xD2  ; Invalid directory move
        __FILRO         = 0xD1  ; Read only file
        __DINE          = 0xD0  ; Directory not empty
        __IATTR         = 0xCF  ; Invalid attributes
        __DOT           = 0xCE  ; Invalid . or .. operation
        __SYSX          = 0xCD  ; System file exists
        __DIRX          = 0xCC  ; Directory exists
        __FILEX         = 0xCB  ; File exists
        __FOPEN         = 0xCA  ; File already in use 
        __OV64K         = 0xC9  ; Cannot transfer above 64K
        __FILE          = 0xC8  ; File allocation error
        __EOF           = 0xC7  ; End of file 
        __ACCV          = 0xC6  ; File access violation 
        __IPROC         = 0xC5  ; Invalid process id 
        __NHAND         = 0xC4  ; No spare file handles
        __IHAND         = 0xC3  ; Invalid file handle
        __NOPEN         = 0xC2  ; File handle not open
        __IDEV          = 0xC1  ; Invalid device operation
        __IENV          = 0xC0  ; Invalid environment string
        __ELONG         = 0xBF  ; Environment string too long
        __IDATE         = 0xBE  ; Invalid date
        _ITIME          = 0xBD  ; Invalid time
        __RAMDX         = 0xBC  ; RAM disk (drive H:) already exists
        __NRAMD         = 0xBB  ; RAM disk does not exist
        __HDEAD         = 0xBA  ; File handle has been deleted
        __ISBFN         = 0xB8  ; Invalid sub-function number

        ; LOW STORAGE
        __FILE_BUFFER   = 0x80
        .globl __FILE_BUFFER

        .MEMORY_CTL     = 0x3E

        .MEMCTL_JOYON   = 0b00000000
        .MEMCTL_JOYOFF  = 0b00000100
        .MEMCTL_BASEON  = 0b00000000
        .MEMCTL_BASEOFF = 0b00001000
        .MEMCTL_RAMON   = 0b00000000
        .MEMCTL_RAMOFF  = 0b00010000
        .MEMCTL_CROMON  = 0b00000000
        .MEMCTL_CROMOFF = 0b00100000
        .MEMCTL_ROMON   = 0b00000000
        .MEMCTL_ROMOFF  = 0b01000000
        .MEMCTL_EXTON   = 0b00000000
        .MEMCTL_EXTOFF  = 0b10000000

        .JOY_CTL        = 0x3F

        .JOY_P1_LATCH   = 0b00000010
        .JOY_P2_LATCH   = 0b00001000

        .VDP_VRAM       = 0x4000
        .VDP_TILEMAP    = 0x7800
        .VDP_CRAM       = 0xC000
        .VDP_SAT        = 0x7F00

        .VDP_SAT_TERM   = 0xD0

        .VDP_VCOUNTER   = 0x7E
        .VDP_PSG        = 0x7F
        .VDP_HCOUNTER   = 0x7F

        .VDP_DATA       = 0xBE
        .VDP_CMD        = 0xBF
        .VDP_STAT       = 0xBF

        .STATF_INT_VBL  = 0b10000000
        .STATF_9_SPR    = 0b01000000
        .STATF_SPR_COLL = 0b00100000

        .VDP_REG_MASK   = 0b10000000
        .VDP_R0         = 0b10000000

        .R0_VSCRL       = 0b00000000
        .R0_VSCRL_INH   = 0b10000000
        .R0_HSCRL       = 0b00000000
        .R0_HSCRL_INH   = 0b01000000
        .R0_NO_LCB      = 0b00000000
        .R0_LCB         = 0b00100000
        .R0_IE1_OFF     = 0b00000000
        .R0_IE1         = 0b00010000
        .R0_SS_OFF      = 0b00000000
        .R0_SS          = 0b00001000
        .R0_DEFAULT     = 0b00000110
        .R0_ES_OFF      = 0b00000000
        .R0_ES          = 0b00000001

        .VDP_R1         = 0b10000001

        .R1_DEFAULT     = 0b10000000
        .R1_DISP_OFF    = 0b00000000
        .R1_DISP_ON     = 0b01000000
        .R1_IE_OFF      = 0b00000000
        .R1_IE          = 0b00100000
        .R1_SPR_8X8     = 0b00000000
        .R1_SPR_8X16    = 0b00000010

        .VDP_R2         = 0b10000010

        .R2_MAP_0x3800  = 0xFF
        .R2_MAP_0x3000  = 0xFD
        .R2_MAP_0x2800  = 0xFB
        .R2_MAP_0x2000  = 0xF9
        .R2_MAP_0x1800  = 0xF7
        .R2_MAP_0x1000  = 0xF5
        .R2_MAP_0x0800  = 0xF3
        .R2_MAP_0x0000  = 0xF1

        .VDP_R3         = 0b10000011
        .VDP_R4         = 0b10000100
        .VDP_R5         = 0b10000101

        .R5_SAT_0x3F00  = 0xFF
        .R5_SAT_MASK    = 0b10000001

        .VDP_R6         = 0b10000110

        .R6_BANK0       = 0xFB
        .R6_DATA_0x0000 = 0xFB
        .R6_BANK1       = 0xFF
        .R6_DATA_0x2000 = 0xFF

        .VDP_R7         = 0b10000111
        .VDP_RBORDER    = 0b10000111

        .R7_COLOR_MASK  = 0b11110000

        .VDP_R8         = 0b10001000
        .VDP_RSCX       = 0b10001000

        .VDP_R9         = 0b10001001
        .VDP_RSCY       = 0b10001001

        .VDP_R10        = 0b10001010

        .R10_INT_OFF    = 0xFF
        .R10_INT_EVERY  = 0x00

        .JOYPAD_COUNT   = 1

        .UP             = 0b00000001
        .DOWN           = 0b00000010
        .LEFT           = 0b00000100
        .RIGHT          = 0b00001000
        .A              = 0b00010000
        .B              = 0b00100000
        .SELECT         = 0b00100000    ; map to B
        .START          = 0b10000000    ; Game Gear Start button

        .JOY_PORT1      = 0xDC

        .JOY_P1_UP      = 0b00000001
        .JOY_P1_DOWN    = 0b00000010
        .JOY_P1_LEFT    = 0b00000100
        .JOY_P1_RIGHT   = 0b00001000
        .JOY_P1_SW1     = 0b00010000
        .JOY_P1_TRIGGER = 0b00010000
        .JOY_P1_SW2     = 0b00100000
        .JOY_P2_UP      = 0b01000000
        .JOY_P2_DOWN    = 0b10000000

        .JOY_PORT2      = 0xDD

        .JOY_P2_LEFT    = 0b00000001
        .JOY_P2_RIGHT   = 0b00000010
        .JOY_P2_SW1     = 0b00000100
        .JOY_P2_TRIGGER = 0b00000100
        .JOY_P2_SW2     = 0b00001000
        .JOY_RESET      = 0b00010000
        .JOY_P1_LIGHT   = 0b01000000
        .JOY_P2_LIGHT   = 0b10000000

        .FMADDRESS      = 0xF0
        .FMDATA         = 0xF1
        .AUDIOCTRL      = 0xF2

        .RAM_CONTROL    = 0xfffc

        .RAMCTL_BANK    = 0b00000100
        .RAMCTL_ROM     = 0b00000000
        .RAMCTL_RAM     = 0b00001000
        .RAMCTL_RO      = 0b00010000
        .RAMCTL_PROT    = 0b10000000

        .GLASSES_3D     = 0xfff8

        .MAP_FRAME0     = 0xfffd
        .MAP_FRAME1     = 0xfffe
        .MAP_FRAME2     = 0xffff

        .BIOS           = 0xC000

        .SYSTEM_PAL     = 0x00
        .SYSTEM_NTSC    = 0x01

        .CPU_CLOCK      = 3579545

        ;; GBDK library screen modes

        .T_MODE         = 0x02  ; Text mode (bit 2)
        .T_MODE_OUT     = 0x02  ; Text mode output only
        .T_MODE_INOUT   = 0x03  ; Text mode with input
        .M_NO_SCROLL    = 0x04  ; Disables scrolling of the screen in text mode
        .M_NO_INTERP    = 0x08  ; Disables special character interpretation

        ;; Screen dimentions in tiles

        .SCREEN_X_OFS   = 6
        .SCREEN_Y_OFS   = 3
        .SCREEN_WIDTH   = 20
        .SCREEN_HEIGHT  = 18
        .VDP_MAP_HEIGHT = 28
        .VDP_MAP_WIDTH  = 32

        ;; Interrupt flags

        .VBL_IFLAG      = 0x01
        .LCD_IFLAG      = 0x02

        ; characters
        .CR             = 0x0A
        .SPACE          = 0x00

        ;; Main user routine
        .globl  _main

        ;; Macro definitions

.macro CALL_BDOS fn
        ld c, fn
        call .BDOS
.endm

.macro JP_BDOS fn
        ld c, fn
        jp .BDOS
.endm

.macro SMS_WRITE_VDP_DATA regH regL ?lbl
        ld a, i
        ld a, regL
        di
        out (#.VDP_DATA), a     ; 11
        ld a, regH              ; 4
        jp po, lbl              ; 7/12
        ei                      ; 4 (total: 26/27)
lbl:
        out (#.VDP_DATA), a
.endm

.macro SMS_WRITE_VDP_CMD regH regL ?lbl
        ld a, i
        ld a, regL
        di
        out (#.VDP_CMD), a
        ld a, regH
        jp po, lbl
        ei
lbl:
        out (#.VDP_CMD), a
.endm

.macro VDP_DELAY ?lbl
        nop
        jr lbl
lbl:
.endm

.macro VDP_CANCEL_INT
        in a, (.VDP_STAT)       ; cancel pending VDP interrupts
.endm

.macro WRITE_VDP_CMD_HL
        SMS_WRITE_VDP_CMD h, l
.endm

.macro WRITE_VDP_DATA_HL
        SMS_WRITE_VDP_DATA h, l
.endm

.macro CALL_HL
        call ___sdcc_call_hl
.endm

.macro DISABLE_VBLANK_COPY
        ld a, #1
        ld (__shadow_OAM_OFF), a
.endm

.macro ENABLE_VBLANK_COPY
        xor a
        ld (__shadow_OAM_OFF), a
.endm

.macro ADD_A_REG16 regH regL
        add regL
        ld regL, a
        adc regH
        sub regL
        ld regH, a
.endm

.macro MUL_DE_BY_A_RET_HL ?lbl1 ?lbl2
        ; Multiply DE by A, return result in HL; preserves: BC
        ld hl, #0
lbl1:
        srl a
        jp nc, lbl2
        add hl, de
lbl2:
        sla e
        rl d
        or a
        jp nz, lbl1
.endm

.macro DIV_PART divident divisor ?lbl
        rl divident
        rla
        sub divisor
        jr  nc, lbl
        add divisor
lbl:
.endm
.macro FAST_DIV8 divident divisor
        ; returns modulus in A
        .rept 8
                DIV_PART divident divisor
        .endm
        ld a, divident
        cpl
.endm
.macro FAST_MOD8 divident divisor
        ; returns modulus in A
        .rept 8
                DIV_PART divident divisor
        .endm
.endm
