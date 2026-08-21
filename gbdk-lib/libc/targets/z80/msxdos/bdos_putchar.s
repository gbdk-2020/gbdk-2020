        .include        "global.s"

        .title  "putchar"
        .module putchar

        .area   _CODE

_setchar::
_putchar::
        ld e, a         ; char in E
        JP_BDOS #_CONOUT
	