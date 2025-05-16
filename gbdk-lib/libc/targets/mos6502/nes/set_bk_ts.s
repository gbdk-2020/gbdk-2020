    .include    "global.s"

    ; NOTE: This overlay arrangement MUST match that of set_bkg_based_submap
    .area   GBDKOVR (PAG, OVR)
    _set_bkg_tiles_PARM_3::
    _set_bkg_based_tiles_PARM_3::   .ds 1
    _set_bkg_tiles_PARM_4::
    _set_bkg_based_tiles_PARM_4::   .ds 1
    _set_bkg_tiles_PARM_5::
    _set_bkg_based_tiles_PARM_5::   .ds 2
    _set_bkg_based_tiles_PARM_6::   .ds 1
    .padding::                      .ds 1
    .xpos:                          .ds 1
    .ypos:                          .ds 1
    .num_rows:                      .ds 1
    .src_tiles:                     .ds 2
    .remainder:                     .ds 1
    .ppuhi:                         .ds 1

    .define .width          "_set_bkg_submap_PARM_3"
    .define .height         "_set_bkg_submap_PARM_4"
    .define .tiles          "_set_bkg_submap_PARM_5"
    .define .map_width      "_set_bkg_submap_PARM_6"
    .define .tile_offset    "_set_bkg_based_submap_PARM_7"

    .area   _HOME

_set_bkg_tiles::
    ldy #0
    sty *_set_bkg_based_tiles_PARM_6
_set_bkg_based_tiles::
    sta *.xpos
    stx *.ypos
    lda *_set_bkg_based_tiles_PARM_6
    sta *.tile_offset
    lda *.width
    sta *.map_width
    lda *.tiles
    sta *.src_tiles
    lda *.tiles+1
    sta *.src_tiles+1
    jmp .set_bkg_common
