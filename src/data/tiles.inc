;==============================================================
; Tile data - bright cartoony style
;==============================================================

; Item types (9 items in 3 categories)
.define ITEM_WEAPON_1  1   ; Sharp Stick (+1 ATK)
.define ITEM_WEAPON_2  2   ; Lacquer Blade (+3 ATK)
.define ITEM_WEAPON_3  3   ; Bog Oak Mace (+5 ATK)
.define ITEM_SHIELD_1  4   ; Wooden Shield (+1 DEF)
.define ITEM_SHIELD_2  5   ; Spidershell Shield (+2 DEF)
.define ITEM_SHIELD_3  6   ; Bog Oak Shield (+4 DEF)
.define ITEM_ARMOR_1   7   ; Reed Armor (+2 MaxHP)
.define ITEM_ARMOR_2   8   ; Spidershell Armor (+5 MaxHP)
.define ITEM_ARMOR_3   9   ; Bog Oak Armor (+10 MaxHP)
.define ITEM_HELMET   10  ; Helmet (+1 DEF)
.define ITEM_POTION   11  ; Healing Potion (+8 HP, 2 hearts)
.define ITEM_WAND     12  ; Wand of Fire (zap nearest enemy, 10 dmg)

; Background tile indices
.define BG_WALL_TL  95
.define BG_WALL_TR  96
.define BG_FLOOR    97
.define BG_STAIR_TL 98
.define BG_STAIR_TR 99
.define BG_STAIR_BL 100
.define BG_STAIR_BR 101

;==============================================================
; BACKGROUND TILES (palette 0) - bright bricks and warm floors
;==============================================================

; Wall TL - warm brick with highlights
; Colors: 2=mortar, 3=brick, 4=highlight
WallTLData:
.db $ef,$ef,$10,$00
.db $ef,$ef,$10,$00
.db $ef,$ef,$10,$00
.db $00,$ff,$00,$00
.db $f9,$fb,$04,$00
.db $f9,$fb,$04,$00
.db $f9,$fb,$04,$00
.db $00,$ff,$00,$00

; Wall TR - offset brick pattern
WallTRData:
.db $fc,$fd,$02,$00
.db $fc,$fd,$02,$00
.db $fc,$fd,$02,$00
.db $00,$ff,$00,$00
.db $bf,$bf,$40,$00
.db $bf,$bf,$40,$00
.db $bf,$bf,$40,$00
.db $00,$ff,$00,$00

; Floor - dark with light speckles
FloorTileData:
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $20,$20,$20,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $04,$04,$04,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00

; Stairs TL - big gold steps
StairsTLData:
.db $fc,$00,$00,$fc
.db $fc,$00,$00,$fc
.db $00,$00,$00,$fc
.db $00,$00,$00,$fc
.db $0f,$00,$00,$0f
.db $0f,$00,$00,$0f
.db $00,$00,$00,$0f
.db $00,$00,$00,$0f

StairsTRData:
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $c0,$00,$00,$c0
.db $c0,$00,$00,$c0
.db $00,$00,$00,$c0
.db $00,$00,$00,$c0

StairsBLData:
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00

StairsBRData:
.db $fc,$00,$00,$fc
.db $fc,$00,$00,$fc
.db $00,$00,$00,$fc
.db $00,$00,$00,$fc
.db $0f,$00,$00,$0f
.db $0f,$00,$00,$0f
.db $00,$00,$00,$0f
.db $00,$00,$00,$0f

; === ROOT BORDER TILES (palette 0) ===
; Organic twisted root frame: 2=brown shadow, 3=root body, 4=highlight
; 8 tiles: TL corner, top edge, TR corner, left edge, right edge, BL, bottom, BR
.define BG_ROOT_TL 102
.define BG_ROOT_T  103
.define BG_ROOT_TR 104
.define BG_ROOT_L  105
.define BG_ROOT_R  106
.define BG_ROOT_BL 107
.define BG_ROOT_B  108
.define BG_ROOT_BR 109
.define BG_ROOT_T2 110
.define BG_ROOT_B2 111
.define BG_ROOT_L2 112
.define BG_ROOT_R2 113


; Root border tiles (color 5 fill for priority)

RootTLData:
.db $28,$08,$30,$38
.db $28,$08,$30,$38
.db $ef,$08,$f7,$ff
.db $00,$00,$ff,$ff
.db $fe,$fe,$00,$fe
.db $28,$08,$30,$38
.db $28,$08,$30,$38
.db $28,$08,$30,$38

RootTData:
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $ff,$00,$ff,$ff
.db $00,$00,$ff,$ff
.db $ff,$ff,$00,$ff
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00

RootTRData:
.db $14,$10,$0c,$1c
.db $14,$10,$0c,$1c
.db $f7,$10,$ef,$ff
.db $00,$00,$ff,$ff
.db $7f,$7f,$00,$7f
.db $14,$10,$0c,$1c
.db $14,$10,$0c,$1c
.db $14,$10,$0c,$1c

RootLData:
.db $28,$08,$30,$38
.db $28,$08,$30,$38
.db $28,$08,$30,$38
.db $28,$08,$30,$38
.db $28,$08,$30,$38
.db $28,$08,$30,$38
.db $28,$08,$30,$38
.db $28,$08,$30,$38

RootRData:
.db $14,$10,$0c,$1c
.db $14,$10,$0c,$1c
.db $14,$10,$0c,$1c
.db $14,$10,$0c,$1c
.db $14,$10,$0c,$1c
.db $14,$10,$0c,$1c
.db $14,$10,$0c,$1c
.db $14,$10,$0c,$1c

RootBLData:
.db $28,$08,$30,$38
.db $28,$08,$30,$38
.db $ef,$08,$f7,$ff
.db $00,$00,$ff,$ff
.db $fe,$fe,$00,$fe
.db $28,$08,$30,$38
.db $28,$08,$30,$38
.db $28,$08,$30,$38

