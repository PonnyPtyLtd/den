;==============================================================
; Enemy spawning and AI
;==============================================================

;--------------------------------------------------------------
; SpawnEnemies - populate enemies for current level
;--------------------------------------------------------------
SpawnEnemies:
    ; Clear enemy data
    ld hl,EnemyData
    ld bc,MAX_ENEMIES * ENEMY_SIZE
_se_clr:
    ld (hl),0
    inc hl
    dec bc
    ld a,b
    or c
    jr nz,_se_clr
    ; Number = DungeonLevel + 2 (capped at MAX_ENEMIES)
    ld a,(DungeonLevel)
    add a,2
    cp MAX_ENEMIES + 1
    jr c,+
    ld a,MAX_ENEMIES
+:
    ld b,a
    ld ix,EnemyData
_se_loop:
    push bc
    ld c,50                 ; retry limit
_se_find:
    dec c
    jp z,_se_giveUp
    push bc
    ld a,MAP_WIDTH-2
    call GetRandomRange
    inc a
    ld d,a
    ld a,MAP_HEIGHT-2
    call GetRandomRange
    inc a
    ld e,a
    ld b,d
    ld c,e
    push de
    call GetMapTile
    pop de
    pop bc
    cp '.'
    jr nz,_se_find
    ; Not on player?
    ld a,(PlayerX)
    cp d
    jr nz,_se_place
    ld a,(PlayerY)
    cp e
    jr z,_se_find
_se_place:
    ld (ix+ENEMY_X),d
    ld (ix+ENEMY_Y),e
    ; Boss check: floor 5, first enemy slot = spider
    ld a,(DungeonLevel)
    cp 5
    jr nz,_se_normalType
    push ix
    pop hl
    push de
    ld de,EnemyData
    or a
    sbc hl,de
    pop de
    jr nz,_se_normalType
    ; BOSS SPIDER
    ld (ix+ENEMY_TYPE),4
    ld (ix+ENEMY_HP),20
    ld (ix+ENEMY_ATK),8
    jp _se_next
_se_normalType:
    ; Type based on level
    ld a,(DungeonLevel)
    cp 3
    jr nc,_se_allTypes
    cp 2
    jr nc,_se_mid
    ; Level 1: rats
    ld (ix+ENEMY_TYPE),1
    ld (ix+ENEMY_HP),3
    ld (ix+ENEMY_ATK),2
    jr _se_next
_se_mid:
    push ix
    call GetRandomNumber
    pop ix
    and 1
    inc a                   ; 1 or 2
    jr _se_setType
_se_allTypes:
    push ix
    ld a,3
    call GetRandomRange
    pop ix
    inc a                   ; 1, 2, or 3
_se_setType:
    ld (ix+ENEMY_TYPE),a
    cp 4
    jr z,_se_badger
    cp 3
    jr z,_se_orc
    cp 2
    jr z,_se_snake
    ; Rat
    ld (ix+ENEMY_HP),3
    ld (ix+ENEMY_ATK),2
    jr _se_next
_se_snake:
    ld (ix+ENEMY_HP),5
    ld (ix+ENEMY_ATK),3
    jr _se_next
_se_orc:
    ld (ix+ENEMY_HP),8
    ld (ix+ENEMY_ATK),5
    jr _se_next
_se_badger:
    ld (ix+ENEMY_HP),20
    ld (ix+ENEMY_ATK),8
_se_next:
    ld bc,ENEMY_SIZE
    add ix,bc
_se_giveUp:
    pop bc
    dec b
    jp nz,_se_loop
    ret

;--------------------------------------------------------------
; UpdateEnemies - each enemy takes a turn
;--------------------------------------------------------------
UpdateEnemies:
    ld ix,EnemyData
    ld c,MAX_ENEMIES
_ue_loop:
    push bc
    ld a,(ix+ENEMY_TYPE)
    or a
    jr z,_ue_skip
    call EnemyTurn
_ue_skip:
    ld bc,ENEMY_SIZE
    add ix,bc
    pop bc
    dec c
    jr nz,_ue_loop
    ret

