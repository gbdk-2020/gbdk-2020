        ;; SMS palette routines

        .include        "global.s"

        .title  "CRAM SMS Palettes"
        .module CRAMUtils

        .globl  .CRT_DEFAULT_PALETTE, _set_palette

        .area   _HOME

_set_default_palette::
        ld hl, #.CRT_DEFAULT_PALETTE
        push hl
        ld hl, #0x0100
        push hl
        call _set_palette
        ld hl, #.CRT_DEFAULT_PALETTE
        push hl
        ld hl, #0x0101
        push hl
        call _set_palette
        ret
