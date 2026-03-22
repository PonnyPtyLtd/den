;==============================================================
; Player movement and combat
;==============================================================

ProcessPlayerInput:
    ld a,(InputNew)
    bit 0,a
    jr nz,_pi_up
    bit 1,a
    jr nz,_pi_down
    bit 2,a
    jr nz,_pi_left
    bit 3,a
    jr nz,_pi_right
    ret
_pi_up:
    ld de,$00FF
    jr _pi_move
_pi_down:
    ld de,$0001
    jr _pi_move
_pi_left:
    ld de,$FF00
    jr _pi_move
_pi_right:
    ld de,$0100

_pi_move:
    ld a,(PlayerX)
    add a,d
    ld b,a
    ld a,(PlayerY)
    add a,e
    ld c,a
    ; Bounds
    ld a,b
    cp MAP_WIDTH
    ret nc
    ld a,c
    cp MAP_HEIGHT
    ret nc
    ; Terrain
    push bc
    call GetMapTile
    pop bc
    cp '#'
    ret z
    ; Enemy check
    push bc
    call FindEnemyAt
    pop bc
    cp $FF
    jr z,_pi_doMove
    jp AttackEnemy

_pi_doMove:
    ; No auto-pickup! Player must use radial menu to pick up.
    ld a,b
    ld (PlayerX),a
    ld a,c
    ld (PlayerY),a
    ret

;--------------------------------------------------------------
; AttackEnemy
;--------------------------------------------------------------
AttackEnemy:
    ; A = enemy index. Save for deferred damage.
    ld (AttackTargetIdx),a
    ; Find enemy position for lunge direction
    ld hl,EnemyData
    ld de,ENEMY_SIZE
    or a
    jr z,_ae_gotPtr
    ld b,a
_ae_mul:
    add hl,de
    djnz _ae_mul
_ae_gotPtr:
    ld d,(hl)               ; D = enemy X
    inc hl
    ld e,(hl)               ; E = enemy Y
    ; Compute lunge direction (toward enemy)
    ld a,d
    ld b,a
    ld a,(PlayerX)
    sub b                   ; playerX - enemyX
    neg                     ; enemyX - playerX = direction
    add a,a
    add a,a                 ; * 4 px per frame
    ld (AttackDirX),a
    ld a,e
    ld b,a
    ld a,(PlayerY)
    sub b
    neg
    add a,a
    add a,a
    ld (AttackDirY),a
    ; Save damage for deferred application
    ld a,(PlayerAtk)
    ld (AttackDamage),a
    ; Play attack sound
    call PlayAttackSfx
    ; Start lunge forward (2 frames)
    ld a,TURN_PLAYER_ATK_FWD
    ld (TurnPhase),a
    ld a,2
    ld (AnimCounter),a
    ret

; Apply deferred attack damage (called after lunge completes)
ApplyAttackDamage:
    ld a,(AttackTargetIdx)
    ld hl,EnemyData
    ld de,ENEMY_SIZE
    or a
    jr z,_aad_gotPtr
    ld b,a
_aad_mul:
    add hl,de
    djnz _aad_mul
_aad_gotPtr:
    ld d,(hl)               ; D = enemy X
    inc hl
    ld e,(hl)               ; E = enemy Y
    inc hl                  ; HL -> HP
    ld a,(AttackDamage)
    ld b,a
    ld a,(hl)
    sub b
    jr c,_aad_kill
    jr z,_aad_kill
    ld (hl),a
    ret
_aad_kill:
    xor a
    ld (hl),a               ; HP = 0
    inc hl
    ld c,(hl)               ; C = type
    ld (hl),a               ; Type = 0
    push de
    ld a,c
    call PlayDeathSfx
    pop de
    call TryDropItem
    ret

;--------------------------------------------------------------
; TryDropItem - floor-scaled drop rate and quality
; D=X, E=Y of dead enemy
;--------------------------------------------------------------
TryDropItem:
    ; Check for guaranteed drop (at least 1 per level)
    ld a,(LevelDropCount)
    or a
    jr z,_td_forceCheck
    ; Normal drop chance: floor*5 + 5 percent
    push de
    ld a,100
    call GetRandomRange     ; A = 0-99
    ld b,a
    ld a,(DungeonLevel)
    ld c,a
    add a,a
    add a,a
    add a,c
    add a,5
    cp b
    pop de
    ret c
    ret z
    jr _td_doDropRoll

_td_forceCheck:
    ; No drops yet - check if any enemies remain
    push de
    ld hl,EnemyData
    ld b,MAX_ENEMIES
    ld c,0                  ; alive count
_td_countLoop:
    push hl
    inc hl
    inc hl
    inc hl
    ld a,(hl)               ; type
    pop hl
    or a
    jr z,+
    inc c
+:
    push de
    ld de,ENEMY_SIZE
    add hl,de
    pop de
    djnz _td_countLoop
    pop de
    ld a,c
    or a
    jr z,_td_doDropRoll     ; last enemy - force drop!
    ; Not last enemy, use normal chance
    push de
    ld a,100
    call GetRandomRange
    ld b,a
    ld a,(DungeonLevel)
    ld c,a
    add a,a
    add a,a
    add a,c
    add a,5
    cp b
    pop de
    ret c
    ret z

