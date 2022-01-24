        .module Random

        .area   _DATA

___rand_seed::
.randval:
        .ds     0x02

        .area   _CODE

_rand::	
_randw::

        LD HL, (.randval)
        EX DE, HL
        LD L, E
        LD A, D

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
        LD (.randval), HL
        LD H, L
        LD L, A

        RET

_initrand::                     ; Non banked
.initrand::
        LD (.randval), HL
        RET
