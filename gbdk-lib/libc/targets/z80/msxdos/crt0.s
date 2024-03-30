        .include        "global.s"

        .title  "Runtime"
        .module Runtime

        .ez80

        ;; Ordering of segments for the linker.
        .area   _CODE
        .area   _CODE_0
        .area   _HOME
        .area   _BASE
        .area   _LIT
        .area   _GSINIT
        .area   _GSFINAL
        .area   _INITIALIZER

        .area   _DATA
        .area   _INITIALIZED
        .area   _BSEG
        .area   _BSS
        .area   _HEAP
        .area   _HEAP_END

        ; program startup code
        .area   _CODE
.start::
        ei
        call .save_int_vector
        call .gsinit

        ld a, (___overlay_count)
        and a
        call nz, .load_overlays
        jp c, .exit_error

        call .setup_video_mode

        ld hl, #.LS_FILE_BUFFER
        xor a
        ld b, a
        ld c, (hl)
        or c
        jr z, 1$

        add hl, bc
        inc hl
        xor a
        ld (hl), a
        ld c, a                 ; param counter

        ld hl, #(.LS_FILE_BUFFER + 1)
3$:
        ld a, (hl)
        or a
        jr z, 6$
        cp #0x20
        jr nz, 6$
        inc hl
        jr 3$
6$:
        jr z, 2$

        inc c
        push hl
4$:
        inc hl
        ld a, (hl)
        or a
        jr z, 2$
        cp #0x20
        jr nz, 4$
5$:
        ld (hl), #0
        inc hl
        jr 3$
2$:
        ld hl, #0
        add hl, sp
        push hl                 ; push pointer to argv array on stack
1$:
        ld b, #0
        push bc                 ; push argc
                                ; reverse argv order
        ld e, c
        srl e
        jr z, 7$                ; only 1 parameter

        dec c
        ld ixh, b
        ld ixl, c
        add ix, ix
        ld b, h
        ld c, l
        add ix, bc
8$:
        ld d, (hl)
        ld a, 0 (ix)
        ld 0 (ix), d
        ld (hl), a
        inc hl
        ld d, (hl)
        ld a, 1 (ix)
        ld 1 (ix), d
        ld (hl), a
        inc hl
        dec ix
        dec ix
        dec e
        jr nz, 8$
7$:
        call _main
.exit:
        push hl
        ld l, #0
        call _SWITCH_ROM
        call .restore_int_vector
        pop hl
        ld b, l
        CALL_BDOS #_TERM        ; try terminate usind MSX DOS 2 function
        JP_BDOS #_TERM0
_exit::
        pop hl
        pop hl
        jr .exit

.exit_error:
        ld hl, #1$
        push hl
        call _puts
        JP_BDOS #_TERM0
1$:
        .asciz "LOAD ERROR!\r\n"

        ;; fills memory at HL of length BC with A, clobbers DE
.memset_simple::
        ld e, a
        ld a, c
        or b
        ret z
        ld (hl), e
        dec bc
        ld d, h
        ld e, l
        inc de

        ;; copies BC bytes from HL into DE
.memcpy_simple::
        ld a, c
        or b
        ret z
        ldir
        ret

        ;; put some internal data here, to save bytes wasted for the alignment of __banks_remap_table

___overlay_count::
        .db 0
.overlay_fcb:
        .db 0                   ; drive
___overlay_name::
        .ascii "OVERLAYS"       ; filename
.overlay_fcb_ext::
        .ascii "000"            ; extension
.overlay_fcb_extent:
        .db 0                   ; extent
        .db 0                   ; attributes
.overlay_fcb_record_size:
        .db 0                   ; extent       ; msx-dos1: record size low byte
.overlay_fbc_record_cont:
        .db 0                   ; record count ; msx-dos1: record size high byte
.overlay_fcb_file_size:
        .db 0, 0, 0, 0          ; file size
        .db 0, 0, 0, 0          ; volume id
        .ds 8                   ; internal
.overlay_fcb_position:
        .db 0                   ; current record within extent
