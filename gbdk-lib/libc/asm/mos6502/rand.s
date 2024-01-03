;
; Random number generation for mos6502.
;
; Re-implementation of rand / randw / inirand, originally from gbdk-lib/libc/asm/sm83/rand.s
;
; Also redirects arand / initarand to here, to avoid their extra memory cost.
;

.area _ZP (PAG)
___rand_seed::
.randlo: .ds 1
.randhi: .ds 1

.area _HOME

.define .tmp "REGTEMP"

;
; Returns an 8-bit / 16-bit rand value and updates the seed.
;
_arand::
_rand::
_randw::
    ; rand += 17 * rand
    lda *.randhi
    sta *.tmp
    lda *.randlo
    asl
    rol *.tmp
    asl
    rol *.tmp
    asl
    rol *.tmp
    asl
    rol *.tmp
    ;
    clc
    adc *.randlo
    sta *.randlo
    lda *.tmp
    adc *.randhi
    sta *.randhi
    ; rand += 0x5C93
    lda *.randlo
    clc
    adc #<0x5C93
    sta *.randlo
    tax
    lda *.randhi
    adc #>0x5C93
    sta *.randhi
    ; Return high byte of random number for 8-bit. Swapped low/high for 16-bit
    rts

;
; Sets the seed value.
;
_initarand::
_initrand::
.initrand::
    sta *.randlo
    stx *.randhi
    rts
