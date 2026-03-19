;==============================================================
; Rendering routines
;==============================================================

;--------------------------------------------------------------
; WriteTileToVRAM - write single tile to name table
; Input: B=X, C=Y, A=ASCII char, D=attribute
; Clobbers: HL, BC
;--------------------------------------------------------------
WriteTileToVRAM:
    push af
    push de
    ; Calculate VRAM address = $3800 + (Y*32 + X) * 2
    ld h,0
    ld l,c
    add hl,hl
    add hl,hl
    add hl,hl
    add hl,hl
    add hl,hl              ; Y * 32
    ld c,b
    ld b,0
    add hl,bc              ; + X
    add hl,hl              ; * 2 (word entries)
    ld bc,$3800
    add hl,bc              ; + name table base
    ; Prepare data before critical section
    pop de                  ; D = attribute
    pop af                  ; A = ASCII char
    sub $20                 ; to tile index
    ld b,a                  ; B = tile index
    ld c,d                  ; C = attribute
    ; Critical section: all port I/O
    di
    ld a,l
    out ($bf),a
    ld a,h
    or $40
    out ($bf),a
    ld a,b
    out ($be),a
    ld a,c
    out ($be),a
    ei
    ret

;--------------------------------------------------------------
; ClearNameTable - fill entire name table with zeroes
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
; RenderFullMap - write MapBuffer + entities to VRAM
; Call with display OFF
;--------------------------------------------------------------
RenderFullMap:
    di
    ld a,$00
    out ($bf),a
    ld a,$38 | $40
    out ($bf),a
    ; Write 22 rows * 32 cols of terrain
    ld hl,MapBuffer
    ld de,MAP_SIZE
_rfm_loop:
    ld a,(hl)
    push af
    sub $20
    out ($be),a             ; tile index
    pop af
    cp '>'                  ; stairs get bright palette
    jr nz,_rfm_pal0
    ld a,ATTR_PAL1
    jr _rfm_attr
_rfm_pal0:
    ld a,ATTR_PAL0
_rfm_attr:
    out ($be),a
    inc hl
    dec de
    ld a,d
    or e
    jr nz,_rfm_loop
    ei
    ; Overlay entities
    call DrawPlayer
    call DrawAllEnemies
    ret

;--------------------------------------------------------------
; RenderHUD - draw status line on row 22
;--------------------------------------------------------------
RenderHUD:
    di
    ; Row 22 VRAM = $3800 + 22*64 = $3D80
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
    ; PlayerHP
    ld a,(PlayerHP)
    call WriteNumber2
    ; "/"
    ld a,'/'-$20
    out ($be),a
    xor a
    out ($be),a
    ; MaxHP
    ld a,(PlayerMaxHP)
    call WriteNumber2
    ; "  Fl:"
    xor a
    out ($be),a
    out ($be),a
    out ($be),a
    out ($be),a
    ld a,'F'-$20
    out ($be),a
    xor a
    out ($be),a
    ld a,'l'-$20
    out ($be),a
    xor a
    out ($be),a
    ld a,':'-$20
    out ($be),a
    xor a
    out ($be),a
    ; Level number
    ld a,(DungeonLevel)
    call WriteNumber1
    ei
    ret

;--------------------------------------------------------------
; WriteNumber2 - write 2-digit number (0-99) to VRAM
;--------------------------------------------------------------
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

;--------------------------------------------------------------
; WriteNumber1 - write 1-digit number to VRAM
;--------------------------------------------------------------
WriteNumber1:
    add a,'0'-$20
    out ($be),a
    xor a
    out ($be),a
    ret

;--------------------------------------------------------------
; EraseEntities - restore terrain under all entities
;--------------------------------------------------------------
EraseEntities:
    ; Erase player
    ld a,(PlayerX)
    ld b,a
    ld a,(PlayerY)
    ld c,a
    push bc
    call GetMapTile
    pop bc
    ld d,ATTR_PAL0
    cp '>'
    jr nz,+
    ld d,ATTR_PAL1
+:
    call WriteTileToVRAM
    ; Erase enemies
    ld ix,EnemyData
    ld e,MAX_ENEMIES
_ee_loop:
    ld a,(ix+ENEMY_TYPE)
    or a
    jr z,_ee_skip
    ld b,(ix+ENEMY_X)
    ld c,(ix+ENEMY_Y)
    push de
    push bc
    call GetMapTile
    pop bc
    ld d,ATTR_PAL0
    cp '>'
    jr nz,+
    ld d,ATTR_PAL1
+:
    call WriteTileToVRAM
    pop de
_ee_skip:
    push de
    ld bc,ENEMY_SIZE
    add ix,bc
    pop de
    dec e
    jr nz,_ee_loop
    ret

;--------------------------------------------------------------
; DrawEntities - draw all entities at current positions
;--------------------------------------------------------------
DrawEntities:
    call DrawPlayer
    call DrawAllEnemies
    ret

;--------------------------------------------------------------
; DrawPlayer
;--------------------------------------------------------------
DrawPlayer:
    ld a,(PlayerX)
    ld b,a
    ld a,(PlayerY)
    ld c,a
    ld a,'@'
    ld d,ATTR_PAL1
    call WriteTileToVRAM
    ret

;--------------------------------------------------------------
; DrawAllEnemies
;--------------------------------------------------------------
DrawAllEnemies:
    ld ix,EnemyData
    ld e,MAX_ENEMIES
_dae_loop:
    ld a,(ix+ENEMY_TYPE)
    or a
    jr z,_dae_skip
    push de
    call GetEnemyChar
    ld b,(ix+ENEMY_X)
    ld c,(ix+ENEMY_Y)
    ld d,ATTR_PAL1
    call WriteTileToVRAM
    pop de
_dae_skip:
    push de
    ld bc,ENEMY_SIZE
    add ix,bc
    pop de
    dec e
    jr nz,_dae_loop
    ret

;--------------------------------------------------------------
; GetEnemyChar - ASCII char for enemy type
; Input: (ix+ENEMY_TYPE)
; Output: A = char
;--------------------------------------------------------------
GetEnemyChar:
    ld a,(ix+ENEMY_TYPE)
    cp 1
    jr z,_gec_rat
    cp 2
    jr z,_gec_snake
    ld a,'o'
    ret
_gec_rat:
    ld a,'r'
    ret
_gec_snake:
    ld a,'s'
    ret

;--------------------------------------------------------------
; PrintString - null-terminated ASCII string to name table
; Input: B=startX, C=startY, HL=string
; Clobbers: AF, HL, BC
;--------------------------------------------------------------
PrintString:
    push hl
    ; Calculate VRAM address
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
