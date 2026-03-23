;==============================================================
; Rendering routines - 16x16 tile system with sprites
;==============================================================

;--------------------------------------------------------------
; MapCharToTileBG - map ASCII to background tile index
; For items, show floor (items are sprites)
; Input: A = ASCII char
; Output: A = bg tile index, C = attribute
;--------------------------------------------------------------
MapCharToTileBG:
    ld c,ATTR_PAL0
    cp '.'
    jr z,_mctb_floor
    cp '>'
    jr z,_mctb_stairs
    ; Items, walls, and unknown -> show floor underneath
    ; (walls are handled separately via WriteWallTile16)
    ld a,BG_FLOOR
    ret
_mctb_floor:
    ld a,BG_FLOOR
    ret
_mctb_stairs:
    ld c,ATTR_PAL1
    ld a,0              ; special: stairs uses 4 tiles
    ret

;--------------------------------------------------------------
; WriteTile16 - write a 16x16 logical tile to name table
; Input: B=logical_X (0-15), C=logical_Y (0-10), A=ASCII char
; Handles 2x2 hardware tile block
;--------------------------------------------------------------
WriteTile16:
    push bc
    push af
    call MapCharToTileBG    ; A=tile, C=attr
    pop af                  ; restore original char for stairs check
    pop bc
    cp '>'
    jr z,_wt16_stairs
    ; Floor (and items-as-floor): same tile in all 4 positions
    push bc
    ld d,BG_FLOOR
    ld e,ATTR_PAL0
    pop bc
    jr _wt16_uniform

_wt16_uniform:
    ; D=tile, E=attr - write same tile in all 4 positions
    ; Top row
    push bc
    ld a,c
    add a,a
    ld c,a
    ld a,b
    add a,a
    ld b,a
    di
    call _wt16_addr
    ld a,d
    out ($be),a
    ld a,e
    out ($be),a
    ld a,d
    out ($be),a
    ld a,e
    out ($be),a
    ei
    pop bc
    ; Bottom row
    push bc
    ld a,c
    add a,a
    inc a
    ld c,a
    ld a,b
    add a,a
    ld b,a
    di
    call _wt16_addr
    ld a,d
    out ($be),a
    ld a,e
    out ($be),a
    ld a,d
    out ($be),a
    ld a,e
    out ($be),a
    ei
    pop bc
    ret

_wt16_stairs:
    ; Stairs: 4 distinct tiles
    push bc
    ld a,c
    add a,a
    ld c,a
    ld a,b
    add a,a
    ld b,a
    di
    call _wt16_addr
    ld a,BG_STAIR_TL
    out ($be),a
    ld a,ATTR_PAL1
    out ($be),a
    ld a,BG_STAIR_TR
    out ($be),a
    ld a,ATTR_PAL1
    out ($be),a
    ei
    pop bc
    push bc
    ld a,c
    add a,a
    inc a
    ld c,a
    ld a,b
    add a,a
    ld b,a
    di
    call _wt16_addr
    ld a,BG_STAIR_BL
    out ($be),a
    ld a,ATTR_PAL1
    out ($be),a
    ld a,BG_STAIR_BR
    out ($be),a
    ld a,ATTR_PAL1
    out ($be),a
    ei
    pop bc
    ret

; Helper: set VRAM write address for hw tile (B=hw_x, C=hw_y)
_wt16_addr:
    ld h,0
    ld l,c
    add hl,hl
    add hl,hl
    add hl,hl
    add hl,hl
    add hl,hl              ; hw_y * 32
    ld a,b
    ld c,a
    ld b,0
    add hl,bc              ; + hw_x
    add hl,hl              ; * 2
    ld bc,$3800
    add hl,bc
    ld a,l
    out ($bf),a
    ld a,h
    or $40
    out ($bf),a
    ret

;--------------------------------------------------------------
; ClearNameTable
;--------------------------------------------------------------
ClearNameTable:
    di
    ld a,$00
    out ($bf),a
    ld a,$38 | $40
    out ($bf),a
    ld bc,32 * 24 * 2
_cnt_loop:
    xor a
    out ($be),a
    dec bc
    ld a,b
    or c
    jr nz,_cnt_loop
    ei
    ret

