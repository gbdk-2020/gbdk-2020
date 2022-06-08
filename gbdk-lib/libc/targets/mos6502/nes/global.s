        .NEAR_CALLS = 1         ; <near_calls> - tag so that sed can change this

        ;;  Keypad
        .UP             = 0x10
        .DOWN           = 0x20
        .LEFT           = 0x40
        .RIGHT          = 0x80
        .A              = 0x01
        .B              = 0x02
        .SELECT         = 0x04
        .START          = 0x08

        ;;  Screen dimensions (in tiles)
        .MAXCURSPOSX    = 31
        .MAXCURSPOSY    = 29

        .SCREENWIDTH    = 256
        .SCREENHEIGHT   = 240

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

        ;; OAM related constants

        OAM_COUNT       = 64  ; number of OAM entries in OAM RAM

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

        ;; C related
        ;; Overheap of a banked call.  Used for parameters
        ;;  = ret + real ret + bank

        .if .NEAR_CALLS
        .BANKOV         = 2

        .else
        .BANKOV         = 6

        .endif

        .globl  __current_bank
        .globl  __shadow_OAM_base

        ;; Global variables
        .globl  .mode
        .globl  .tmp

        .globl _shadow_PPUCTRL, _shadow_PPUMASK
        .globl _bkg_scroll_x, _bkg_scroll_y
        
        ;; Identity table for register-to-register-adds and bankswitching
        .globl .identity, _identity

        ;; Global routines

        .globl  .display_off, .display_on
        .globl  .wait_vbl_done
        .globl  .writeNametableByte

        ;; Symbols defined at link time
        .globl _shadow_OAM, _vram_transfer_buffer

        ;; Main user routine
        .globl  _main
