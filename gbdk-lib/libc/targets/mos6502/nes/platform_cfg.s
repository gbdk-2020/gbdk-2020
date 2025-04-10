;;;
;;; Platform/mapper specific configuration settings.
;;;

;
; Tilemap layout (nametable mirroring) setting.
;
; Available settings:
;
;   S: Single-screen layout/mirroring.
;   H: Horizontal layout (vertical mirroring)
;   V: Vertical layout (horizontal mirroring)
;   F: Four-screen layout no mirroring)
;
; Only *one* of these should be enabled.
; The same define should also be passed to LCC for compile-time settings in the C include files.
;

NES_TILEMAP_S = 1
;NES_TILEMAP_H = 1
;NES_TILEMAP_V = 1
;NES_TILEMAP_F = 1

;
; LOMEM setting
;
; This places the 64 bytes of attribute shadow (assuming single-screen layout) in the stack area
; saving 64 bytes of RAM memory for user variables, at the expense of a reduced stack for function calls.
;
; This setting is *only* valid when NES_TILEMAP_S = 1 (using it with other tilmap layouts will corrupt the stack)
;

NES_LOMEM = 1
