        .include        "global.s"

        .title  "memset small"
        .module MemSetSmall

        .area   _HOME

.memset_small::
1$:
        ld (hl), a
        inc hl
        dec c
        jp nz, 1$
        ret
