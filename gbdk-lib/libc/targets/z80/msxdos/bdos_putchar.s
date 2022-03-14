        .include        "global.s"

        .title  "putchar"
        .module putchar

        .area   _CODE

_setchar::
_putchar::
        pop hl
        pop de
        push de         ; char in E
        push hl
        JP_BDOS #_CONOUT
	