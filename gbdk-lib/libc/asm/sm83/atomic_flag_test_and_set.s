;--------------------------------------------------------------------------
;  atomic_flag_test_and_set.s
;
;--------------------------------------------------------------------------

        .module atomic

        .area   _HOME

        .globl _atomic_flag_test_and_set

_atomic_flag_test_and_set:
        ld h, d
        ld l, e
        xor a
        sra (hl)
        ccf
        adc a
        ret