RootBData:
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $ff,$00,$ff,$ff
.db $00,$00,$ff,$ff
.db $ff,$ff,$00,$ff
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00

RootBRData:
.db $14,$10,$0c,$1c
.db $14,$10,$0c,$1c
.db $f7,$10,$ef,$ff
.db $00,$00,$ff,$ff
.db $7f,$7f,$00,$7f
.db $14,$10,$0c,$1c
.db $14,$10,$0c,$1c
.db $14,$10,$0c,$1c

RootT2Data:
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $df,$00,$ff,$ff
.db $60,$00,$ff,$ff
.db $ff,$ff,$00,$ff
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00

RootB2Data:
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $ff,$00,$ff,$ff
.db $0c,$00,$ff,$ff
.db $f9,$f9,$06,$ff
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00

RootL2Data:
.db $28,$08,$30,$38
.db $28,$08,$30,$38
.db $08,$08,$30,$38
.db $38,$08,$30,$38
.db $28,$08,$30,$38
.db $28,$08,$30,$38
.db $08,$08,$30,$38
.db $28,$08,$30,$38

RootR2Data:
.db $14,$10,$0c,$1c
.db $14,$10,$0c,$1c
.db $10,$10,$0c,$1c
.db $1c,$10,$0c,$1c
.db $14,$10,$0c,$1c
.db $14,$10,$0c,$1c
.db $10,$10,$0c,$1c
.db $14,$10,$0c,$1c

.define BG_ICON_INV_TL 114
.define BG_ICON_INV_TR 115
.define BG_ICON_INV_BL 116
.define BG_ICON_INV_BR 117
.define BG_ICON_PICK_TL 118
.define BG_ICON_PICK_TR 119
.define BG_ICON_PICK_BL 120
.define BG_ICON_PICK_BR 121
.define BG_ICON_WAIT_TL 122
.define BG_ICON_WAIT_TR 123
.define BG_ICON_WAIT_BL 124
.define BG_ICON_WAIT_BR 125
.define BG_ICON_X_TL 126
.define BG_ICON_X_TR 127
.define BG_ICON_X_BL 128
.define BG_ICON_X_BR 129

; Menu icons (color 5 fill for priority)
IconInvTL:
.db $24,$00,$00,$00
.db $24,$00,$00,$00
.db $3c,$00,$00,$00
.db $3c,$00,$00,$00
.db $3c,$00,$00,$00
.db $3c,$00,$00,$00
.db $3c,$00,$00,$00
.db $3c,$00,$00,$00

IconInvTR:
.db $24,$00,$00,$00
.db $24,$00,$00,$00
.db $3c,$00,$00,$00
.db $3c,$00,$00,$00
.db $3c,$00,$00,$00
.db $3c,$00,$00,$00
.db $3c,$00,$00,$00
.db $3c,$00,$00,$00

IconInvBL:
.db $3c,$00,$00,$00
.db $3c,$00,$00,$00
.db $3c,$00,$00,$00
.db $3c,$00,$00,$00
.db $3c,$00,$00,$00
.db $18,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00

IconInvBR:
.db $3c,$00,$00,$00
.db $3c,$00,$00,$00
.db $3c,$00,$00,$00
.db $3c,$00,$00,$00
.db $3c,$00,$00,$00
.db $18,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00

IconPickTL:
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $08,$00,$00,$00
.db $08,$00,$00,$00
.db $08,$00,$00,$00
.db $08,$00,$00,$00
.db $38,$00,$00,$00
.db $08,$00,$00,$00

IconPickTR:
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00

IconPickBL:
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $18,$00,$00,$00
.db $18,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00

IconPickBR:
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00

IconWaitTL:
.db $00,$00,$00,$00
.db $7e,$00,$00,$00
.db $3c,$00,$00,$00
.db $18,$00,$00,$00
.db $18,$00,$00,$00
.db $3c,$00,$00,$00
.db $7e,$00,$00,$00
.db $7e,$00,$00,$00

IconWaitTR:
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00

IconWaitBL:
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00

IconWaitBR:
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00

IconXTL:
.db $00,$00,$00,$00
.db $c3,$00,$00,$00
.db $66,$00,$00,$00
.db $3c,$00,$00,$00
.db $18,$00,$00,$00
.db $3c,$00,$00,$00
.db $66,$00,$00,$00
.db $c3,$00,$00,$00

IconXTR:
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00

IconXBL:
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00

IconXBR:
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00

; Solid black tile using color 5 (non-zero, so priority bit works)
.define BG_SOLID_BLACK 130
SolidBlackData:
.db $00,$00,$00,$00  ; all pixels = color 5 (0101 = bp0+bp2)
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00

; === CONTEXTUAL DIRT WALL TILES (palette 0) ===
; 7 tiles: solid(x2), top edge(x2), left edge(x2), corner
; Right/bottom edges and other corners via H-flip/V-flip attributes
.define BG_DIRT_SOLID   131
.define BG_DIRT_SOLID_V 132
.define BG_DIRT_EDGE_T  133
.define BG_DIRT_EDGE_T2 134
.define BG_DIRT_EDGE_L  135
.define BG_DIRT_EDGE_L2 136
.define BG_DIRT_CORNER  137

DirtSolidData:
.db $94,$ff,$00,$00
.db $62,$ff,$00,$00
.db $14,$ff,$00,$00
.db $a1,$ff,$00,$00
.db $12,$ff,$00,$00
.db $44,$ff,$00,$00
.db $92,$ff,$00,$00
.db $24,$ff,$00,$00

DirtSolidV2Data:
.db $22,$ff,$00,$00
.db $94,$ff,$00,$00
.db $42,$ff,$00,$00
.db $28,$ff,$00,$00
.db $84,$ff,$00,$00
.db $12,$ff,$00,$00
.db $a2,$ff,$00,$00
.db $14,$ff,$00,$00