.overlay_fcb_random_pos:
        .db 0, 0, 0, 0          ; Random record number
.overlay_fcb_end:

__memman_present::
        .db 0
__rammapper_table::
        .dw 0

.rammapper_hiwater:
        .db 4                   ; 7 segments for 128K config
.rammapper_alloc_hiwater:
        .db 1                   ; allocation starts from bank 1

.mapper_page_alloc::
        ld hl, #.rammapper_hiwater
        ld a, (hl)
        inc (hl)
        or a                    ; never fail
        ret

_SWITCH_ROM::                   ; Z88DK_FASTCALL : uint8_t parameter in l
        ld a, l
        ld (__current_bank), a
        ld h, #>__banks_remap_table
        ld l, a
        ld a, (hl)
.mapper_page_set::              ; must preserve C
        out (.MAP_FRAME1), a
        ret

.mapper_page_get::
        in a, (.MAP_FRAME1)
__mapper_page_mask = .+1
.globl __mapper_page_mask
        and #0x00               ; zero ram size, will be patched
        ret

__current_bank::
        .db 0

; --- 256 byte boundary ---------------------------------------

        .bndry 0x100
__banks_remap_table::
        .ds 100
l__banks_remap_table = .-__banks_remap_table

__mapper_bank_alloc::
        ld hl, #.rammapper_alloc_hiwater
        ld a, (hl)
        inc (hl)
        ld h, #>__banks_remap_table
        ld l, a
        ld a, #0xff
        cp (hl)
        jr nz, 1$

        xor a
        ld b, a
        push hl
        call .mapper_page_alloc
        pop hl
        ret c                   ; return if no memory
        ld (hl), a              ; set segment number for the bank number
1$:
        or a
        ret                     ; allocated bank returns in l

.macro TRANSFER_POINTER ofs dest
        ld hl, #dest
        ld (hl), #0xc3
        inc hl
        ld a, ofs (ix)
        ld (hl), a
        inc hl
        ld a, ofs+1 (ix)
        ld (hl), a
.endm

.initialize_ram_mapper::
                                ; detect mapper capacity
        in a, (.MAP_FRAME1)
        ld e, a
        xor a
        out (.MAP_FRAME1), a
        in a, (.MAP_FRAME1)
        cpl
        ld (__mapper_page_mask), a
        and e
        ld hl, #__banks_remap_table
        ld (hl), a
        inc hl
        ld (hl), a              ; bank 0 == bank 1

        xor a
        ld de,#0401             ; get routines info table
        call .EXTBIO
        ld (__rammapper_table), hl

        xor a
        ld de, #0x0402          ; populate ram mapper routines table
        call .EXTBIO
        or a
        jr nz, 1$
        scf
        ret
1$:
        push hl
        pop ix

        TRANSFER_POINTER 0x01, .mapper_page_alloc
        TRANSFER_POINTER 0x1f, .mapper_page_set
        TRANSFER_POINTER 0x22, .mapper_page_get

        ld a, #1
        ld (__memman_present), a

        or a                    ; return ok
        ret

        ;; Wait for VBL interrupt to be finished
.wait_vbl_done::
_wait_vbl_done::
_vsync::
        ld  a, (_shadow_VDP_R1)
        and #.R1_DISP_ON
        ret z

        xor a
        ld (.vbl_done), a
1$:
        halt
        ld a, (.vbl_done)
        or a
        jr z, 1$
        ret

; --- 256 byte boundary ---------------------------------------

        .bndry 256
_shadow_OAM::
        .ds 0x80

        ;; load overlays, count in A
.load_overlays:
        ld b, a
        ld a, (__memman_present)
        or a
        jr nz, 3$
        ld a, (__mapper_page_mask)
        sub #3                  ; ram segments used for DOS
        inc a
        cp b
        ret c                   ; not sufficient ram to load overlays
3$:
        ld c, #1
1$:
        push bc
        call .load_overlay
        pop bc
        ret c                   ; error loading overlay

        ld a, c
        cp b
        jr z, 2$                ; all loaded - return

        inc c                   ; next overlay
        ld a, #(l__banks_remap_table - 1)
        cp c
        ret c                   ; more than 99 overlays are not allowed

        jr 1$