;--------------------------------------------------------------
; GetWallNeighborMask - check cardinal neighbors of a wall cell
; Input: D=logical X, E=logical Y
; Output: A = 4-bit mask (bit3=up open, bit2=right, bit1=down, bit0=left)
; "Open" = anything that is not '#'
; Clobbers: HL, BC
;--------------------------------------------------------------
GetWallNeighborMask:
    ; Compute center address: HL = MapBuffer + E*16 + D
    ld h,0
    ld l,e
    add hl,hl
    add hl,hl
    add hl,hl
    add hl,hl              ; Y * 16
    ld a,d
    ld c,a
    ld b,0
    add hl,bc              ; + X
    ld bc,MapBuffer
    add hl,bc              ; HL = &MapBuffer[Y*16+X]
    ld c,0                  ; C = mask accumulator
    ; Check UP (HL - 16)
    ld a,e
    or a
    jr z,_gnm_noUp          ; Y=0, border = wall
    push hl
    ld a,c                  ; save mask
    ld bc,-16
    add hl,bc
    ld c,a                  ; restore mask
    ld a,(hl)
    pop hl
    cp '#'
    jr z,_gnm_noUp
    set 3,c
_gnm_noUp:
    ; Check RIGHT (HL + 1)
    ld a,d
    cp MAP_WIDTH-1
    jr z,_gnm_noRight       ; X=15, border = wall
    inc hl
    ld a,(hl)
    dec hl
    cp '#'
    jr z,_gnm_noRight
    set 2,c
_gnm_noRight:
    ; Check DOWN (HL + 16)
    ld a,e
    cp MAP_HEIGHT-1
    jr z,_gnm_noDown        ; Y=10, border = wall
    push hl
    ld a,c                  ; save mask
    ld bc,16
    add hl,bc
    ld c,a                  ; restore mask
    ld a,(hl)
    pop hl
    cp '#'
    jr z,_gnm_noDown
    set 1,c
_gnm_noDown:
    ; Check LEFT (HL - 1)
    ld a,d
    or a
    jr z,_gnm_noLeft        ; X=0, border = wall
    dec hl
    ld a,(hl)
    cp '#'
    jr z,_gnm_noLeft
    set 0,c
_gnm_noLeft:
    ld a,c                  ; return mask in A
    ret

;--------------------------------------------------------------
; WriteWallTile16 - write contextual dirt wall tile
; Input: B=logical_X, C=logical_Y, A=neighbor mask (0-15)
; Uses DE to carry table pointer across _wt16_addr calls
;--------------------------------------------------------------
WriteWallTile16:
    ; A = neighbor mask, B = logical X, C = logical Y
    or a
    jp nz,_wwt_lookup       ; non-zero mask → use lookup table
    ; ---- Fully interior wall: check greeble buffer ----
    call CheckGreeble       ; B,C preserved; A = tile or 0
    or a
    jr z,_wwt_plain         ; no greeble → plain interior
    ; A = greeble tile. Place in one quadrant.
    ld d,a                  ; D = greeble tile
    ld a,b
    xor c
    and $03
    ld e,a                  ; E = quadrant (0=TL,1=TR,2=BL,3=BR)
    ; --- Write top row ---
    push bc
    ld a,c
    add a,a
    ld c,a
    ld a,b
    add a,a
    ld b,a
    di
    call _wt16_addr         ; DE preserved
    ; TL
    ld a,e
    or a
    jr nz,_wg_tl_s
    ld a,d
    jr _wg_tl_w
_wg_tl_s:
    ld a,BG_DIRT_SOLID
_wg_tl_w:
    out ($be),a
    xor a
    out ($be),a
    ; TR
    ld a,e
    dec a
    jr nz,_wg_tr_s
    ld a,d
    jr _wg_tr_w
_wg_tr_s:
    ld a,BG_DIRT_SOLID_V
_wg_tr_w:
    out ($be),a
    xor a
    out ($be),a
    ei
    ; --- Write bottom row ---
    pop bc
    ld a,c
    add a,a
    inc a
    ld c,a
    ld a,b
    add a,a
    ld b,a
    di
    call _wt16_addr
    ; BL
    ld a,e
    cp 2
    jr nz,_wg_bl_s
    ld a,d
    jr _wg_bl_w
_wg_bl_s:
    ld a,BG_DIRT_SOLID_V
_wg_bl_w:
    out ($be),a
    xor a
    out ($be),a
    ; BR
    ld a,e
    cp 3
    jr nz,_wg_br_s
    ld a,d
    jr _wg_br_w
