;==============================================================
; Custom game tiles (4bpp SMS planar format)
; Each tile: 8 rows x 4 bytes (bp0, bp1, bp2, bp3) = 32 bytes
;
; Placed at tile indices 95+ (after the font, which uses 0-94)
; Game code maps ASCII chars to these indices via GameTileIndex
;==============================================================

.define TILE_WALL   95
.define TILE_FLOOR  96
.define TILE_STAIRS 97
.define TILE_PLAYER 98
.define TILE_RAT    99
.define TILE_SNAKE  100
.define TILE_ORC    101

; --- Wall (tile 95, palette 0) ---
; Brick pattern: brown bricks with dark mortar lines
;  3 3 3 4 3 3 3 2   colors: 2=mortar 3=brick 4=highlight
;  3 3 3 4 3 3 3 2
;  2 2 2 2 2 2 2 2
;  3 3 2 3 3 3 4 3   (offset row)
;  3 3 2 3 3 3 4 3
;  2 2 2 2 2 2 2 2
;  3 3 3 4 3 3 3 2
;  3 3 3 4 3 3 3 2
WallTileData:
.db $EE,$EF,$10,$00
.db $EE,$EF,$10,$00
.db $00,$FF,$00,$00
.db $DD,$FD,$02,$00
.db $DD,$FD,$02,$00
.db $00,$FF,$00,$00
.db $EE,$EF,$10,$00
.db $EE,$EF,$10,$00

; --- Floor (tile 96, palette 0) ---
; Dark floor with subtle speckles
;  0 0 0 0 0 0 0 0   color 7 = grey speckle
;  0 0 0 0 0 0 0 0
;  0 0 7 0 0 0 0 0
;  0 0 0 0 0 0 0 0
;  0 0 0 0 0 0 0 0
;  0 0 0 0 0 7 0 0
;  0 0 0 0 0 0 0 0
;  0 0 0 0 0 0 0 0
FloorTileData:
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $20,$20,$20,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $04,$04,$04,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00

; --- Stairs (tile 97, palette 0) ---
; Descending staircase in gold
;  0 0 0 0 0 0 0 0   colors: 8=dark gold 9=bright gold
;  9 9 9 0 0 0 0 0
;  8 8 8 0 0 0 0 0
;  0 0 9 9 9 0 0 0
;  0 0 8 8 8 0 0 0
;  0 0 0 0 9 9 9 0
;  0 0 0 0 8 8 8 0
;  0 0 0 0 0 0 0 0
StairsTileData:
.db $00,$00,$00,$00
.db $E0,$00,$00,$E0
.db $00,$00,$00,$E0
.db $38,$00,$00,$38
.db $00,$00,$00,$38
.db $0E,$00,$00,$0E
.db $00,$00,$00,$0E
.db $00,$00,$00,$00

; --- Player frog adventurer (tile 98, palette 1) ---
; Anthropomorphic frog with red hat and big eyes
;  0 0 5 5 5 0 0 0   5=red hat
;  0 4 2 2 2 4 0 0   4=white eyes 2=green body
;  0 2 3 2 3 2 0 0   3=bright green highlights
;  0 0 2 1 2 0 0 0   1=dark green shadow
;  0 0 3 2 3 0 0 0
;  0 3 2 2 2 3 0 0   arms out
;  0 0 2 0 2 0 0 0   legs
;  0 0 1 0 1 0 0 0   feet
PlayerTileData:
.db $38,$00,$38,$00
.db $00,$38,$44,$00
.db $28,$7C,$00,$00
.db $10,$28,$00,$00
.db $28,$38,$00,$00
.db $44,$7C,$00,$00
.db $00,$28,$00,$00
.db $28,$00,$00,$00

; --- Rat (tile 99, palette 1) ---
; Small brown rat with pink ears and tail
;  0 0 0 0 0 0 0 0   7=brown 8=light brown
;  0 9 0 0 0 9 0 0   9=pink ears 4=white eyes
;  0 7 4 0 4 7 0 0
;  0 0 7 7 7 0 0 0
;  0 7 8 8 8 7 0 0
;  0 0 7 7 7 0 0 0
;  0 0 7 0 7 0 9 9   tail
;  0 0 0 0 0 0 0 0
RatTileData:
.db $00,$00,$00,$00
.db $44,$00,$00,$44
.db $44,$44,$6C,$00
.db $38,$38,$38,$00
.db $44,$44,$44,$38
.db $38,$38,$38,$00
.db $2B,$28,$28,$03
.db $00,$00,$00,$00

; --- Snake (tile 100, palette 1) ---
; Coiled snake with fangs
;  0  0  0  0  0  0  0  0   11=olive body 12=highlight
;  0  0  11 12 11 0  0  0   4=white eyes 5=red fangs
;  0  4  11 11 11 4  0  0   10=dark olive
;  0  0  5  11 5  0  0  0
;  0  11 12 0  12 11 0  0
;  11 0  0  11 0  0  11 0
;  11 12 12 11 12 12 10 0
;  0  0  0  0  0  0  0  0
SnakeTileData:
.db $00,$00,$00,$00
.db $28,$28,$10,$38
.db $38,$38,$44,$38
.db $38,$10,$28,$10
.db $44,$44,$28,$6C
.db $92,$92,$00,$92
.db $90,$92,$6C,$FE
.db $00,$00,$00,$00

; --- Orc (tile 101, palette 1) ---
; Stocky green-skinned orc with tusks
;  0  0  14 14 14 0  0  0    14=orc skin 15=yellow eyes
;  0  14 15 14 15 14 0  0    13=dark shadow 4=white tusks
;  0  14 14 14 14 14 0  0
;  0  0  4  14 4  0  0  0    tusks
;  0  14 13 13 13 14 0  0
;  0  13 14 14 14 13 0  0
;  0  0  13 0  13 0  0  0
;  0  0  14 0  14 0  0  0
OrcTileData:
.db $00,$38,$38,$38
.db $28,$7C,$7C,$7C
.db $00,$7C,$7C,$7C
.db $00,$10,$38,$10
.db $38,$44,$7C,$7C
.db $44,$38,$7C,$7C
.db $28,$00,$28,$28
.db $00,$28,$28,$28

;==============================================================
; Custom tiles are loaded sequentially starting at tile 95
; VRAM address = 95 * 32 = 3040 = $0BE0
; Total: 7 tiles * 32 bytes = 224 bytes
;==============================================================
CustomTilesStart:
; (WallTileData is already at the right position, tiles are sequential)
CustomTileCount = 7
