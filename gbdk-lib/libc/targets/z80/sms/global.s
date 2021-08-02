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

        .VCOUNTER       = 0x7E
        .PSG            = 0x7F
        .HCOUNTER       = 0x7F

        .VDP_DATA       = 0xBE
        .VDP_CMD        = 0xBF
        .VDP_STAT       = 0xBF
        
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
        
        .MAP_FRAME0     = 0xfffd
        .MAP_FRAME1     = 0xfffe
        .MAP_FRAME2     = 0xffff

        .BIOS           = 0xC000
        
        ;; Interrupt routines 
        .globl _INT_ISR
        .globl _NMI_ISR

        ;; Symbols defined at link time
        .globl  .STACK
        .globl  _shadow_OAM     ; temporary
        .globl  .refresh_OAM    ; temporary
        
        ;; Main user routine    
        .globl  _main

        ;; Macro definitions

.macro SMS_WRITE_VDP_DATA
        rst 0x18
.endm

.macro CALL_HL
        rst 0x20
.endm

.macro SMS_WRITE_VDP_CMD
        rst 0x28
.endm