DirtEdgeTData:
.db $00,$00,$00,$00
.db $a9,$a9,$00,$a9
.db $ff,$ff,$00,$55
.db $da,$ff,$00,$00
.db $94,$ff,$00,$00
.db $22,$ff,$00,$00
.db $94,$ff,$00,$00
.db $44,$ff,$00,$00

DirtEdgeT2Data:
.db $00,$00,$00,$00
.db $4a,$4a,$00,$4a
.db $ff,$ff,$00,$a5
.db $6a,$ff,$00,$00
.db $22,$ff,$00,$00
.db $94,$ff,$00,$00
.db $22,$ff,$00,$00
.db $94,$ff,$00,$00

DirtEdgeLData:
.db $72,$7f,$00,$40
.db $74,$7f,$00,$60
.db $72,$7f,$00,$50
.db $74,$7f,$00,$40
.db $72,$7f,$00,$60
.db $74,$7f,$00,$40
.db $72,$7f,$00,$50
.db $74,$7f,$00,$40

DirtEdgeL2Data:
.db $74,$7f,$00,$60
.db $72,$7f,$00,$40
.db $74,$7f,$00,$60
.db $72,$7f,$00,$50
.db $74,$7f,$00,$40
.db $72,$7f,$00,$60
.db $74,$7f,$00,$40
.db $72,$7f,$00,$50

DirtCornerTLData:
.db $00,$00,$00,$00
.db $0b,$0b,$00,$0b
.db $1f,$1f,$00,$1a
.db $3a,$3f,$00,$20
.db $74,$7f,$00,$60
.db $72,$7f,$00,$40
.db $e4,$ff,$00,$a0
.db $d2,$ff,$00,$00

; === GREEBLE TILES (palette 0) - decorative details for interior walls ===
.define BG_GREEBLE_CRACK 138
.define BG_GREEBLE_BONE  139
.define BG_GREEBLE_SKULL 140
.define BG_GREEBLE_MOLE  141
.define BG_GREEBLE_ROOT1 142
.define BG_GREEBLE_ROOT2 143
.define BG_GREEBLE_ROOT3 144

GreebleCrackData:
.db $94,$bf,$00,$00
.db $52,$df,$00,$00
.db $24,$ef,$00,$00
.db $90,$f7,$00,$00
.db $08,$fb,$00,$00
.db $44,$fd,$00,$00
.db $92,$fe,$00,$00
.db $25,$ff,$00,$00

GreebleBoneData:
.db $94,$ff,$00,$00
.db $62,$ff,$00,$00
.db $00,$db,$24,$00
.db $82,$e7,$18,$00
.db $82,$e7,$18,$00
.db $00,$db,$24,$00
.db $92,$ff,$00,$00
.db $24,$ff,$00,$00

GreebleSkullData:
.db $94,$ff,$00,$00
.db $42,$cf,$30,$00
.db $02,$87,$48,$00
.db $02,$87,$78,$00
.db $00,$c7,$10,$00
.db $44,$cf,$30,$00
.db $92,$ff,$00,$00
.db $24,$ff,$00,$00

GreebleMoleData:
.db $94,$ff,$00,$00
.db $62,$ff,$00,$00
.db $0c,$ff,$30,$00
.db $82,$ef,$6c,$00
.db $12,$ff,$68,$00
.db $0c,$ff,$30,$00
.db $92,$ff,$00,$00
.db $24,$ff,$00,$00

GreebleRoot1Data:
.db $90,$ff,$04,$04
.db $62,$ff,$04,$04
.db $14,$ff,$08,$08
.db $91,$ff,$20,$20
.db $12,$ff,$40,$40
.db $44,$ff,$80,$80
.db $92,$ff,$00,$00
.db $24,$ff,$00,$00

GreebleRoot2Data:
.db $14,$ff,$80,$80
.db $22,$ff,$40,$40
.db $14,$ff,$20,$20
.db $81,$ff,$10,$10
.db $02,$ff,$08,$08
.db $40,$ff,$04,$04
.db $92,$ff,$00,$00
.db $24,$ff,$00,$00

GreebleRoot3Data:
.db $10,$ff,$84,$84
.db $22,$ff,$48,$48
.db $04,$ff,$30,$30
.db $81,$ff,$10,$10
.db $02,$ff,$10,$10
.db $44,$ff,$00,$00
.db $92,$ff,$00,$00
.db $24,$ff,$00,$00

; Greeble tile lookup (7 entries, indexed by greeble index 0-6)
GreebleTileTable:
.db BG_GREEBLE_CRACK, BG_GREEBLE_BONE, BG_GREEBLE_SKULL
.db BG_GREEBLE_MOLE, BG_GREEBLE_ROOT1, BG_GREEBLE_ROOT2
.db BG_GREEBLE_ROOT3

; === HEART TILES (palette 0) - HP display in 1/4 increments ===
; Color 15 = red (fill), color 11 = dark brown (outline)
.define BG_HEART_FULL  145
.define BG_HEART_34    146
.define BG_HEART_HALF  147
.define BG_HEART_14    148
.define BG_HEART_EMPTY 149

HeartFullData:
.db $6c,$6c,$00,$6c
.db $fe,$fe,$6c,$fe
.db $fe,$fe,$7c,$fe
.db $fe,$fe,$7c,$fe
.db $fe,$fe,$7c,$fe
.db $7c,$7c,$38,$7c
.db $38,$38,$10,$38
.db $10,$10,$00,$10

Heart34Data:
.db $6c,$6c,$00,$6c
.db $f2,$fe,$60,$fe
.db $f2,$fe,$70,$fe
.db $fe,$fe,$7c,$fe
.db $fe,$fe,$7c,$fe
.db $7c,$7c,$38,$7c
.db $38,$38,$10,$38
.db $10,$10,$00,$10

HeartHalfData:
.db $6c,$6c,$00,$6c
.db $f2,$fe,$60,$fe
.db $f2,$fe,$70,$fe
.db $f2,$fe,$70,$fe
.db $f2,$fe,$70,$fe
.db $74,$7c,$30,$7c
.db $38,$38,$10,$38
.db $10,$10,$00,$10

Heart14Data:
.db $6c,$6c,$00,$6c
.db $f2,$fe,$60,$fe
.db $f2,$fe,$70,$fe
.db $82,$fe,$00,$fe
.db $82,$fe,$00,$fe
.db $44,$7c,$00,$7c
.db $28,$38,$00,$38
.db $10,$10,$00,$10

HeartEmptyData:
.db $6c,$6c,$00,$6c
.db $92,$fe,$00,$fe
.db $82,$fe,$00,$fe
.db $82,$fe,$00,$fe
.db $82,$fe,$00,$fe
.db $44,$7c,$00,$7c
.db $28,$38,$00,$38
.db $10,$10,$00,$10

; Heart tile lookup (indexed by quarter-fill 0-4)
HeartTileTable:
.db BG_HEART_EMPTY, BG_HEART_14, BG_HEART_HALF, BG_HEART_34, BG_HEART_FULL

BgTileCount = 55

; Neighbor mask lookup table for contextual wall tiles
; 4-bit mask: bit3=up open, bit2=right open, bit1=down open, bit0=left open
; Each entry: TL_tile, TL_attr, TR_tile, TR_attr, BL_tile, BL_attr, BR_tile, BR_attr
; Flip bits in attr: $02=H-flip, $04=V-flip, $06=HV-flip
WallQuadrantTable:
; Mask 0 (0000): all wall neighbors - full interior
.db BG_DIRT_SOLID,  $00, BG_DIRT_SOLID_V,$00, BG_DIRT_SOLID_V,$00, BG_DIRT_SOLID,  $00
; Mask 1 (0001): left open
.db BG_DIRT_EDGE_L, $00, BG_DIRT_SOLID_V,$00, BG_DIRT_EDGE_L2,$00, BG_DIRT_SOLID,  $00
; Mask 2 (0010): down open
.db BG_DIRT_SOLID,  $00, BG_DIRT_SOLID_V,$00, BG_DIRT_EDGE_T, $04, BG_DIRT_EDGE_T2,$04
; Mask 3 (0011): down+left open
.db BG_DIRT_EDGE_L, $00, BG_DIRT_SOLID_V,$00, BG_DIRT_CORNER, $04, BG_DIRT_EDGE_T2,$04
; Mask 4 (0100): right open
.db BG_DIRT_SOLID,  $00, BG_DIRT_EDGE_L, $02, BG_DIRT_SOLID_V,$00, BG_DIRT_EDGE_L2,$02
; Mask 5 (0101): right+left open
.db BG_DIRT_EDGE_L, $00, BG_DIRT_EDGE_L, $02, BG_DIRT_EDGE_L2,$00, BG_DIRT_EDGE_L2,$02
; Mask 6 (0110): right+down open
.db BG_DIRT_SOLID,  $00, BG_DIRT_EDGE_L, $02, BG_DIRT_EDGE_T, $04, BG_DIRT_CORNER, $06
; Mask 7 (0111): right+down+left open
.db BG_DIRT_EDGE_L, $00, BG_DIRT_EDGE_L, $02, BG_DIRT_CORNER, $04, BG_DIRT_CORNER, $06
; Mask 8 (1000): up open
.db BG_DIRT_EDGE_T, $00, BG_DIRT_EDGE_T2,$00, BG_DIRT_SOLID_V,$00, BG_DIRT_SOLID,  $00
; Mask 9 (1001): up+left open
.db BG_DIRT_CORNER, $00, BG_DIRT_EDGE_T2,$00, BG_DIRT_EDGE_L2,$00, BG_DIRT_SOLID,  $00
; Mask 10 (1010): up+down open
.db BG_DIRT_EDGE_T, $00, BG_DIRT_EDGE_T2,$00, BG_DIRT_EDGE_T, $04, BG_DIRT_EDGE_T2,$04
; Mask 11 (1011): up+down+left open
.db BG_DIRT_CORNER, $00, BG_DIRT_EDGE_T2,$00, BG_DIRT_CORNER, $04, BG_DIRT_EDGE_T2,$04
; Mask 12 (1100): up+right open
.db BG_DIRT_EDGE_T, $00, BG_DIRT_CORNER, $02, BG_DIRT_SOLID_V,$00, BG_DIRT_EDGE_L2,$02
; Mask 13 (1101): up+right+left open
.db BG_DIRT_CORNER, $00, BG_DIRT_CORNER, $02, BG_DIRT_EDGE_L2,$00, BG_DIRT_EDGE_L2,$02
; Mask 14 (1110): up+right+down open
.db BG_DIRT_EDGE_T, $00, BG_DIRT_CORNER, $02, BG_DIRT_EDGE_T, $04, BG_DIRT_CORNER, $06
; Mask 15 (1111): all open
.db BG_DIRT_CORNER, $00, BG_DIRT_CORNER, $02, BG_DIRT_CORNER, $04, BG_DIRT_CORNER, $06

; (BgTileCount defined above with greebles = 50)

;==============================================================
; SPRITE TILES - cute cartoony characters filling 14x14 space
; Order per entity: TL, BL, TR, BR (for 8x16 sprite mode)
; Palette 1: 0=trans 1=dkgrn 2=grn 3=lime 4=white 5=red
;   6=dkred 7=brown 8=ltbrn 9=pink 10=olive 11=ylwgrn
;   12=brtlime 13=warmgrey 14=teal 15=yellow
;==============================================================

; Frog player frame 1 - big round head, red hat, huge eyes

; Frog player frame 1 - big round head, red hat, huge eyes
PlayerSpr1Data:
; TL
.db $00,$00,$00,$00
.db $07,$18,$1f,$00
.db $1f,$20,$3f,$00
.db $7f,$67,$18,$00
.db $44,$47,$30,$00
.db $43,$47,$30,$00
.db $38,$7f,$00,$00
.db $12,$7d,$00,$00
; BL
.db $40,$3f,$00,$00
.db $30,$1f,$00,$00
.db $50,$4f,$00,$00
.db $25,$1a,$00,$00
.db $14,$08,$00,$00
.db $08,$10,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $e0,$10,$f0,$00
.db $f0,$08,$f8,$00
.db $fc,$cc,$30,$00
.db $44,$c4,$18,$00
.db $44,$c4,$18,$00
.db $38,$fc,$00,$00
.db $50,$bc,$00,$00
; BR
.db $04,$f8,$00,$00
.db $18,$f0,$00,$00
.db $14,$e4,$00,$00
.db $48,$b0,$00,$00
.db $50,$20,$00,$00
.db $40,$10,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Frog player frame 2 - sway right (top shifted 1px right)
PlayerSpr2Data:
; TL
.db $00,$00,$00,$00
.db $03,$0c,$0f,$00
.db $0f,$10,$1f,$00
.db $3f,$33,$0c,$00
.db $22,$23,$18,$00
.db $21,$23,$18,$00
.db $1c,$3f,$00,$00
.db $09,$3e,$00,$00
; BL
.db $40,$3f,$00,$00
.db $30,$1f,$00,$00
.db $50,$4f,$00,$00
.db $25,$1a,$00,$00
.db $14,$08,$00,$00
.db $08,$10,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $e0,$18,$f8,$00
.db $f8,$04,$fc,$00
.db $fe,$e6,$18,$00
.db $22,$e2,$0c,$00
.db $a2,$e2,$0c,$00
.db $1c,$fe,$00,$00
.db $28,$de,$00,$00
; BR
.db $04,$f8,$00,$00
.db $18,$f0,$00,$00
.db $14,$e4,$00,$00
.db $48,$b0,$00,$00
.db $50,$20,$00,$00
.db $40,$10,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Rat - round brown body, pink ears, big eyes, curvy tail
RatSprData:
; TL
.db $00,$00,$00,$00
.db $10,$00,$00,$10
.db $38,$10,$10,$28
.db $00,$00,$00,$3f
.db $00,$00,$18,$67
.db $24,$24,$34,$43
.db $5f,$5b,$5b,$24
.db $67,$67,$67,$18
; BL
.db $70,$70,$70,$0f
.db $35,$35,$35,$0a
.db $1f,$1f,$1f,$00
.db $08,$08,$08,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $10,$00,$00,$10
.db $70,$20,$20,$50
.db $00,$00,$00,$f8
.db $00,$00,$30,$cc
.db $90,$90,$98,$44
.db $e4,$64,$64,$98
.db $94,$94,$94,$68
; BR
.db $1c,$1c,$1c,$e0
.db $70,$70,$70,$80
.db $e0,$e0,$e0,$00
.db $98,$80,$80,$18
.db $38,$10,$10,$28
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Snake - S-curve, big head, olive/lime body, red tongue
SnakeSprData:
; TL
.db $00,$00,$00,$00
.db $00,$00,$0f,$0f
.db $0f,$0f,$10,$10
.db $03,$03,$38,$20
.db $23,$27,$10,$04
.db $0f,$3f,$00,$30
.db $0c,$1e,$00,$12
.db $05,$0d,$02,$0a
; BL
.db $01,$07,$00,$06
.db $00,$03,$00,$03
.db $00,$01,$00,$01
.db $02,$06,$01,$05
.db $0a,$1b,$04,$15
.db $08,$1c,$00,$14
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $00,$00,$c0,$c0
.db $e0,$e0,$10,$10
.db $e0,$e0,$0c,$04
.db $c4,$e4,$08,$20
.db $f0,$fc,$00,$0c
.db $08,$18,$00,$10
.db $00,$80,$00,$80
; BR
.db $40,$60,$80,$a0
.db $a0,$b0,$40,$50
.db $50,$d8,$20,$a8
.db $80,$c0,$00,$40
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Orc - broad teal body, yellow eyes, white tusks
OrcSprData:
; TL
.db $00,$00,$00,$00
.db $00,$3f,$3f,$3f
.db $00,$7f,$7f,$7f
.db $10,$57,$77,$57
.db $08,$47,$6f,$4f
.db $20,$5f,$7f,$7f
.db $40,$2f,$7f,$6f
.db $38,$07,$3f,$3f
; BL
.db $10,$2f,$3f,$3f
.db $35,$0a,$3f,$3f
.db $15,$0a,$1f,$1f
.db $11,$08,$19,$19
.db $18,$00,$18,$18
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $00,$f0,$f0,$f0
.db $00,$fc,$fc,$fc
.db $10,$d4,$dc,$d4
.db $20,$c4,$ec,$e4
.db $08,$f4,$fc,$fc
.db $0c,$d0,$fc,$dc
.db $1c,$e0,$fc,$fc
; BR
.db $08,$f4,$fc,$fc
.db $58,$a0,$f8,$f8
.db $50,$a0,$f0,$f0
.db $10,$80,$90,$90
.db $c0,$00,$c0,$c0
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Generic sword (unused, kept for compat)
WeaponSprData:
; TL
.db $00,$00,$00,$00
.db $00,$00,$03,$00
.db $00,$00,$03,$00
.db $00,$00,$03,$00
.db $00,$00,$03,$00
.db $00,$00,$03,$00
.db $0c,$00,$0f,$00
.db $03,$03,$03,$00
; BL
.db $03,$03,$03,$00
.db $03,$03,$03,$00
.db $03,$00,$03,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $c0,$00,$c0,$00
.db $00,$00,$00,$00
; BR
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Generic shield (unused, kept for compat)
ArmorSprData:
; TL
.db $00,$00,$00,$00
.db $00,$00,$07,$00
.db $17,$00,$1f,$17
.db $3f,$00,$3f,$3f
.db $3f,$00,$3f,$3f
.db $37,$00,$3f,$37
.db $3f,$00,$3f,$3f
.db $1f,$00,$1f,$1f
; BL
.db $0f,$00,$0f,$0f
.db $07,$00,$07,$07
.db $00,$00,$02,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $40,$00,$c0,$40
.db $e0,$00,$e0,$e0
.db $e0,$00,$e0,$e0
.db $60,$00,$e0,$60
.db $e0,$00,$e0,$e0
.db $c0,$00,$c0,$c0
; BR
.db $80,$00,$80,$80
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Honey Badger boss - low wide, white stripe, angry
BadgerSprData:
; TL
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$1f,$00
.db $20,$20,$3f,$00
.db $78,$58,$7f,$30
.db $7f,$2f,$7f,$50
.db $7f,$3f,$7f,$40
.db $75,$35,$75,$4a
; BL
.db $60,$20,$60,$5f
.db $75,$15,$75,$6a
.db $3f,$1f,$3f,$20
.db $1c,$08,$1c,$14
.db $1c,$08,$1c,$14
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$f8,$00
.db $08,$08,$f8,$00
.db $3c,$34,$fc,$28
.db $fc,$b4,$fc,$48
.db $fc,$e8,$fc,$14
.db $7c,$68,$7c,$94
; BR
.db $38,$20,$38,$d8
.db $78,$40,$78,$b8
.db $f0,$e0,$f0,$10
.db $e0,$40,$e0,$a0
.db $e0,$40,$e0,$a0
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Body armor sprite - chestplate
BodyArmorSprData:
; TL
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$07,$00
.db $01,$16,$1f,$17
.db $0f,$30,$3f,$3f
.db $0f,$30,$3f,$3f
.db $00,$00,$0f,$00
.db $00,$1f,$1f,$1f
; BL
.db $00,$1f,$1f,$1f
.db $00,$0f,$0f,$0f
.db $00,$07,$07,$07
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$80,$00
.db $80,$20,$e0,$a0
.db $c0,$30,$f0,$f0
.db $c0,$30,$f0,$f0
.db $00,$00,$c0,$00
.db $00,$c0,$c0,$c0
; BR
.db $00,$c0,$c0,$c0
.db $00,$80,$80,$80
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Healing potion - red flask, white glass
PotionSprData:
; TL
.db $00,$00,$00,$00
.db $03,$03,$03,$00
.db $00,$00,$07,$00
.db $02,$00,$07,$00
.db $02,$00,$07,$00
.db $07,$00,$0f,$00
.db $0f,$00,$1f,$00
.db $0f,$00,$1f,$00
; BL
.db $0f,$00,$1f,$00
.db $07,$00,$0f,$00
.db $00,$00,$07,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$80,$00
.db $80,$00,$c0,$00
.db $80,$00,$c0,$00
; BR
.db $80,$00,$c0,$00
.db $00,$00,$80,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Wand of fire - diagonal wand with flame tip
WandSprData:
; TL
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $01,$01,$01,$01
.db $01,$01,$01,$00
.db $03,$03,$03,$00
.db $06,$06,$06,$00
.db $0c,$0c,$0c,$00
.db $18,$18,$18,$00
; BL
.db $30,$30,$30,$00
.db $60,$60,$60,$00
.db $40,$40,$40,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $c0,$80,$c0,$80
.db $c0,$40,$c0,$40
.db $c0,$40,$c0,$40
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; BR
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Sharp Stick - simple brown stick, thin diagonal
Weapon1SprData:
; TL
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $01,$01,$01,$00
.db $02,$02,$02,$01
.db $04,$04,$04,$02
.db $08,$08,$08,$04
; BL
.db $10,$10,$10,$08
.db $20,$20,$20,$10
.db $40,$40,$40,$20
.db $40,$40,$40,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $20,$20,$20,$10
.db $40,$40,$40,$20
.db $80,$80,$80,$40
.db $00,$00,$00,$80
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; BR
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Lacquer Blade - proper sword, white blade, red guard
Weapon2SprData:
; TL
.db $00,$00,$00,$00
.db $00,$00,$03,$00
.db $00,$00,$03,$00
.db $00,$00,$03,$00
.db $00,$00,$03,$00
.db $00,$00,$03,$00
.db $00,$00,$03,$00
.db $0c,$00,$0f,$00
; BL
.db $03,$03,$03,$00
.db $03,$03,$03,$00
.db $03,$03,$03,$00
.db $03,$00,$03,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $c0,$00,$c0,$00
; BR
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Bog Oak Mace - heavy flanged head, thick shaft
Weapon3SprData:
; TL
.db $00,$00,$00,$00
.db $07,$00,$07,$07
.db $17,$00,$1f,$17
.db $1f,$00,$1f,$1f
.db $17,$00,$1f,$17
.db $07,$00,$07,$07
.db $03,$03,$03,$00
.db $03,$03,$03,$00
; BL
.db $03,$03,$03,$00
.db $03,$03,$03,$00
.db $03,$00,$03,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $40,$00,$c0,$40
.db $c0,$00,$c0,$c0
.db $40,$00,$c0,$40
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; BR
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Wooden Shield - small round, brown
Shield1SprData:
; TL
.db $00,$00,$00,$00
.db $00,$00,$00,$07
.db $00,$00,$00,$1f
.db $08,$08,$08,$37
.db $10,$10,$10,$2f
.db $08,$08,$08,$37
.db $00,$00,$00,$1f
.db $00,$00,$00,$0e
; BL
.db $04,$04,$04,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$80
.db $80,$80,$80,$40
.db $80,$80,$80,$40
.db $80,$80,$80,$40
.db $00,$00,$00,$80
.db $00,$00,$00,$00
; BR
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Shell Shield - medium, grey with teal boss
Shield2SprData:
; TL
.db $00,$00,$00,$00
.db $07,$00,$07,$07
.db $1f,$00,$1f,$1f
.db $3b,$04,$3f,$3f
.db $38,$07,$3f,$3f
.db $3b,$04,$3f,$3f
.db $1f,$00,$1f,$1f
.db $07,$00,$07,$07
; BL
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $c0,$00,$c0,$c0
.db $a0,$40,$e0,$e0
.db $d0,$20,$f0,$f0
.db $a0,$40,$e0,$e0
.db $c0,$00,$c0,$c0
.db $00,$00,$00,$00
; BR
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Bog Oak Shield - large ornate, gold trim
Shield3SprData:
; TL
.db $00,$00,$00,$00
.db $0f,$08,$0f,$0f
.db $1f,$10,$1f,$1f
.db $3d,$20,$3f,$3d
.db $38,$00,$3f,$38
.db $3d,$20,$3f,$3d
.db $1f,$10,$1f,$1f
.db $0f,$08,$0f,$0f
; BL
.db $07,$07,$07,$07
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $80,$80,$80,$80
.db $c0,$40,$c0,$c0
.db $e0,$20,$e0,$e0
.db $e0,$00,$e0,$e0
.db $e0,$20,$e0,$e0
.db $c0,$40,$c0,$c0
.db $80,$80,$80,$80
; BR
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Reed Armor - light vest
Armor1SprData:
; TL
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$07
.db $00,$00,$00,$1f
.db $06,$06,$06,$19
.db $06,$06,$06,$19
.db $00,$00,$00,$1f
.db $00,$00,$00,$0f
; BL
.db $00,$00,$00,$05
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$80
.db $00,$00,$00,$c0
.db $00,$00,$00,$c0
.db $00,$00,$00,$c0
.db $00,$00,$00,$c0
.db $00,$00,$00,$80
; BR
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Shell Armor - chest plate, grey with teal
Armor2SprData:
; TL
.db $00,$00,$00,$00
.db $00,$05,$05,$05
.db $06,$19,$1f,$1f
.db $0f,$30,$3f,$3f
.db $0d,$32,$3f,$3f
.db $0f,$30,$3f,$3f
.db $06,$19,$1f,$1f
.db $06,$09,$0f,$0f
; BL
.db $00,$06,$06,$06
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$80,$80,$80
.db $80,$60,$e0,$e0
.db $80,$60,$e0,$e0
.db $80,$60,$e0,$e0
.db $00,$80,$80,$80
.db $00,$00,$00,$00
; BR
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Bog Oak Armor - full plate with gold trim
Armor3SprData:
; TL
.db $00,$00,$00,$00
.db $0d,$08,$0d,$0d
.db $1f,$10,$1f,$1f
.db $3f,$20,$3f,$3f
.db $3a,$20,$3f,$3a
.db $3f,$20,$3f,$3f
.db $1f,$10,$1f,$1f
.db $1f,$19,$1f,$1f
; BL
.db $0f,$09,$0f,$0f
.db $06,$06,$06,$06
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $80,$80,$80,$80
.db $80,$80,$80,$80
.db $e0,$20,$e0,$e0
.db $e0,$20,$e0,$e0
.db $e0,$20,$e0,$e0
.db $80,$80,$80,$80
.db $40,$40,$40,$40
; BR
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Dingo frame 1 - pointed ears, brown, lean
DingoSpr1Data:
; TL
.db $00,$00,$00,$00
.db $00,$00,$00,$10
.db $10,$10,$10,$28
.db $00,$00,$00,$3f
.db $00,$00,$08,$37
.db $10,$10,$18,$23
.db $27,$27,$27,$18
.db $33,$33,$33,$0c
; BL
.db $30,$30,$30,$0f
.db $1a,$1a,$1a,$05
.db $1f,$1f,$1f,$00
.db $08,$08,$08,$04
.db $08,$08,$08,$04
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $00,$00,$00,$10
.db $40,$40,$40,$30
.db $00,$00,$00,$f8
.db $00,$00,$40,$bc
.db $84,$84,$a4,$48
.db $88,$88,$88,$74
.db $94,$94,$94,$68
; BR
.db $1c,$1c,$1c,$e0
.db $b0,$b0,$b0,$40
.db $f0,$f0,$f0,$00
.db $40,$40,$40,$20
.db $40,$40,$40,$20
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Dingo frame 2 - sway right
DingoSpr2Data:
; TL
.db $00,$00,$00,$00
.db $00,$00,$00,$08
.db $08,$08,$08,$14
.db $00,$00,$00,$1f
.db $00,$00,$04,$1b
.db $08,$08,$0c,$11
.db $13,$13,$13,$0c
.db $19,$19,$19,$06
; BL
.db $30,$30,$30,$0f
.db $1a,$1a,$1a,$05
.db $1f,$1f,$1f,$00
.db $08,$08,$08,$04
.db $08,$08,$08,$04
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $00,$00,$00,$08
.db $10,$10,$10,$0c
.db $00,$00,$00,$fc
.db $00,$00,$20,$dc
.db $42,$42,$52,$a4
.db $c4,$c4,$c4,$3a
.db $ca,$ca,$ca,$34
; BR
.db $1c,$1c,$1c,$e0
.db $b0,$b0,$b0,$40
.db $f0,$f0,$f0,$00
.db $40,$40,$40,$20
.db $40,$40,$40,$20
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Manul frame 1 - very round, tiny ears, grey/brown
ManulSpr1Data:
; TL
.db $00,$00,$00,$00
.db $20,$00,$20,$20
.db $7f,$00,$7f,$7f
.db $7f,$00,$7f,$7f
.db $4e,$00,$6e,$5f
.db $6f,$20,$6f,$7f
.db $6f,$00,$6a,$7f
.db $00,$00,$00,$1f
; BL
.db $2a,$00,$2a,$3f
.db $40,$00,$40,$7f
.db $35,$00,$35,$3f
.db $19,$00,$19,$19
.db $19,$00,$19,$19
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $20,$00,$20,$20
.db $f8,$00,$f8,$f8
.db $f0,$00,$f0,$f8
.db $e4,$00,$ec,$f4
.db $b8,$10,$b8,$f8
.db $b8,$00,$b8,$f8
.db $18,$00,$18,$d8
; BR
.db $a0,$00,$a0,$e0
.db $20,$00,$20,$e0
.db $60,$00,$60,$e0
.db $80,$00,$80,$80
.db $80,$00,$80,$80
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Manul frame 2 - sway right
ManulSpr2Data:
; TL
.db $00,$00,$00,$00
.db $10,$00,$10,$10
.db $3f,$00,$3f,$3f
.db $3f,$00,$3f,$3f
.db $27,$00,$37,$2f
.db $37,$10,$37,$3f
.db $37,$00,$35,$3f
.db $00,$00,$00,$0f
; BL
.db $15,$00,$15,$1f
.db $20,$00,$20,$3f
.db $1a,$00,$1a,$1f
.db $0c,$00,$0c,$0c
.db $0c,$00,$0c,$0c
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $10,$00,$10,$10
.db $fc,$00,$fc,$fc
.db $f8,$00,$f8,$fc
.db $72,$00,$76,$fa
.db $dc,$08,$dc,$fc
.db $dc,$00,$5c,$fc
.db $0c,$00,$0c,$ec
; BR
.db $50,$00,$50,$f0
.db $10,$00,$10,$f0
.db $b0,$00,$b0,$f0
.db $c0,$00,$c0,$c0
.db $c0,$00,$c0,$c0
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Polish Chicken frame 1 - upright, red crest, white body
ChickenSpr1Data:
; TL
.db $00,$00,$00,$00
.db $07,$00,$07,$00
.db $0f,$00,$0f,$00
.db $00,$00,$1f,$00
.db $00,$00,$1b,$00
.db $00,$00,$0f,$00
.db $04,$04,$0f,$04
.db $00,$00,$0f,$00
; BL
.db $00,$00,$1f,$00
.db $40,$00,$7f,$40
.db $00,$00,$1f,$00
.db $00,$00,$0f,$00
.db $08,$08,$08,$08
.db $08,$08,$08,$08
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $80,$00,$80,$00
.db $00,$00,$c0,$00
.db $00,$00,$40,$00
.db $00,$00,$c0,$00
.db $00,$00,$00,$00
.db $00,$00,$80,$00
; BR
.db $00,$00,$c0,$00
.db $20,$00,$e0,$20
.db $00,$00,$c0,$00
.db $00,$00,$80,$00
.db $80,$80,$80,$80
.db $80,$80,$80,$80
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Polish Chicken frame 2 - sway right
ChickenSpr2Data:
; TL
.db $00,$00,$00,$00
.db $03,$00,$03,$00
.db $07,$00,$07,$00
.db $00,$00,$0f,$00
.db $00,$00,$0d,$00
.db $00,$00,$07,$00
.db $02,$02,$07,$02
.db $00,$00,$07,$00
; BL
.db $00,$00,$0f,$00
.db $20,$00,$3f,$20
.db $00,$00,$0f,$00
.db $00,$00,$07,$00
.db $04,$04,$04,$04
.db $04,$04,$04,$04
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $80,$00,$80,$00
.db $c0,$00,$c0,$00
.db $00,$00,$e0,$00
.db $00,$00,$a0,$00
.db $00,$00,$e0,$00
.db $00,$00,$80,$00
.db $00,$00,$c0,$00
; BR
.db $00,$00,$e0,$00
.db $20,$00,$f0,$20
.db $00,$00,$e0,$00
.db $00,$00,$c0,$00
.db $40,$40,$40,$40
.db $40,$40,$40,$40
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; Fireball - bright flame, red/yellow/white core
FireballSprData:
; TL
.db $00,$00,$00,$00
.db $03,$00,$03,$00
.db $07,$01,$07,$01
.db $0f,$03,$0f,$03
.db $1f,$07,$1f,$07
.db $19,$09,$1f,$09
.db $39,$19,$3f,$19
.db $39,$29,$3f,$29
; BL
.db $39,$19,$3f,$19
.db $1f,$0f,$1f,$0f
.db $0f,$03,$0f,$03
.db $07,$01,$07,$01
.db $03,$00,$03,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
; TR
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $80,$00,$80,$00
.db $c0,$00,$c0,$00
.db $c0,$80,$c0,$80
.db $c0,$00,$c0,$00
.db $c0,$80,$c0,$80
.db $c0,$40,$c0,$40
; BR
.db $c0,$80,$c0,$80
.db $80,$00,$80,$00
.db $c0,$00,$c0,$00
.db $80,$00,$80,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00
.db $00,$00,$00,$00

SpriteTileDataEnd:
RaceSpriteTable:
.dw PlayerSpr1Data, PlayerSpr2Data    ; Race 0: Frog
.dw DingoSpr1Data, DingoSpr2Data      ; Race 1: Dingo
.dw ManulSpr1Data, ManulSpr2Data      ; Race 2: Manul
.dw ChickenSpr1Data, ChickenSpr2Data  ; Race 3: Polish Chicken
