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
    jr nz,_se_notPlayer
    ld a,(PlayerY)
    cp e
    jr z,_se_find
_se_notPlayer:
    ; Not on another enemy?
    push bc
    push de                 ; save position (FindEnemyAt clobbers E)
    ld b,d
    ld c,e
    call FindEnemyAt
    pop de                  ; restore position
    pop bc
    cp $FF
    jr nz,_se_find          ; spot taken by another enemy
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
; UpdateEnemies - process enemies closest to player first
; This prevents congestion: closest moves first, clearing the path
;--------------------------------------------------------------
UpdateEnemies:
    ; Process all 8 enemies, each turn picking the unprocessed one
    ; closest to the player (lowest distance in DistMap)
    ; Use a bitmask to track which have been processed
    ld a,$FF
    ld (BorderX),a          ; reuse as "remaining" bitmask (all 8 bits set)
    ld b,MAX_ENEMIES
_ue_outerLoop:
    push bc
    ; Find unprocessed enemy with lowest distance
    ld c,$FF                ; best distance so far
    ld b,$FF                ; best enemy index
    ld e,0                  ; current index
    ld ix,EnemyData
_ue_findBest:
    ld a,e
    cp MAX_ENEMIES
    jr z,_ue_foundBest
    ; Check if already processed (bit clear in mask)
    ld a,(BorderX)
    ld d,a
    ld a,e
    call _ue_checkBit       ; Z = bit is clear (already done)
    jr z,_ue_findNext
    ; Check if alive
    ld a,(ix+ENEMY_TYPE)
    or a
    jr z,_ue_findNext
    ; Get distance
    ld a,(ix+ENEMY_Y)
    add a,a
    add a,a
    add a,a
    add a,a
    add a,(ix+ENEMY_X)
    call _et_getDist
    cp c
    jr nc,_ue_findNext
    ld c,a                  ; new best distance
    ld b,e                  ; new best index
_ue_findNext:
    push de
    ld de,ENEMY_SIZE
    add ix,de
    pop de
    inc e
    jr _ue_findBest

_ue_foundBest:
    ld a,b
    cp $FF
    jr z,_ue_allDone        ; no more unprocessed enemies

    ; Mark this enemy as processed (clear bit in mask)
    ld a,(BorderX)
    ld d,a
    ld a,b
    call _ue_clearBit
    ld (BorderX),a

    ; Get IX pointing to this enemy
    ld ix,EnemyData
    ld a,b
    or a
    jr z,_ue_gotEnemy
    ld c,a
_ue_seekLoop:
    push de
    ld de,ENEMY_SIZE
    add ix,de
    pop de
    dec c
    jr nz,_ue_seekLoop
_ue_gotEnemy:
    call EnemyTurn
    pop bc
    djnz _ue_outerLoop
    ret
_ue_allDone:
    pop bc
    ret

; Check if bit E is set in D. Z flag = bit clear.
_ue_checkBit:
    push bc
    ld a,e
    ld b,a
    ld a,1
    or a                    ; clear Z
    inc b
_ucb_shift:
    dec b
    jr z,_ucb_done
    add a,a
    jr _ucb_shift
_ucb_done:
    and d                   ; Z if bit was clear
    pop bc
    ret

; Clear bit A in register D, return result in A
_ue_clearBit:
    push bc
    ld b,a
    ld a,1
    inc b
_uclb_shift:
    dec b
    jr z,_uclb_done
    add a,a
    jr _uclb_shift
_uclb_done:
    cpl                     ; invert = clear mask
    and d                   ; clear the bit
    pop bc
    ret

;--------------------------------------------------------------
; BuildDistMap - BFS flood fill from player position
; Fills DistMap with distances. $FF = unreachable/wall.
; Uses PathQueue as BFS queue (tile indices 0-175)
;--------------------------------------------------------------
BuildDistMap:
    ; Fill DistMap with $FF
    ld hl,DistMap
    ld a,$FF
    ld b,176
_bdm_fill:
    ld (hl),a
    inc hl
    djnz _bdm_fill

    ; Seed: player position = distance 0
    ld a,(PlayerY)
    add a,a
    add a,a
    add a,a
    add a,a
    ld b,a
    ld a,(PlayerX)
    add a,b                 ; A = player tile index
    ; Set DistMap[player] = 0
    ld hl,DistMap
    ld c,a
    ld b,0
    add hl,bc
    ld (hl),0
    ; Enqueue player
    ld (PathQueue),a
    xor a
    ld (BfsHead),a
    ld a,1
    ld (BfsTail),a

_bdm_bfs:
    ld a,(BfsHead)
    ld b,a
    ld a,(BfsTail)
    cp b
    ret z                   ; queue empty

    ; Dequeue: A = PathQueue[head++]
    ld hl,PathQueue
    ld c,b
    ld b,0
    add hl,bc
    ld c,(hl)               ; C = current tile index
    ld a,(BfsHead)
    inc a
    ld (BfsHead),a

    ; Get current distance
    ld hl,DistMap
    ld b,0
    push bc                 ; save C = current index
    add hl,bc
    ld b,(hl)               ; B = distance at current tile
    pop hl                  ; H=0, L=current index (was BC with B=0)
    ld c,l                  ; C = current index, B = current distance

    ; Try 4 neighbors. For each: check bounds, check unvisited, check walkable.
    ; Right
    ld a,c
    and $0F
    cp 15
    jr z,_bdm_noR
    ld a,c
    inc a
    call _bdm_tryVisit
_bdm_noR:
    ; Left
    ld a,c
    and $0F
    jr z,_bdm_noL
    ld a,c
    dec a
    call _bdm_tryVisit
_bdm_noL:
    ; Down
    ld a,c
    cp 160
    jr nc,_bdm_noD
    ld a,c
    add a,16
    call _bdm_tryVisit
