;==============================================================
; Player movement and combat
;==============================================================

ProcessPlayerInput:
    ld a,(InputNew)
    ; Check directions in priority order
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
    ld de,$00FF             ; dx=0, dy=-1
    jr _pi_move
_pi_down:
    ld de,$0001             ; dx=0, dy=1
    jr _pi_move
_pi_left:
    ld de,$FF00             ; dx=-1, dy=0
    jr _pi_move
_pi_right:
    ld de,$0100             ; dx=1, dy=0

_pi_move:
    ; Calculate target position
    ld a,(PlayerX)
    add a,d
    ld b,a                  ; target X
    ld a,(PlayerY)
    add a,e
    ld c,a                  ; target Y
    ; Bounds check (unsigned: wraps to >= 32 or >= 22)
    ld a,b
    cp MAP_WIDTH
    ret nc
    ld a,c
    cp MAP_HEIGHT
    ret nc
    ; Check terrain
    push bc
    call GetMapTile
    pop bc
    cp '#'
    ret z                   ; wall
    ; Check for enemy at target
    push bc
    call FindEnemyAt
    pop bc
    cp $FF
    jr z,_pi_doMove
    ; Attack enemy (A = index)
    jp AttackEnemy

_pi_doMove:
    ld a,b
    ld (PlayerX),a
    ld a,c
    ld (PlayerY),a
    ret

;--------------------------------------------------------------
; AttackEnemy - player attacks enemy by index
; Input: A = enemy index
;--------------------------------------------------------------
AttackEnemy:
    ld hl,EnemyData
    ld de,ENEMY_SIZE
    or a
    jr z,_ae_gotPtr
    ld b,a
_ae_mul:
    add hl,de
    djnz _ae_mul
_ae_gotPtr:
    inc hl
    inc hl                  ; HL -> enemy HP
    ld a,(PlayerAtk)
    ld b,a
    ld a,(hl)
    sub b
    jr c,_ae_kill
    jr z,_ae_kill
    ld (hl),a
    ret
_ae_kill:
    xor a
    ld (hl),a               ; HP = 0
    inc hl
    ld (hl),a               ; Type = 0 (dead)
    ret