_td_doDropRoll:

    ; Roll for special items first: potion (20%) or wand (2%)
    push de
    ld a,100
    call GetRandomRange     ; A = 0-99
    cp 2
    jr c,_td_wand           ; 0-1 = wand (2%)
    cp 22
    jr c,_td_potion         ; 2-21 = potion (20%)

    ; Normal category: weapon(0), shield(1), armor(2), helmet(3)
    ld a,4
    call GetRandomRange     ; A = 0,1,2,3
    cp 3
    jr z,_td_helmet         ; helmet = always type 10

    ld c,a                  ; C = category (0-2)

    ; Pick tier based on floor
    ld a,(DungeonLevel)
    dec a                   ; 0-indexed (0-4)
    ld hl,DropMinTier
    ld b,0
    push bc
    ld c,a
    add hl,bc
    ld b,(hl)               ; B = min tier
    ld hl,DropMaxTier
    add hl,bc
    ld a,(hl)               ; A = max tier
    sub b
    inc a                   ; range = max-min+1
    push bc
    call GetRandomRange     ; A = 0..range-1
    pop bc
    add a,b                 ; A = min + roll = tier (0-2)

    ; Calculate item type: category*3 + tier + 1
    pop bc                  ; C = category
    ld b,a                  ; B = tier
    ld a,c
    add a,a
    add a,c                 ; A = category*3
    add a,b                 ; + tier
    inc a                   ; + 1 = item type (1-9)
    pop de                  ; D=X, E=Y
    jr _td_placeItem

_td_wand:
    pop de
    ld a,ITEM_WAND
    jr _td_placeItem

_td_potion:
    pop de
    ld a,ITEM_POTION
    jr _td_placeItem

_td_helmet:
    pop de
    ld a,ITEM_HELMET

_td_placeItem:

    ; Place item on map (try target tile, then adjacent)
    push af                 ; save item type
    call ItemTypeToChar     ; A = ASCII char
    ld b,d
    ld c,e
    push af                 ; save char
    call GetMapTile         ; check if tile is floor
    cp '.'
    jr z,_td_placeHere
    ; Try adjacent tiles: right, left, down, up
    pop af
    push af
    inc d                   ; try right
    ld b,d
    call GetMapTile
    cp '.'
    jr z,_td_placeHere
    dec d
    dec d                   ; try left
    ld b,d
    call GetMapTile
    cp '.'
    jr z,_td_placeHere
    inc d
    inc e                   ; try down
    ld b,d
    ld c,e
    call GetMapTile
    cp '.'
    jr z,_td_placeHere
    dec e
    dec e                   ; try up
    ld b,d
    ld c,e
    call GetMapTile
    cp '.'
    jr z,_td_placeHere
    ; All blocked - drop lost
    pop af
    pop af
    ret
_td_placeHere:
    pop af                  ; A = item char
    ld b,d
    ld c,e
    call SetMapTile
    ; Track drop count for guaranteed drop
    ld a,(LevelDropCount)
    inc a
    ld (LevelDropCount),a
    pop af                  ; clean stack (item type)
    ret

; Floor-scaled tier tables (indexed by DungeonLevel-1)
DropMinTier: .db 0, 0, 0, 1, 2
DropMaxTier: .db 0, 1, 2, 2, 2

;--------------------------------------------------------------
; CharToItemType - ASCII char to item type (1-9)
;--------------------------------------------------------------
CharToItemType:
    cp 'w'
    jr z,_cit_1
    cp 'x'
    jr z,_cit_2
    cp 'y'
    jr z,_cit_3
    cp 's'
    jr z,_cit_4
    cp 'S'
    jr z,_cit_5
    cp 'T'
    jr z,_cit_6
    cp 'a'
    jr z,_cit_7
    cp 'A'
    jr z,_cit_8
    cp 'B'
    jr z,_cit_9
    cp 'h'
    jr z,_cit_10
    cp 'p'
    jr z,_cit_11
    ld a,ITEM_WAND          ; 'f'
    ret
_cit_9: ld a,9
    ret
_cit_10: ld a,10
    ret
_cit_11: ld a,11
    ret
_cit_1: ld a,1
    ret
_cit_2: ld a,2
    ret
_cit_3: ld a,3
    ret
_cit_4: ld a,4
    ret
_cit_5: ld a,5
    ret
_cit_6: ld a,6
    ret
_cit_7: ld a,7
    ret
_cit_8: ld a,8
    ret

;--------------------------------------------------------------
; ItemTypeToChar - item type (1-9) to ASCII char
;--------------------------------------------------------------
ItemTypeToChar:
    dec a
    ld hl,_itcTable
    ld c,a
    ld b,0
    add hl,bc
    ld a,(hl)
    ret
_itcTable: .db 'w','x','y','s','S','T','a','A','B','h','p','f'

;--------------------------------------------------------------
; Stat tables
;--------------------------------------------------------------
WeaponBonusTable: .db 1, 3, 5    ; weapon types 1-3: ATK bonus
ShieldDefTable:   .db 1, 2, 4    ; shield types 4-6: DEF bonus
ArmorHPTable:     .db 2, 5, 10   ; armor types 7-9: MaxHP bonus
