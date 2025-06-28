        .include "platform_cfg.s"

        ;; Maximum number of times LCD ISR can be repeatedly called
        .MAX_LCD_ISR_CALLS = 4
        ;; Total number is +1 to support VBL ISR with the same logic
        .MAX_DEFERRED_ISR_CALLS = (.MAX_LCD_ISR_CALLS+1)

        ;; Transfer buffer (lower half of hardware stack)
        __vram_transfer_buffer = 0x100
        ;; Number of 8-cycles available each frame for transfer buffer
        VRAM_DELAY_CYCLES_X8  = 171

        ; Bits for quick checking of OAM validity and display ON/OFF
        OAM_VALID_MASK      = 0x80
        DISPLAY_OFF_MASK    = 0x40

        ;;  Keypad
        .UP             = 0x08
        .DOWN           = 0x04
        .LEFT           = 0x02
        .RIGHT          = 0x01
        .A              = 0x80
        .B              = 0x40
        .SELECT         = 0x20
        .START          = 0x10

;;  Screen dimensions (in tiles)
        .DEVICE_SCREEN_WIDTH            = 32
        .DEVICE_SCREEN_HEIGHT           = 30

;;  Buffer dimensions (in tiles)
;;  Dependent on tilemap layout
.ifdef NES_TILEMAP_F
        NUM_NT                          = 4
        NT_2W                           = 1
        NT_2H                           = 1
        AT_SHADOW_WIDTH                 = 16
        AT_SHADOW_HEIGHT                = 16
        .DEVICE_SCREEN_BUFFER_WIDTH     = 64
        .DEVICE_SCREEN_BUFFER_HEIGHT    = 60
.endif
.ifdef NES_TILEMAP_H
        NUM_NT                          = 2
        NT_2W                           = 1
        NT_2H                           = 0
        AT_SHADOW_WIDTH                 = 16
        AT_SHADOW_HEIGHT                = 8
        .DEVICE_SCREEN_BUFFER_WIDTH     = 64
        .DEVICE_SCREEN_BUFFER_HEIGHT    = 30
.endif
.ifdef NES_TILEMAP_V
        NUM_NT                          = 2
        NT_2W                           = 0
        NT_2H                           = 1
        AT_SHADOW_WIDTH                 = 8
        AT_SHADOW_HEIGHT                = 16
        .DEVICE_SCREEN_BUFFER_WIDTH     = 32
        .DEVICE_SCREEN_BUFFER_HEIGHT    = 60
.endif
.ifdef NES_TILEMAP_S
        NUM_NT                          = 1
        NT_2W                           = 0
        NT_2H                           = 0
        AT_SHADOW_WIDTH                 = 8
        AT_SHADOW_HEIGHT                = 8
        .DEVICE_SCREEN_BUFFER_WIDTH     = 32
        .DEVICE_SCREEN_BUFFER_HEIGHT    = 30