_wg_br_s:
    ld a,BG_DIRT_SOLID
_wg_br_w:
    out ($be),a
    xor a
    out ($be),a
    ei
    ret

_wwt_plain:
    xor a                   ; restore mask = 0
_wwt_lookup:
    ; Compute table pointer: DE = WallQuadrantTable + mask*8
    push bc                 ; save position
    add a,a
    add a,a
    add a,a                 ; mask * 8
    ld c,a
    ld b,0
    ld hl,WallQuadrantTable
    add hl,bc
    ld d,h
    ld e,l                  ; DE = table entry pointer
    pop bc                  ; B=logical X, C=logical Y
    ; --- Top row ---
    push bc                 ; save position
    ld a,c
    add a,a
    ld c,a
    ld a,b
    add a,a
    ld b,a                  ; B=hw_x, C=hw_y
    di
    call _wt16_addr         ; set VRAM address (clobbers HL,BC; DE safe)
    ld a,(de)
    out ($be),a
    inc de                  ; TL tile
    ld a,(de)
    out ($be),a
    inc de                  ; TL attr
    ld a,(de)
    out ($be),a
    inc de                  ; TR tile
    ld a,(de)
    out ($be),a
    inc de                  ; TR attr
    ei
    ; --- Bottom row ---
    pop bc                  ; B=logical X, C=logical Y
    ld a,c
    add a,a
    inc a
    ld c,a                  ; hw_y = Y*2 + 1
    ld a,b
    add a,a
    ld b,a                  ; hw_x = X*2
    di
    call _wt16_addr
    ld a,(de)
    out ($be),a
    inc de                  ; BL tile
    ld a,(de)
    out ($be),a
    inc de                  ; BL attr
    ld a,(de)
    out ($be),a
    inc de                  ; BR tile
    ld a,(de)
    out ($be),a             ; BR attr
    ei
    ret

;--------------------------------------------------------------
; RenderFullMap - write entire MapBuffer as 16x16 bg tiles
; Call with display OFF
;--------------------------------------------------------------
RenderFullMap:
    ld e,0                  ; logical Y
_rfm_yloop:
    ld a,e
    cp MAP_HEIGHT
    jr z,_rfm_done
    ld d,0                  ; logical X
_rfm_xloop:
    ld a,d
    cp MAP_WIDTH
    jr z,_rfm_nextrow
    ; Get map tile
    ld b,d
    ld c,e
    push de
    call GetMapTile         ; A = ASCII char
    pop de
    cp '#'
    jr nz,_rfm_notWall
    ; Wall: contextual dirt tile
    push de
    call GetWallNeighborMask ; A = 4-bit mask, D=X E=Y preserved on stack
    pop de
    push de
    ld b,d
    ld c,e
    call WriteWallTile16    ; B=X, C=Y, A=mask
    pop de
    inc d
    jr _rfm_xloop
_rfm_notWall:
    push de
    ld b,d
    ld c,e
    call WriteTile16
    pop de
    inc d
    jr _rfm_xloop
_rfm_nextrow:
    inc e
    jr _rfm_yloop
_rfm_done:
    ret

