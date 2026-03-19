;==============================================================
; VDP initialization data
;==============================================================
VdpData:
.db $04,$80,$00,$81,$ff,$82,$ff,$85,$ff,$86,$ff,$87,$00,$88,$00,$89,$ff,$8a
VdpDataEnd:

;==============================================================
; Palette data (32 bytes - both palettes)
; SMS format: --BBGGRR (2 bits per channel)
;==============================================================
PaletteData:
; Palette 0 (terrain + font)
;  0: black bg       1: grey (font fg)   2: dark brown (mortar)
;  3: med brown (brick) 4: tan (brick hi) 5: very dark grey
;  6: dark grey       7: med grey (speckle) 8: dark gold (stairs)
;  9: bright gold    10: light yellow    11: dark green (moss)
; 12-14: unused      15: white
.db $00,$15,$10,$14,$24,$05,$15,$2A,$08,$0F,$2F,$04,$00,$00,$00,$3F
; Palette 1 (entities)
;  0: black bg        1: dark green (frog shadow)
;  2: med green (body) 3: bright green (highlight)
;  4: white (eyes)    5: red (hat)       6: dark red (hat shadow)
;  7: brown (rat)     8: light brown     9: pink (rat ears)
; 10: dark olive     11: olive (snake)  12: yellow-green (snake hi)
; 13: dark grey (orc) 14: green-grey (orc skin) 15: yellow (orc eyes)
.db $00,$04,$08,$0C,$3F,$02,$01,$10,$14,$23,$14,$18,$1C,$15,$19,$0F
PaletteDataEnd:
