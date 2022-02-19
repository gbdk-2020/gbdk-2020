        .include        "global.s"

        .title  "putchar"
        .module putchar

        .area   _HOME

_setchar::
_putchar::
        pop hl
        pop de
        push de         ; char in E
        push hl
        JP_BDOS #_CONOUT
	