.endif

        .MAXCURSPOSX    = 31
        .MAXCURSPOSY    = 29

        .SCREENWIDTH    = 256
        .SCREENHEIGHT   = 240

        ;; NAMETABLES
        PPU_NT0         = 0x2000
        PPU_AT0         = 0x23C0
        PPU_NT1         = 0x2400
        PPU_AT1         = 0x27C0
        PPU_NT2         = 0x2800
        PPU_AT2         = 0x2BC0
        PPU_NT3         = 0x2C00
        PPU_AT3         = 0x2FC0

        NT_WIDTH                       = 32
        NT_HEIGHT                      = 30
        AT_WIDTH                       = 8
        AT_HEIGHT                      = 8

        ATTRIBUTE_WIDTH                = 16
        ATTRIBUTE_HEIGHT               = 15
        ATTRIBUTE_PACKED_WIDTH         = 8
        ATTRIBUTE_PACKED_HEIGHT        = 8
        ATTRIBUTE_MASK_TL = 0b00000011
        ATTRIBUTE_MASK_TR = 0b00001100
        ATTRIBUTE_MASK_BL = 0b00110000
        ATTRIBUTE_MASK_BR = 0b11000000

        ;; Hardware registers / masks
        PPUCTRL             = 0x2000
        PPUCTRL_NMI         = 0b10000000
        PPUCTRL_SPR_8X8     = 0b00000000
        PPUCTRL_SPR_8X16    = 0b00100000
        PPUCTRL_BG_CHR      = 0b00010000
        PPUCTRL_SPR_CHR     = 0b00001000
        PPUCTRL_INC32       = 0b00000100
        ;
        PPUMASK             = 0x2001
        PPUMASK_BLUE        = 0b10000000
        PPUMASK_RED         = 0b01000000
        PPUMASK_GREEN       = 0b00100000
        PPUMASK_SHOW_SPR    = 0b00010000
        PPUMASK_SHOW_BG     = 0b00001000
        PPUMASK_SHOW_SPR_LC = 0b00000100
        PPUMASK_SHOW_BG_LC  = 0b00000010
        PPUMASK_MONOCHROME  = 0b00000001
        ;
        PPUSTATUS       = 0x2002
        ;
        OAMADDR         = 0x2003
        ;
        OAMDATA         = 0x2004
        ;
        PPUSCROLL       = 0x2005
        ;
        PPUADDR         = 0x2006
        ;
        PPUDATA         = 0x2007
        ;
        OAMDMA          = 0x4014
        ;
        JOY1            = 0x4016
        ;
        JOY2            = 0x4017

        ; Interrupt handler flags
        .VBL_IFLAG      = 0x01
        .LCD_IFLAG      = 0x02
        .TIM_IFLAG      = 0x04

        ;; OAM related constants

        OAM_COUNT       = 64  ; number of OAM entries in OAM RAM

        OAM_POS_Y       = 0
        OAM_TILE_INDEX  = 1
        OAM_ATTRIBUTES  = 2
        OAM_POS_X       = 3

        OAMF_PRI        = 0b00100000 ; Priority
        OAMF_YFLIP      = 0b10000000 ; Y flip
        OAMF_XFLIP      = 0b01000000 ; X flip

        ;; GBDK library screen modes

        .G_MODE         = 0x01  ; Graphic mode
        .T_MODE         = 0x02  ; Text mode (bit 2)
        .T_MODE_OUT     = 0x02  ; Text mode output only
        .T_MODE_INOUT   = 0x03  ; Text mode with input
        .M_NO_SCROLL    = 0x04  ; Disables scrolling of the screen in text mode
        .M_NO_INTERP    = 0x08  ; Disables special character interpretation

        ;; Table of routines for modes
        .MODE_TABLE     = 0xFFE0

        ;; Values for vblank parity mode when using timer emulation
        .TIMER_VBLANK_PARITY_MODE_SYSTEM_60HZ = 0x78
        .TIMER_VBLANK_PARITY_MODE_SYSTEM_50HZ = 0x5D

        ;; C related
        ;; Overheap of a banked call.  Used for parameters
        ;;  = ret + real ret + bank

        .BANKOV         = 6

        .globl  __current_bank
        .globl  __shadow_OAM_base

        ;; Global variables
        .globl  .mode
        .define .tmp "REGTEMP"

        .globl _shadow_PPUCTRL, _shadow_PPUMASK
        .globl _bkg_scroll_x, _bkg_scroll_y
        .globl __crt0_paletteShadow
        .globl _attribute_shadow, _attribute_row_dirty
        
        ;; Identity table for register-to-register-adds and bankswitching
        .globl .identity, _identity

        ;; Global routines

        .globl  .display_off, .display_on
        .globl  .wait_vbl_done

        ;; Symbols defined at link time
        .globl _shadow_OAM, __vram_transfer_buffer
        .globl __lcd_isr_PPUCTRL, __lcd_isr_PPUMASK
        .globl __lcd_isr_scroll_x, __lcd_isr_scroll_y, __lcd_isr_ppuaddr_lo
        .globl __lcd_isr_delay_num_scanlines

        ;; Main user routine
        .globl  _main

.macro DIV_PART divident divisor ?lbl
        rol divident
        rol
        sec
        sbc divisor
        bcs lbl
        adc divisor
lbl:
.endm
.macro FAST_MOD8 divident divisor
        ; returns modulus in A
        .rept 8
                DIV_PART divident divisor
        .endm
.endm
