        .module Random

        .area   _DATA

.randval:
        .ds     0x02

        .area   _CODE

_rand::	
_randw::

        ld HL, (.randval)
        ex DE, HL
        ld L, E

        SLA L                   ; * 16
        RLA
        SLA L
        RLA
        SLA L
        RLA
        SLA L
        RLA
        LD H, A                 ; Save randhi*16

        LD A, E                 ; Old randlo
        ADD A, L                ; Add randlo*16
        LD L, A                 ; Save randlo*17

        LD A, H                 ; randhi*16
        ADC A, D                ; Add old randhi
        LD H, A                 ; Save randhi*17

        LD A, L                 ; randlo*17
        ADD A, #0x93
        LD L, A
        LD A, H                 ; randhi*17
        ADC A, #0x5c
        LD H, A
        ld (.randval), HL

        RET

_initrand::                     ; Non banked
.initrand::
        ld (.randval), HL
        ret
