;
; Emulation of GB's timer interrupt (TIM) via calls from NES's 60Hz / 50Hz vblank NMI
;
; Timer values are based on GBC running in double-speed mode
;

.include    "global.s"

.area _DATA
_TIMA_REG::                             .ds 2
_TMA_REG::                              .ds 1
_TAC_REG::                              .ds 1

.area _XINIT
.db 0x00
.db 0x00
.db .TIMER_VBLANK_PARITY_MODE_SYSTEM_60HZ
.db 0x04

.area   _HOME

; Lookup tables for adjusted rates based on GB TAC register
.TAC_lookup_lo:
; PAL
.db <163        ;    8192 / 50 =   163.84     ~=   164
.db <10486      ;  524288 / 50 = 10485.76     ~= 10486
.db <2621       ;  131072 / 50 =  2621.44     ~=  2621
.db <655        ;   32768 / 50 =   655.36     ~=   655
; NTSC
.db <136        ;    8192 / 60 =  136.5333... ~=   136
.db <8738       ;  524288 / 60 = 8738.1333... ~=  8378
.db <2184       ;  131072 / 60 = 2184.5333... ~=  2184
.db <546        ;   32768 / 60 =  546.1333... ~=   546
;
.TAC_lookup_hi:
; PAL
.db >163        ;    8192 / 50 =   163.84     ~=   164
.db >10486      ;  524288 / 50 = 10485.76     ~= 10486 
.db >2621       ;  131072 / 50 =  2621.44     ~=  2621 
.db >655        ;   32768 / 50 =   655.36     ~=   655
; NTSC
.db >136        ;    8192 / 60 =  136.5333... ~=   136
.db >8738       ;  524288 / 60 = 8738.1333... ~=  8378
.db >2184       ;  131072 / 60 = 2184.5333... ~=  2184
.db >546        ;   32768 / 60 =  546.1333... ~=   546

;
; Call timer interrupt repeatedly to emulate desired GB playback rate via vblank NMI.
;
.tim_emulation::
    lda _TAC_REG
    and #7
    ; Early-out if timer enabled bit not set
    cmp #4
    bcs 1$
    rts
1$:
    and #3
    bit *__SYSTEM
    bmi .tim_emulation_dendy
    bvs .tim_emulation_pal
    ; +4 to index NTSC table
    ora #0x04
.tim_emulation_dendy:
.tim_emulation_pal:
    pha     ; Save TAC lookup index to stack
.tim_emulation_loop:
    ; Finished when TIMA_REG <= 0
    bit _TIMA_REG+1
    bmi .tim_emulation_calls_done
    lda _TIMA_REG
    beq .tim_emulation_calls_done
    ; Skip if handler disabled (check for RTS)
    lda .jmp_to_TIM_isr
    cmp #0x60
    beq .tim_emulation_isr_disabled
    ; Save REGTEMP to stack
    ldx #17
4$:
    lda *ALL_REGTEMPS_BEGIN,x
    pha
    dex
    bpl 4$
    ; Call handler
    jsr .jmp_to_TIM_isr
    ; Restore REGTEMP from stack
    ldx #0
5$:
    pla
    sta *ALL_REGTEMPS_BEGIN,x
    inx
    cpx #18
    bne 5$
.tim_emulation_isr_disabled:
    ; TIMA_REG += TMA_REG
    lda _TIMA_REG
    clc
    adc _TMA_REG
    sta _TIMA_REG
    lda _TIMA_REG+1
    adc #0xFF
    sta _TIMA_REG+1
    jmp .tim_emulation_loop
;
.tim_emulation_calls_done:
    pla     ; Restore TAC lookup index from stack
    tay
    ; TIMA_REG += initial value
    lda _TIMA_REG
    clc
    adc .TAC_lookup_lo,y
    sta _TIMA_REG
    lda _TIMA_REG+1
    adc .TAC_lookup_hi,y
    sta _TIMA_REG+1
    rts