_bdm_noD:
    ; Up
    ld a,c
    cp 16
    jr c,_bdm_noU
    ld a,c
    sub 16
    call _bdm_tryVisit
_bdm_noU:
    jr _bdm_bfs

; Try to visit tile at index A
; B = parent distance (preserved), C = parent index (preserved)
_bdm_tryVisit:
    push bc                 ; save parent dist + index
    ld e,a                  ; E = neighbor index

    ; Check if already visited
    ld hl,DistMap
    ld c,a
    ld b,0
    add hl,bc
    ld a,(hl)
    cp $FF
    jr nz,_bdm_tvDone      ; already visited

    ; Check map: walkable?
    push hl                 ; save DistMap pointer
    ld a,e
    and $0F
    ld b,a                  ; B = x
    ld a,e
    rrca
    rrca
    rrca
    rrca
    and $0F
    ld c,a                  ; C = y
    call GetMapTile
    pop hl                  ; restore DistMap pointer
    cp '#'
    jr z,_bdm_tvDone

    ; Set distance and enqueue
    pop bc                  ; B = parent distance
    push bc
    ld a,b
    inc a                   ; distance = parent + 1
    ld (hl),a               ; DistMap[neighbor] = dist + 1

    ; Enqueue neighbor
    ld a,(BfsTail)
    ld c,a
    ld b,0
    ld hl,PathQueue
    add hl,bc
    ld (hl),e               ; PathQueue[tail] = neighbor index
    ld a,c
    inc a
    ld (BfsTail),a        ; tail++

_bdm_tvDone:
    pop bc                  ; restore parent dist + index
    ret

;--------------------------------------------------------------
; EnemyTurn - move toward player using distance map
; Tries all 4 directions sorted by distance, falls back if blocked
; Input: IX = enemy data pointer
;--------------------------------------------------------------
; Helper: get distance of tile at index A from DistMap
; Returns A = distance (or $FF)
_et_getDist:
    ld hl,DistMap
    push bc
    ld c,a
    ld b,0
    add hl,bc
    pop bc
    ld a,(hl)
    ret

EnemyTurn:
    ld d,(ix+ENEMY_X)
    ld e,(ix+ENEMY_Y)

    ; Get my distance
    ld a,e
    add a,a
    add a,a
    add a,a
    add a,a
    add a,d
    call _et_getDist
    cp $FF
    ret z                   ; unreachable
    or a
    ret z                   ; distance 0 = at player

    push de                 ; save enemy pos
    ld c,a                  ; C = my distance

    ; Try all 4 directions. Only attempt move if neighbor distance < mine.
    ; If TryEnemyMove blocked (another enemy), try next direction.

    ; Left
    ld a,d
    or a
    jr z,_et_skipL
    ld a,e
    add a,a
    add a,a
    add a,a
    add a,a
    add a,d
    dec a
    call _et_getDist
    cp c
    jr nc,_et_skipL
    pop de
    push de
    ld b,d
    ld c,e
    dec b
    call TryEnemyMove
    jp z,_et_done
    ; Reload my distance
    pop de
    push de
    ld a,e
    add a,a
    add a,a
    add a,a
    add a,a
    add a,d
    call _et_getDist
    ld c,a
_et_skipL:

    ; Right
    pop de
    push de
    ld a,d
    cp MAP_WIDTH-1
    jr z,_et_skipR
    ld a,e
    add a,a
    add a,a
    add a,a
    add a,a
    add a,d
    inc a
    call _et_getDist
    cp c
    jr nc,_et_skipR
    pop de
    push de
    ld b,d
    ld c,e
    inc b
    call TryEnemyMove
    jp z,_et_done
    pop de
    push de
    ld a,e
    add a,a
    add a,a
    add a,a
    add a,a
    add a,d
    call _et_getDist
    ld c,a
_et_skipR:

    ; Down
    pop de
    push de
    ld a,e
    cp MAP_HEIGHT-1
    jr z,_et_skipD
    ld a,e
    add a,a
    add a,a
    add a,a
    add a,a
    add a,d
    add a,16
    call _et_getDist
    cp c
    jr nc,_et_skipD
    pop de
    push de
    ld b,d
    ld c,e
    inc c
    call TryEnemyMove
    jp z,_et_done
    pop de
    push de
    ld a,e
    add a,a
    add a,a
    add a,a
    add a,a
    add a,d
    call _et_getDist
    ld c,a
_et_skipD:

    ; Up
    pop de
    push de
    ld a,e
    or a
    jr z,_et_skipU
    ld a,e
    add a,a
    add a,a
    add a,a
    add a,a
    add a,d
    sub 16
    call _et_getDist
    cp c
    jr nc,_et_skipU
    pop de
    push de
    ld b,d
    ld c,e
    dec c
    call TryEnemyMove
    jp z,_et_done
_et_skipU:

    ; All optimal directions blocked - try random walk as fallback
    ; Pick a random direction and try it (prevents permanent freezing)
    pop de
    push de
    ld a,4
    call GetRandomRange     ; 0-3
    or a
    jr z,_et_rndR
    cp 1
    jr z,_et_rndL
    cp 2
    jr z,_et_rndD
    ; Up
    ld b,d
    ld c,e
    dec c
    call TryEnemyMove
    pop de
    ret
_et_rndR:
    ld b,d
    ld c,e
    inc b
    call TryEnemyMove
    pop de
    ret
_et_rndL:
    ld b,d
    ld c,e
    dec b
    call TryEnemyMove
    pop de
    ret
_et_rndD:
    ld b,d
    ld c,e
    inc c
    call TryEnemyMove
    pop de
    ret

_et_done:
    pop de
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
