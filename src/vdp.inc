;==============================================================
; VDP initialization data
;==============================================================
VdpData:
.db $04,$80,$02,$81,$ff,$82,$ff,$85,$ff,$86,$00,$87,$00,$88,$00,$89,$ff,$8a
VdpDataEnd:

;==============================================================
; Palette data - BRIGHT & CARTOONY Sega style
; SMS format: --BBGGRR (2 bits per channel, 0-3)
;==============================================================
PaletteData:
; Palette 0 (terrain + font) - warm and visible
;  0: $00 black bg        1: $2A bright grey (font)
;  2: $14 warm brown      3: $19 saturated brick
;  4: $2E bright tan      5: $00 black (for priority fill)
;  6: $1A warm grey       7: $2F light speckle
;  8: $0C warm gold       9: $1F bright gold
; 10: $3F white
; 11: $05 dark brown (root shadow)  12: $06 warm brown (root body)
; 13: $0B golden brown (root highlight)  14: $04 dark green (leaf greeble)
; 15: $3F white
.db $00,$2A,$14,$19,$2E,$00,$1A,$2F,$0C,$1F,$3F,$05,$06,$0B,$04,$3F
; Palette 1 (sprites) - vivid and cute
;  0: $00 transparent     1: $08 forest green
;  2: $0C bright green    3: $1C vivid lime
;  4: $3F white           5: $07 bright red
;  6: $02 dark red        7: $19 warm brown
;  8: $1D light brown     9: $2B vivid pink
; 10: $18 olive           11: $1C yellow-green
; 12: $2C bright lime     13: $1A warm grey
; 14: $1E bright teal     15: $1F bright yellow
.db $00,$08,$0C,$1C,$3F,$07,$02,$19,$1D,$2B,$18,$1C,$2C,$1A,$1E,$1F
PaletteDataEnd:
