        .include        "global.s"

        .title  "getchar"
        .module getchar

        .area   _CODE

_getchar::
        JP_BDOS #_INNOE
	