        .module Random

        .area   _DATA

___rand_seed::
.randval:
        .ds     0x02

        .area   _CODE

_rand::	
_randw::

        ld hl, (.randval)
        LD d, h
        ld e, l

        add hl, hl
        add hl, hl
        add hl, hl
        add hl, hl
        add hl, de
        ld de, #0x5C93
        add hl, de
        ld (.randval), hl

        ld a, h
        ld h, l
        ld l, a

        RET

_initrand::                     ; Non banked
.initrand::
        LD (.randval), HL
        RET