;--------------------------------------------------------------
; RenderHUD - status bar on hw rows 22-23
;--------------------------------------------------------------
RenderHUD:
    di
    ld a,$80
    out ($bf),a
    ld a,$3D | $40
    out ($bf),a
    ; "HP:"
    ld a,'H'-$20
    out ($be),a
    xor a
    out ($be),a
    ld a,'P'-$20
    out ($be),a
    xor a
    out ($be),a
    ld a,':'-$20
    out ($be),a
    xor a
    out ($be),a
    ld a,(PlayerHP)
    call WriteNumber2
    ld a,'/'-$20
    out ($be),a
    xor a
    out ($be),a
    ld a,(PlayerMaxHP)
    call WriteNumber2
    ; " Fl:"
    xor a
    out ($be),a
    out ($be),a
    ld a,'F'-$20
    out ($be),a
    xor a
    out ($be),a
    ld a,':'-$20
    out ($be),a
    xor a
    out ($be),a
    ld a,(DungeonLevel)
    call WriteNumber1
    ; Equipment icons (using 8x8 tiles from sprite VRAM won't work for bg)
    ; Show text indicators instead
    xor a
    out ($be),a
    out ($be),a
    xor a
    out ($be),a
    out ($be),a
    ; Weapon indicator
    ld a,(PlayerWeapon)
    or a
    jr z,_hud_noWpn
    ld a,'+'-$20
    out ($be),a
    xor a
    out ($be),a
    jr _hud_arm
_hud_noWpn:
    ld a,'-'-$20
    out ($be),a
    xor a
    out ($be),a
_hud_arm:
    ; Armor indicator
    ld a,(PlayerArmor)
    or a
    jr z,_hud_noArm
    ld a,'+'-$20
    out ($be),a
    xor a
    out ($be),a
    jr _hud_end
_hud_noArm:
    ld a,'-'-$20
    out ($be),a
    xor a
    out ($be),a
_hud_end:
    ei
    ret

WriteNumber2:
    ld b,0
_wn2_div:
    cp 10
    jr c,_wn2_done
    sub 10
    inc b
    jr _wn2_div
_wn2_done:
    ld c,a
    ld a,b
    add a,'0'-$20
    out ($be),a
    xor a
    out ($be),a
    ld a,c
    add a,'0'-$20
    out ($be),a
    xor a
    out ($be),a
    ret

WriteNumber1:
    add a,'0'-$20
    out ($be),a
    xor a
    out ($be),a
    ret

;--------------------------------------------------------------
; SPRITE MANAGEMENT
;--------------------------------------------------------------

; Hide all sprites by writing $D0 to first Y entry
InitSprites:
    di
    ld a,$00
    out ($bf),a
    ld a,$3F | $40          ; SAT Y table at $3F00
    out ($bf),a
    ld a,$D0                ; terminator
    out ($be),a
    ei
    ; Clear RAM buffer
    ld a,$D0
    ld (SatBufY),a
    xor a
    ld (NumSprites),a
    ret

; Copy SAT buffer to VRAM - called from VBlank handler
CopySATToVRAM:
    ; Copy Y coordinates to $3F00
    di
    ld a,$00
    out ($bf),a
    ld a,$3F | $40
    out ($bf),a
    ld a,(NumSprites)
    or a
    jr z,_csat_term
    ld b,a
    ld hl,SatBufY
_csat_yloop:
    ld a,(hl)
    out ($be),a
    inc hl
    djnz _csat_yloop
_csat_term:
    ld a,$D0                ; terminator after last sprite
    out ($be),a
    ; Copy X/tile pairs to $3F80
    ld a,$80
    out ($bf),a
    ld a,$3F | $40
    out ($bf),a
    ld a,(NumSprites)
    or a
    jr z,_csat_done
    add a,a                 ; * 2 (2 bytes per sprite)
    ld b,a
    ld hl,SatBufXN
_csat_xloop:
    ld a,(hl)
    out ($be),a
    inc hl
    djnz _csat_xloop
_csat_done:
    ei
    ret

; BuildSpriteTable - rebuild SAT from game state
BuildSpriteTable:
    xor a
    ld (NumSprites),a

    ; --- Player ---
    ld a,(PlayerPixelX)
    ld d,a
    ld a,(PlayerPixelY)
    ld e,a
    ld a,(AnimFrame)
    or a
    jr nz,_bst2_pf2
    ld c,SPR_PLAYER1
    jr _bst2_padd
_bst2_pf2:
    ld c,SPR_PLAYER2
_bst2_padd:
    call AddSprite16Simple

    ; --- Enemies ---
    ld hl,EnemyData
    ld b,MAX_ENEMIES
_bst2_eloop:
    push bc
    push hl
    inc hl
    inc hl
    inc hl
    ld a,(hl)              ; ENEMY_TYPE
    pop hl
    or a
    jr z,_bst2_eskip
    ; Pixel X = EnemyX * 16
    ld a,(hl)              ; ENEMY_X
    add a,a
    add a,a
    add a,a
    add a,a
    ld d,a
    inc hl
    ld a,(hl)              ; ENEMY_Y
    add a,a
    add a,a
    add a,a
    add a,a
    ld e,a
    inc hl
    inc hl
    ld a,(hl)              ; ENEMY_TYPE
    dec hl
    dec hl
    dec hl
    push hl
    call GetEnemySprBase   ; A = sprite tile base
    ld c,a
    call AddSprite16Simple
    pop hl
    jr _bst2_enext
_bst2_eskip:
_bst2_enext:
    push de
    ld de,ENEMY_SIZE
    add hl,de
    pop de
    pop bc
    djnz _bst2_eloop

    ; --- Items on ground (scan MapBuffer) ---
    ld hl,MapBuffer
    ld d,0                  ; logical Y
_bst2_iy:
    ld a,d
    cp MAP_HEIGHT
    jr z,_bst2_idone
    ld e,0                  ; logical X
_bst2_ix:
    ld a,e
    cp MAP_WIDTH
    jr z,_bst2_iny
    ld a,(hl)
    call IsItemTile
    jr nz,_bst2_inext
    ; Item found! Create sprites
    push hl
    push de
    ld a,e                  ; logical X
    add a,a
    add a,a
    add a,a
    add a,a                 ; * 16 = pixel X
    push af
    ld a,d                  ; logical Y
    add a,a
    add a,a
    add a,a
    add a,a                 ; * 16 = pixel Y
    ld e,a
    pop af
    ld d,a
    ; Determine sprite base from map char
    ld a,(hl)
    call GetItemSprBase
    ld c,a
    call AddSprite16Simple
    pop de
    pop hl
_bst2_inext:
    inc hl
    inc e
    jr _bst2_ix
_bst2_iny:
    inc d
    jr _bst2_iy
_bst2_idone:
    ret

; Helper: Add a 16x16 sprite (2 actual sprites in 8x16 mode)
; D=pixelX, E=pixelY, C=sprite_tile_base
; Uses NumSprites as index
AddSprite16Simple:
    ld a,(NumSprites)
    cp MAX_SPRITES
    ret nc                  ; no room
    ; Sprite 1 (left half)
    push de
    push bc
    ld b,a                  ; B = sprite index
    ; Y coordinate
    ld hl,SatBufY
    ld c,b
    ld b,0
    add hl,bc
    ld (hl),e               ; Y
    ; X and tile
    pop bc                  ; C = tile base
    pop de
    push de
    push bc
    ld a,(NumSprites)
    add a,a                 ; * 2
    ld hl,SatBufXN
    ld b,0
    ld c,a
    add hl,bc
    ld (hl),d               ; X
    inc hl
    pop bc
    ld (hl),c               ; tile base (left column)
    ; Sprite 2 (right half)
    ld a,(NumSprites)
    inc a
    ld (NumSprites),a
    ; Y coordinate
    pop de
    push de
    push bc
    ld hl,SatBufY
    ld c,a
    ld b,0
    add hl,bc
    ld (hl),e               ; same Y
    ; X and tile
    ld a,(NumSprites)
    add a,a
    ld hl,SatBufXN
    ld b,0
    ld c,a
    add hl,bc
    pop bc
    pop de
    ld a,d
    add a,8                 ; X + 8 for right half
    ld (hl),a
    inc hl
    ld a,c
    add a,2                 ; tile base + 2 for right column
    ld (hl),a
    ; Update count
    ld a,(NumSprites)
    inc a
    ld (NumSprites),a
    ret

;--------------------------------------------------------------
; GetEnemySprBase - sprite tile base for enemy type
;--------------------------------------------------------------
GetEnemySprBase:
    cp 1
    jr z,_gesb_rat
    cp 2
    jr z,_gesb_snake
    cp 4
    jr z,_gesb_badger
    ld a,SPR_ORC
    ret
_gesb_rat:
    ld a,SPR_RAT
    ret
_gesb_snake:
    ld a,SPR_SNAKE
    ret
_gesb_badger:
    ld a,SPR_BADGER
    ret

;--------------------------------------------------------------
; GetItemSprBase - sprite tile base for item on ground
;--------------------------------------------------------------
GetItemSprBase:
    ; Per-tier weapons
    cp 'w'
    jr z,_gisb_w1
    cp 'x'
    jr z,_gisb_w2
    cp 'y'
    jr z,_gisb_w3
    ; Per-tier shields
    cp 's'
    jr z,_gisb_s1
    cp 'S'
    jr z,_gisb_s2
    cp 'T'
    jr z,_gisb_s3
    ; Per-tier armor
    cp 'a'
    jr z,_gisb_a1
    cp 'A'
    jr z,_gisb_a2
    cp 'B'
    jr z,_gisb_a3
    cp 'h'
    jr z,_gisb_s1
    cp 'p'
    jr z,_gisb_pot
    cp 'f'
    jr z,_gisb_wand
    ld a,SPR_WEAPON1
    ret
_gisb_w1: ld a,SPR_WEAPON1
    ret
_gisb_w2: ld a,SPR_WEAPON2
    ret
_gisb_w3: ld a,SPR_WEAPON3
    ret
_gisb_s1: ld a,SPR_SHIELD1
    ret
_gisb_s2: ld a,SPR_SHIELD2
    ret
_gisb_s3: ld a,SPR_SHIELD3
    ret
_gisb_a1: ld a,SPR_ARMOR1
    ret
_gisb_a2: ld a,SPR_ARMOR2
    ret
_gisb_a3: ld a,SPR_ARMOR3
    ret
_gisb_pot: ld a,SPR_POTION
    ret
_gisb_wand: ld a,SPR_WAND
    ret

;--------------------------------------------------------------
; UpdatePlayerSprite - update player sprite positions in SAT
; Uses PlayerPixelX/Y and AnimFrame
;--------------------------------------------------------------
UpdatePlayerSprite:
    ; Player is always sprites 0 and 1
    ld a,(PlayerPixelY)
    ld (SatBufY+0),a
    ld (SatBufY+1),a
    ld a,(PlayerPixelX)
    ld (SatBufXN+0),a
    add a,8
    ld (SatBufXN+2),a
    ; Set tile based on animation frame
    ld a,(AnimFrame)
    or a
    jr nz,_ups_f2
    ld a,SPR_PLAYER1
    jr _ups_set
_ups_f2:
    ld a,SPR_PLAYER2
_ups_set:
    ld (SatBufXN+1),a
    add a,2
    ld (SatBufXN+3),a
    ret

;--------------------------------------------------------------
; PrintString - null-terminated ASCII string to name table
; Input: B=startX, C=startY, HL=string (uses 8x8 font tiles)
;--------------------------------------------------------------
PrintString:
    push hl
    ld h,0
    ld l,c
    add hl,hl
    add hl,hl
    add hl,hl
    add hl,hl
    add hl,hl
    ld c,b
    ld b,0
    add hl,bc
    add hl,hl
    ld bc,$3800
    add hl,bc
    di
    ld a,l
    out ($bf),a
    ld a,h
    or $40
    out ($bf),a
    pop hl
_ps_loop:
    ld a,(hl)
    or a
    jr z,_ps_done
    sub $20
    out ($be),a
    xor a
    out ($be),a
    inc hl
    jr _ps_loop
_ps_done:
    ei
    ret

;--------------------------------------------------------------
; DrawRootBorder - draw root/vine frame around a rectangle
; Input: B=x, C=y (top-left hw tile), D=width, E=height (in hw tiles)
; Uses palette 0
;--------------------------------------------------------------
DrawRootBorder:
    ; Save params to RAM variables (NOT ROM!)
    ld a,b
    ld (BorderX),a
    ld a,c
    ld (BorderY),a
    ld a,d
    ld (BorderW),a
    ld a,e
    ld (BorderH),a

    ; --- 1. Fill interior with black ---
    ld a,(BorderH)
    sub 2
    ld e,a                  ; inner height counter
    ld a,(BorderY)
    inc a
    ld c,a                  ; start row
_drb_fillRow:
    ld a,e
    or a
    jr z,_drb_fillDone
    ld a,(BorderW)
    sub 2
    ld d,a                  ; inner width counter
    ld a,(BorderX)
    inc a
    ld b,a                  ; start col
_drb_fillCol:
    ld a,d
    or a
    jr z,_drb_fillNext
    push de
    push bc
    ld a,(BorderPriority)
    or a
    jr z,+
    ld a,BG_SOLID_BLACK
    jr ++
+:  xor a                   ; tile 0 for non-priority
++: call _drb_writeTile
    pop bc
    pop de
    inc b
    dec d
    jr _drb_fillCol
_drb_fillNext:
    inc c
    dec e
    jr _drb_fillRow
_drb_fillDone:

    ; --- 2. Corners ---
    ld a,(BorderX)
    ld b,a
    ld a,(BorderY)
    ld c,a
    ld a,BG_ROOT_TL
    call _drb_writeTile

    ld a,(BorderX)
    ld b,a
    ld a,(BorderW)
    dec a
    add a,b
    ld b,a
    ld a,(BorderY)
    ld c,a
    ld a,BG_ROOT_TR
    call _drb_writeTile

    ld a,(BorderX)
    ld b,a
    ld a,(BorderY)
    ld c,a
    ld a,(BorderH)
    dec a
    add a,c
    ld c,a
    ld a,BG_ROOT_BL
    call _drb_writeTile

    ld a,(BorderX)
    ld b,a
    ld a,(BorderW)
    dec a
    add a,b
    ld b,a
    ld a,(BorderY)
    ld c,a
    ld a,(BorderH)
    dec a
    add a,c
    ld c,a
    ld a,BG_ROOT_BR
    call _drb_writeTile

    ; --- 3. Top edge ---
    ld a,(BorderW)
    sub 2
    ld e,a
    ld a,(BorderX)
    inc a
    ld b,a
    ld a,(BorderY)
    ld c,a
_drb_topLoop:
    ld a,e
    or a
    jr z,_drb_topDone
    push bc
    push de
    ld a,b
    call _drb_mod3
    jr nz,+
    ld a,BG_ROOT_T2
    jr ++
+:  ld a,BG_ROOT_T
++: call _drb_writeTile
    pop de
    pop bc
    inc b
    dec e
    jr _drb_topLoop
_drb_topDone:

    ; --- 4. Bottom edge ---
    ld a,(BorderW)
    sub 2
    ld e,a
    ld a,(BorderX)
    inc a
    ld b,a
    ld a,(BorderY)
    ld c,a
    ld a,(BorderH)
    dec a
    add a,c
    ld c,a
_drb_botLoop:
    ld a,e
    or a
    jr z,_drb_botDone
    push bc
    push de
    ld a,b
    call _drb_mod3
    jr nz,+
    ld a,BG_ROOT_B2
    jr ++
+:  ld a,BG_ROOT_B
++: call _drb_writeTile
    pop de
    pop bc
    inc b
    dec e
    jr _drb_botLoop
_drb_botDone:

    ; --- 5. Left edge ---
    ld a,(BorderH)
    sub 2
    ld e,a
    ld a,(BorderX)
    ld b,a
    ld a,(BorderY)
    inc a
    ld c,a
_drb_leftLoop:
    ld a,e
    or a
    jr z,_drb_leftDone
    push bc
    push de
    ld a,c
    call _drb_mod3
    jr nz,+
    ld a,BG_ROOT_L2
    jr ++
+:  ld a,BG_ROOT_L
++: call _drb_writeTile
    pop de
    pop bc
    inc c
    dec e
    jr _drb_leftLoop
_drb_leftDone:

    ; --- 6. Right edge ---
    ld a,(BorderH)
    sub 2
    ld e,a
    ld a,(BorderX)
    ld b,a
    ld a,(BorderW)
    dec a
    add a,b
    ld b,a
    ld a,(BorderY)
    inc a
    ld c,a
_drb_rightLoop:
    ld a,e
    or a
    jr z,_drb_rightDone
    push bc
    push de
    ld a,c
    call _drb_mod3
    jr nz,+
    ld a,BG_ROOT_R2
    jr ++
+:  ld a,BG_ROOT_R
++: call _drb_writeTile
    pop de
    pop bc
    inc c
    dec e
    jr _drb_rightLoop
_drb_rightDone:
    ret

; Helper: A mod 3. Sets Z flag if result == 0.
; Preserves B,C.
_drb_mod3:
-:  cp 3
    jr c,+
    sub 3
    jr -
+:  or a               ; set Z if remainder == 0
    ret

; Helper: write single 8x8 bg tile at hw position B,C with tile index A
_drb_writeTile:
    push af
    push bc
    ; Calculate VRAM address
    ld h,0
    ld l,c
    add hl,hl
    add hl,hl
    add hl,hl
    add hl,hl
    add hl,hl              ; Y * 32
    ld c,b
    ld b,0
    add hl,bc
    add hl,hl              ; * 2
    ld bc,$3800
    add hl,bc
    di
    ld a,l
    out ($bf),a
    ld a,h
    or $40
    out ($bf),a
    pop bc
    pop af
    out ($be),a             ; tile index
    push af
    ld a,(BorderPriority)
    or a
    jr z,+
    ld a,ATTR_PAL0 | $10    ; priority bit = BG in front of sprites
    jr ++
+:  ld a,ATTR_PAL0
++: out ($be),a             ; attribute
    pop af
    ei
    ret