;--------------------------------------------------------------
; EnemyTurn - single enemy AI step (chase player)
; Input: IX = enemy data pointer
;--------------------------------------------------------------
EnemyTurn:
    ld d,(ix+ENEMY_X)
    ld e,(ix+ENEMY_Y)
    ; dx toward player
    ld a,(PlayerX)
    sub d
    jr z,_et_noDX
    jr c,_et_dxNeg
    ld b,1
    jr _et_gotDX
_et_dxNeg:
    ld b,-1
    jr _et_gotDX
_et_noDX:
    ld b,0
_et_gotDX:
    ; dy toward player
    ld a,(PlayerY)
    sub e
    jr z,_et_noDY
    jr c,_et_dyNeg
    ld c,1
    jr _et_gotDY
_et_dyNeg:
    ld c,-1
    jr _et_gotDY
_et_noDY:
    ld c,0
_et_gotDY:
    ; B=dx, C=dy. Try X first.
    ld a,b
    or a
    jr z,_et_tryY
    push bc
    push de
    ld a,d
    add a,b
    ld b,a                  ; target X
    ld c,e                  ; same Y
    call TryEnemyMove
    pop de
    pop bc
    ret z                   ; moved or attacked
_et_tryY:
    ld a,c
    or a
    ret z
    push bc
    push de
    ld a,e
    add a,c
    ld c,a                  ; target Y
    ld b,d                  ; same X
    call TryEnemyMove
    pop de
    pop bc
    ret

;--------------------------------------------------------------
; TryEnemyMove - attempt move to target tile
; Input: B=targetX, C=targetY, IX=enemy data
; Output: Z set if action taken
;--------------------------------------------------------------
TryEnemyMove:
    ; Check if target is the player
    ld a,(PlayerX)
    cp b
    jr nz,_tem_notPlayer
    ld a,(PlayerY)
    cp c
    jr nz,_tem_notPlayer
    ; Attack player: damage = max(ATK - DEF, 1)
    ld a,(PlayerDef)
    ld c,a
    ld a,(ix+ENEMY_ATK)
    sub c                   ; ATK - DEF
    jr c,_tem_minDmg
    jr z,_tem_minDmg
    jr _tem_doDmg
_tem_minDmg:
    ld a,1                  ; minimum 1 damage
_tem_doDmg:
    ld b,a
    ; Play hurt sound based on damage taken
    push bc
    ld a,b
    call PlayHurtSfx
    pop bc
    ; Apply damage
    ld a,(PlayerHP)
    sub b
    jr nc,+
    xor a
+:
    ld (PlayerHP),a
    xor a                   ; Z = action taken
    ret
_tem_notPlayer:
    ; Bounds
    ld a,b
    cp MAP_WIDTH
    jr nc,_tem_fail
    ld a,c
    cp MAP_HEIGHT
    jr nc,_tem_fail
    ; Terrain
    push bc
    call GetMapTile
    pop bc
    cp '.'
    jr z,_tem_canMove
    cp '>'
    jr z,_tem_canMove
    call IsItemTile
    jr z,_tem_canMove
    jr _tem_fail
_tem_canMove:
    ; Check for another enemy
    push bc
    call FindEnemyAt
    pop bc
    cp $FF
    jr nz,_tem_fail
    ; Move
    ld (ix+ENEMY_X),b
    ld (ix+ENEMY_Y),c
    xor a                   ; Z set
    ret
_tem_fail:
    or 1                    ; clear Z
    ret

;--------------------------------------------------------------
; FindEnemyAt - find alive enemy at position
; Input: B=X, C=Y
; Output: A = index (0-7) or $FF
; Preserves: IX
;--------------------------------------------------------------
FindEnemyAt:
    push ix
    ld ix,EnemyData
    ld e,0
_fea_loop:
    ld a,e
    cp MAX_ENEMIES
    jr z,_fea_none
    ld a,(ix+ENEMY_TYPE)
    or a
    jr z,_fea_next
    ld a,(ix+ENEMY_X)
    cp b
    jr nz,_fea_next
    ld a,(ix+ENEMY_Y)
    cp c
    jr nz,_fea_next
    ; Found
    ld a,e
    pop ix
    ret
_fea_next:
    push de
    ld de,ENEMY_SIZE
    add ix,de
    pop de
    inc e
    jr _fea_loop
_fea_none:
    ld a,$FF
    pop ix
    ret