2$:
        xor a
        call _SWITCH_ROM
        ret

.load_overlay:
        push bc
        call __mapper_bank_alloc
        pop bc
        ret c                   ; no free segments

        call _SWITCH_ROM        ; switch bank to l

        ld b, #8
        xor a
1$:
        rlc c
        adc a
        daa
        djnz 1$
                                ; result in a, max 99 overlays
        ld c, a
        ld b, #0x0f
        ld e, #'0'
        ld hl, #(.overlay_fcb_ext + 1)
        rra
        rra
        rra
        rra
        and b
        add e
        ld (hl), a
        inc hl
        ld a, c
        and b
        add e
        ld (hl), a

        xor a
        ld bc, #(.overlay_fcb_end - .overlay_fcb_extent)
        ld hl, #.overlay_fcb_extent
        call .memset_simple     ; initialize fcb

        ld de, #.overlay_fcb
        CALL_BDOS #_FOPEN
        rrca
        ret c                   ; file not found

        ld de, #0x4000
3$:
        CALL_BDOS #_SETDTA

        push de
        ld de, #.overlay_fcb
        CALL_BDOS #_RDSEQ
        pop de

        rrca
        jr c, 2$                ; EOF reached

        ld a, #128
        ADD_A_REG16 d, e

        ld a, #0xc0
        cp d
        jr z, 2$                ; end of page1 reached

        jr 3$
2$:
        ld de, #.overlay_fcb
        CALL_BDOS #_FCLOSE

        or a                    ; return ok
        ret

.restore_int_vector:
        ld hl, #__old_int_vector
        ld de, #.LS_INT_VECTOR
        jr .restore_ldir
.save_int_vector:
        ld hl, #.LS_INT_VECTOR
        ld de, #__old_int_vector
.restore_ldir:
        ld bc, #0x0005
        di
        ldir
        ei
        ret

.setup_video_mode:
        di
        ;; Initialize VDP
        ld c, #.VDP_CMD
        ld b, #(.shadow_VDP_end - .shadow_VDP)
        ld hl,#(.shadow_VDP_end - 1)
1$:
        outd
        jr 3$                   ; delay
3$:
        ld a, b
        or #.VDP_REG_MASK
        out (c), a

        ld a, b
        or a
        jr nz, 1$

        ei
        ret

.shadow_VDP:
_shadow_VDP_R0::
        .db #(.R0_DEFAULT | .R0_SCR_MODE2)
_shadow_VDP_R1::
        .db #(.R1_DEFAULT | .R1_DISP_ON | .R1_IE | .R1_SCR_MODE2 | .R1_SPR_8X8)
_shadow_VDP_R2::
        .db .R2_MAP_0x1C00
_shadow_VDP_R3::
        .db 0xFF                        ; tiledata attr from 0x2000
_shadow_VDP_R4::
        .db 0x03                        ; tiledata from 0x0000
_shadow_VDP_R5::
        .db .R5_SAT_0x1B00
_shadow_VDP_R6::
        .db 0x07
_shadow_VDP_R7::
_shadow_VDP_RBORDER::
        .db 0x01
.shadow_VDP_end::

.sys_time::
_sys_time::
        .dw 0x0000
.vbl_done::
        .db 0
__shadow_OAM_base::
        .db #>_shadow_OAM
__shadow_OAM_OFF::
        .db 0
.mode::
        .ds .T_MODE_INOUT       ; Current mode
__old_int_vector::
        .ds 5

        .area   _GSINIT
.gsinit::
        ;; initialize static storage
        xor a
        ld bc, #l__DATA
        ld hl, #s__DATA
        call .memset_simple     ; initialize variables in RAM with zero

        ;; initialize static storage variables
        ld bc, #l__INITIALIZER
        ld hl, #s__INITIALIZER
        ld de, #s__INITIALIZED
        call .memcpy_simple

        ;; initialize ram mapper
        call .initialize_ram_mapper

        .area   _GSFINAL
        ret
