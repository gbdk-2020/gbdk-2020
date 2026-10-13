.include "global.s"

    .area   _HOME

__show_window::
.ifdef NES_WINDOW_LAYER
    lda *__oam_valid_display_on
    ora #WINDOW_ON_MASK
    sta *__oam_valid_display_on
.endif
    rts

__hide_window::
.ifdef NES_WINDOW_LAYER
    lda *__oam_valid_display_on
    and #~WINDOW_ON_MASK
    sta *__oam_valid_display_on
.endif
    rts
