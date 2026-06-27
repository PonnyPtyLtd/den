;==============================================================
; Game state machine
;==============================================================

GameInit:
    xor a
    ld (GameState),a
    ld (InputPrevious),a
    ld (AnimFrame),a
    ld (TurnPhase),a
    ld (SfxTimer),a
    ld a,15
    ld (AnimTimer),a
    ret

GameTick:
    call ReadJoypad
    ld (InputNew),a
    ld a,(GameState)
    or a
    jp z,TitleTick
    cp 1
    jp z,PlayTick
    cp 2
    jp z,DeadTick
    cp 3
    jp z,WinTick
    cp 4
    jp z,RaceSelectTick
    ret

;--------------------------------------------------------------
TitleTick:
    ld hl,(RandomNumberGeneratorWord)
    inc hl
    ld (RandomNumberGeneratorWord),hl
    ld a,(InputNew)
    bit 4,a
    ret z
    ; Go to race selection
    call ShowRaceSelect
    ld a,4
    ld (GameState),a
    ret

;--------------------------------------------------------------
StartNewGame:
    call InitDungeonMusic
    ld a,1
    ld (DungeonLevel),a
    ld a,20
    ld (PlayerHP),a
    ld (PlayerMaxHP),a
    ld a,4
    ld (PlayerAtk),a
    xor a
    ld (PlayerWeapon),a
    ld (PlayerArmor),a
    ld (PlayerDef),a
    ld (AnimFrame),a
    ld (TurnPhase),a
    ld (MenuState),a
    ld (InvLeftHand),a
    ld (InvRightHand),a
    ld (InvBody),a
    ld (InvHead),a
    ld (InvPocket1),a
    ld (InvPocket2),a
    ld (InvSubMenu),a
    ld (WandAiming),a
    ld a,15
    ld (AnimTimer),a
    call GiveRaceDefaults   ; per-race starting equipment + RecalcStats
    call GenerateLevel
    ret

;--------------------------------------------------------------
; GiveRaceDefaults - copy the selected race's starting loadout into the
; 6 contiguous inventory slots (LeftHand,RightHand,Body,Head,Pocket1,Pocket2)
; then recompute ATK/DEF from the equipped gear.
;--------------------------------------------------------------
GiveRaceDefaults:
    ld a,(PlayerRace)
    ld l,a
    add a,a                 ; *2
    add a,l                 ; *3
    add a,a                 ; *6 (6 bytes per race)
    ld c,a
    ld b,0
    ld hl,RaceDefaultEquip
    add hl,bc               ; HL = source row
    ld de,InvLeftHand       ; DE = dest (6 contiguous slots)
    ld bc,6
    ldir
    call RecalcStats
    ret

; Starting loadout per race, one 6-byte row each:
;   LeftHand, RightHand, Body, Head, Pocket1, Pocket2
RaceDefaultEquip:
    .db ITEM_WEAPON_1, 0,            0,           0,          ITEM_POTION, 0            ; 0 Poison Arrow Frog
    .db ITEM_WEAPON_1, 0,            0,           0,          0,           0            ; 1 Dingo
    .db ITEM_WEAPON_1, ITEM_SHIELD_1, ITEM_ARMOR_1, 0,         0,           0            ; 2 Manul
    .db ITEM_WEAPON_1, 0,            0,           0,          ITEM_POTION, 0            ; 3 Polish Chicken

;--------------------------------------------------------------
GenerateLevel:
    call GenerateDungeon
    call SpawnEnemies
    ; Initialize enemy old positions so first animation doesn't start from (0,0)
    call SaveEnemyOldPositions
    ; Reset drop tracking
    xor a
    ld (LevelDropCount),a
    ; Set player pixel position
    ld a,(PlayerX)
    add a,a
    add a,a
    add a,a
    add a,a
    ld (PlayerPixelX),a
    ld a,(PlayerY)
    add a,a
    add a,a
    add a,a
    add a,a
    ld (PlayerPixelY),a
    ; Render
    call DisplayOff
    call ClearNameTable
    call RenderFullMap
    call RenderHUD
    call BuildSpriteTable
    call DisplayOn
    ld a,1
    ld (GameState),a
    xor a
    ld (TurnPhase),a
    ret

;--------------------------------------------------------------
; PlayTick - turn-based with animation phases
;--------------------------------------------------------------
PlayTick:
    ; Check menu state first
    ld a,(MenuState)
    or a
    jr z,_pt_noMenu
    cp 1
    jp z,RadialMenuTick
    cp 2
    jp z,InventoryTick
    ret
_pt_noMenu:
    ld a,(TurnPhase)
    or a
    jp z,_pt_idle
    cp TURN_PLAYER_ANIM
    jp z,_pt_playerAnim
    cp TURN_PLAYER_ATK_FWD
    jp z,_pt_atkFwd
    cp TURN_PLAYER_ATK_BACK
    jp z,_pt_atkBack
    cp TURN_ENEMY_ANIM
    jp z,_pt_enemyAnim
    ret

_pt_idle:
    ; Check wand aiming mode
    ld a,(WandAiming)
    or a
    jr z,_pt_noWand
    ; Waiting for direction to fire wand
    ld a,(InputNew)
    and $0F
    ret z                   ; no direction yet
    ; Close the aim menu, restore map
    push af
    call _closeAimMenu
    pop af
    ; Fire wand with visible projectile
    call FireWand           ; A = direction bits
    ; Rebuild sprites (enemy may have died)
    call BuildSpriteTable
    xor a
    ld (WandAiming),a
    ; Costs a turn
    call SaveEnemyOldPositions
    call BuildDistMap
    call UpdateEnemies
    call StartEnemyAnimation
    ret
_pt_noWand:
    ; Check Start (Button 1) for radial menu
    ld a,(InputNew)
    bit 4,a
    jr z,_pt_noStart
    ld a,1
    ld (MenuState),a
    call DrawRadialMenu
    ret
_pt_noStart:
    ; Check Button 2 for wait (skip turn)
    ld a,(InputNew)
    bit 5,a
    jr z,_pt_noB2
    call SaveEnemyOldPositions
    call BuildDistMap
    call UpdateEnemies
    call StartEnemyAnimation
    ret
_pt_noB2:
    ; Only accept direction input when idle
    ld a,(InputNew)
    and $0F
    ret z                   ; no direction = no turn

    ; Save old player position for animation
    ld a,(PlayerX)
    ld b,a
    ld a,(PlayerY)
    ld c,a
    push bc                 ; save old pos

    ; Process player input (may move or attack)
    call ProcessPlayerInput

    ; Check if an attack animation was started
    ld a,(TurnPhase)
    or a
    jr z,_pt_checkMove
    ; Attack animation started - clean stack and let it run
    pop bc
    ret

_pt_checkMove:
    ; Did player actually move? Compare old vs new position
    pop bc                  ; B=old X, C=old Y
    ld a,(PlayerX)
    cp b
    jr nz,_pt_moved
    ld a,(PlayerY)
    cp c
    jr nz,_pt_moved

    ; Player didn't move (bumped wall) - skip to enemy phase
    call SaveEnemyOldPositions
    call BuildDistMap
    call UpdateEnemies
    call StartEnemyAnimation
    ret

_pt_moved:
    ; Player moved - start sliding animation
    ; Old pixel position = old_logical * 16
    ld a,b                  ; old X
    add a,a
    add a,a
    add a,a
    add a,a
    ld (PlayerPixelX),a
    ld a,c                  ; old Y
    add a,a
    add a,a
    add a,a
    add a,a
    ld (PlayerPixelY),a
    ; Calculate delta per frame
    ld a,(PlayerX)
    sub b                   ; new - old (-1, 0, or 1)
    add a,a
    add a,a                 ; * 4 = pixel step per frame
    ld (PlayerDeltaX),a
    ld a,(PlayerY)
    sub c
    add a,a
    add a,a
    ld (PlayerDeltaY),a
    ; Start animation
    ld a,TURN_PLAYER_ANIM
    ld (TurnPhase),a
    ld a,ANIM_FRAMES
    ld (AnimCounter),a
    ; Update sprite immediately
    call UpdatePlayerSprite
    ret

; --- Attack animation: lunge forward ---
_pt_atkFwd:
    ld a,(PlayerPixelX)
    ld b,a
    ld a,(AttackDirX)
    add a,b
    ld (PlayerPixelX),a
    ld a,(PlayerPixelY)
    ld b,a
    ld a,(AttackDirY)
    add a,b
    ld (PlayerPixelY),a
    call UpdatePlayerSprite
    ld a,(AnimCounter)
    dec a
    ld (AnimCounter),a
    ret nz
    ; Lunge complete - apply damage, start return
    call ApplyAttackDamage
    ; Negate direction for return
    ld a,(AttackDirX)
    neg
    ld (AttackDirX),a
    ld a,(AttackDirY)
    neg
    ld (AttackDirY),a
    ld a,TURN_PLAYER_ATK_BACK
    ld (TurnPhase),a
    ld a,2
    ld (AnimCounter),a
    ret

; --- Attack animation: return to position ---
_pt_atkBack:
    ld a,(PlayerPixelX)
    ld b,a
    ld a,(AttackDirX)
    add a,b
    ld (PlayerPixelX),a
    ld a,(PlayerPixelY)
    ld b,a
    ld a,(AttackDirY)
    add a,b
    ld (PlayerPixelY),a
    call UpdatePlayerSprite
    ld a,(AnimCounter)
    dec a
    ld (AnimCounter),a
    ret nz
    ; Snap back to grid position
    ld a,(PlayerX)
    add a,a
    add a,a
    add a,a
    add a,a
    ld (PlayerPixelX),a
    ld a,(PlayerY)
    add a,a
    add a,a
    add a,a
    add a,a
    ld (PlayerPixelY),a
    call UpdatePlayerSprite
    ; Start enemy phase
    call SaveEnemyOldPositions
    call BuildDistMap
    call UpdateEnemies
    call StartEnemyAnimation
    ret

_pt_playerAnim:
    ; Advance player sprite by delta
    ld a,(PlayerPixelX)
    ld b,a
    ld a,(PlayerDeltaX)
    add a,b
    ld (PlayerPixelX),a
    ld a,(PlayerPixelY)
    ld b,a
    ld a,(PlayerDeltaY)
    add a,b
    ld (PlayerPixelY),a
    call UpdatePlayerSprite

    ; Check if animation complete
    ld a,(AnimCounter)
    dec a
    ld (AnimCounter),a
    ret nz

    ; Snap to final position
    ld a,(PlayerX)
    add a,a
    add a,a
    add a,a
    add a,a
    ld (PlayerPixelX),a
    ld a,(PlayerY)
    add a,a
    add a,a
    add a,a
    add a,a
    ld (PlayerPixelY),a

    ; Check stairs
    ld a,(PlayerX)
    ld b,a
    ld a,(PlayerY)
    ld c,a
    call GetMapTile
    cp '>'
    jr nz,_pt_noStairs
    ld a,(DungeonLevel)
    cp 5
    jr z,_pt_win
    inc a
    ld (DungeonLevel),a
    call GenerateLevel
    ret
_pt_win:
    call ShowWinScreen
    ld a,3
    ld (GameState),a
    ret

_pt_noStairs:
    ; No auto-pickup - player uses radial menu to pick up items
    ; Start enemy phase
    call SaveEnemyOldPositions
    call BuildDistMap
    call UpdateEnemies
    call StartEnemyAnimation
    ret

_pt_enemyAnim:
    ; Advance all enemy sprites
    call AnimateEnemyStep

    ld a,(AnimCounter)
    dec a
    ld (AnimCounter),a
    ret nz

    ; Animation complete - check death, rebuild sprites
    ld a,(PlayerHP)
    or a
    jr nz,_pt_alive
    call ShowDeadScreen
    ld a,2
    ld (GameState),a
    ret
_pt_alive:
    call BuildSpriteTable
    call RenderHUD
    xor a
    ld (TurnPhase),a
    ret

; SaveEnemyOldPositions - capture before UpdateEnemies moves them
SaveEnemyOldPositions:
    ld hl,EnemyData
    ld ix,EnemyOldX
    ld iy,EnemyOldY
    ld b,MAX_ENEMIES
_seop_loop2:
    ld a,(hl)              ; enemy X (offset 0)
    ld (ix+0),a
    inc hl                 ; offset 1 = Y
    ld a,(hl)              ; enemy Y
    ld (iy+0),a
    ; Advance to next enemy: skip remaining 3 bytes (HP, TYPE, ATK)
    ; + 1 more to reach next enemy's X = 4 total inc's after Y
    inc hl                 ; offset 2 = HP
    inc hl                 ; offset 3 = TYPE
    inc hl                 ; offset 4 = ATK
    inc hl                 ; advance to next enemy X
    inc ix
    inc iy
    djnz _seop_loop2
    ret

;--------------------------------------------------------------
; StartEnemyAnimation - compute enemy pixel deltas
;--------------------------------------------------------------
StartEnemyAnimation:
    ld a,TURN_ENEMY_ANIM
    ld (TurnPhase),a
    ld a,ANIM_FRAMES
    ld (AnimCounter),a
    ; Set up pixel positions and deltas using IX for EnemyData
    ld ix,EnemyData
    ld c,0                  ; enemy index
_sea_loop:
    push bc
    ; Set EnemyPixelX = EnemyOldX * 16
    ld b,0
    ld hl,EnemyOldX
    add hl,bc
    ld a,(hl)
    add a,a
    add a,a
    add a,a
    add a,a                 ; * 16
    ld hl,EnemyPixelX
    add hl,bc
    ld (hl),a
    ; DeltaX = (currentX - oldX) * 4
    ld a,(ix+ENEMY_X)       ; current X from EnemyData
    ld d,a
    ld hl,EnemyOldX
    add hl,bc
    ld a,d
    sub (hl)                ; newX - oldX (signed: -1, 0, or 1)
    add a,a
    add a,a                 ; * 4
    ld hl,EnemyDeltaX
    add hl,bc
    ld (hl),a
    ; Set EnemyPixelY = EnemyOldY * 16
    ld hl,EnemyOldY
    add hl,bc
    ld a,(hl)
    add a,a
    add a,a
    add a,a
    add a,a
    ld hl,EnemyPixelY
    add hl,bc
    ld (hl),a
    ; DeltaY = (currentY - oldY) * 4
    ld a,(ix+ENEMY_Y)
    ld d,a
    ld hl,EnemyOldY
    add hl,bc
    ld a,d
    sub (hl)
    add a,a
    add a,a
    ld hl,EnemyDeltaY
    add hl,bc
    ld (hl),a
    ; Advance IX to next enemy
    push de
    ld de,ENEMY_SIZE
    add ix,de
    pop de
    ; Next index
    pop bc
    inc c
    ld a,c
    cp MAX_ENEMIES
    jr c,_sea_loop
    ; Build initial sprite table with old pixel positions
    call BuildEnemySpriteAnim
    ret

;--------------------------------------------------------------
; AnimateEnemyStep - advance all enemy pixel positions by delta
;--------------------------------------------------------------
AnimateEnemyStep:
    ld b,MAX_ENEMIES
    ld c,0
_aes_loop:
    push bc
    ; X
    ld hl,EnemyPixelX
    ld b,0
    add hl,bc
    ld a,(hl)
    ld d,a
    ld hl,EnemyDeltaX
    add hl,bc
    ld a,d
    add a,(hl)
    ld hl,EnemyPixelX
    add hl,bc
    ld (hl),a
    ; Y
    ld hl,EnemyPixelY
    add hl,bc
    ld a,(hl)
    ld d,a
    ld hl,EnemyDeltaY
    add hl,bc
    ld a,d
    add a,(hl)
    ld hl,EnemyPixelY
    add hl,bc
    ld (hl),a
    pop bc
    inc c
    djnz _aes_loop
    call BuildEnemySpriteAnim
    ret

;--------------------------------------------------------------
; BuildEnemySpriteAnim - like BuildSpriteTable but uses pixel arrays
;--------------------------------------------------------------
BuildEnemySpriteAnim:
    xor a
    ld (NumSprites),a
    ; Player sprite
    ld a,(PlayerPixelX)
    ld d,a
    ld a,(PlayerPixelY)
    ld e,a
    ld a,(AnimFrame)
    or a
    jr nz,+
    ld c,SPR_PLAYER1
    jr ++
+:  ld c,SPR_PLAYER2
++: call AddSprite16Simple
    ; Enemy sprites using pixel arrays
    ld ix,EnemyData
    ld b,MAX_ENEMIES
    ld c,0                  ; index
_besa_loop:
    push bc
    ld a,(ix+ENEMY_TYPE)
    or a
    jr z,_besa_skip
    ; Get pixel position from arrays
    ld hl,EnemyPixelX
    ld b,0
    add hl,bc
    ld d,(hl)
    ld hl,EnemyPixelY
    add hl,bc
    ld e,(hl)
    ld a,(ix+ENEMY_TYPE)
    push bc
    call GetEnemySprBase
    ld c,a
    call AddSprite16Simple
    pop bc
_besa_skip:
    push de
    ld de,ENEMY_SIZE
    add ix,de
    pop de
    pop bc
    inc c
    djnz _besa_loop
    ; Items (scan map - same as BuildSpriteTable)
    call _besa_items
    ret

_besa_items:
    ld hl,MapBuffer
    ld d,0
_besa_iy:
    ld a,d
    cp MAP_HEIGHT
    ret z
    ld e,0
_besa_ix:
    ld a,e
    cp MAP_WIDTH
    jr z,_besa_iny
    ld a,(hl)
    call IsItemTile
    jr nz,_besa_inext
    push hl
    push de
    ld a,e
    add a,a
    add a,a
    add a,a
    add a,a
    push af
    ld a,d
    add a,a
    add a,a
    add a,a
    add a,a
    ld e,a
    pop af
    ld d,a
    ld a,(hl)
    call GetItemSprBase
    ld c,a
    call AddSprite16Simple
    pop de
    pop hl
_besa_inext:
    inc hl
    inc e
    jr _besa_ix
_besa_iny:
    inc d
    jr _besa_iy

;--------------------------------------------------------------
; UpdateAnimation - idle bob (only during TURN_IDLE)
;--------------------------------------------------------------
UpdateAnimation:
    ld a,(GameState)
    cp 1
    jp z,_ua_gameplay
    or a
    jr z,_ua_title
    cp 4
    jr z,_ua_raceSelect
    ret
_ua_title:
    ; Animate all 4 character sprites on title screen
    ld a,(AnimTimer)
    dec a
    ld (AnimTimer),a
    ret nz
    ld a,15
    ld (AnimTimer),a
    ld a,(AnimFrame)
    xor 1
    ld (AnimFrame),a
    ; Sprites 0-1 = Frog, 2-3 = Dingo, 4-5 = Manul, 6-7 = Chicken
    ld a,(AnimFrame)
    or a
    jr nz,_uat_f2
    ; Frame 1
    ld a,SPR_PLAYER1
    ld (SatBufXN+1),a
    add a,2
    ld (SatBufXN+3),a
    ld a,SPR_DINGO1
    ld (SatBufXN+5),a
    add a,2
    ld (SatBufXN+7),a
    ld a,SPR_MANUL1
    ld (SatBufXN+9),a
    add a,2
    ld (SatBufXN+11),a
    ld a,SPR_CHICKEN1
    ld (SatBufXN+13),a
    add a,2
    ld (SatBufXN+15),a
    ret
_uat_f2:
    ; Frame 2
    ld a,SPR_PLAYER2
    ld (SatBufXN+1),a
    add a,2
    ld (SatBufXN+3),a
    ld a,SPR_DINGO2
    ld (SatBufXN+5),a
    add a,2
    ld (SatBufXN+7),a
    ld a,SPR_MANUL2
    ld (SatBufXN+9),a
    add a,2
    ld (SatBufXN+11),a
    ld a,SPR_CHICKEN2
    ld (SatBufXN+13),a
    add a,2
    ld (SatBufXN+15),a
    ret

_ua_raceSelect:
    ; Animate the selected race sprite on character select screen
    ld a,(AnimTimer)
    dec a
    ld (AnimTimer),a
    ret nz
    ld a,15
    ld (AnimTimer),a
    ld a,(AnimFrame)
    xor 1
    ld (AnimFrame),a
    ; Determine which sprite pair to animate (PlayerRace * 2 = SAT XN offset)
    ld a,(PlayerRace)
    add a,a                  ; *2 (2 SAT entries per sprite)
    add a,a                  ; *4 (2 bytes per SAT XN entry)
    ld c,a
    ld b,0
    ; Look up frame 1 and frame 2 tile bases
    ld a,(PlayerRace)
    add a,a                  ; *2 (2 bytes per race in table)
    ld e,a
    ld d,0
    ld hl,_raceFrameTable
    add hl,de
    ld a,(AnimFrame)
    or a
    jr nz,+
    ld a,(hl)                ; frame 1 tile base
    jr _uars_set
+:  inc hl
    ld a,(hl)                ; frame 2 tile base
_uars_set:
    ; Write tile base to SAT XN buffer
    push af
    ld hl,SatBufXN
    add hl,bc
    inc hl                   ; XN+1 = tile index for left half
    pop af
    ld (hl),a
    add a,2
    inc hl
    inc hl                   ; XN+3 = tile index for right half
    ld (hl),a
    ret

_raceFrameTable:
.db SPR_PLAYER1, SPR_PLAYER2    ; Frog
.db SPR_DINGO1, SPR_DINGO2      ; Dingo
.db SPR_MANUL1, SPR_MANUL2      ; Manul
.db SPR_CHICKEN1, SPR_CHICKEN2  ; Chicken

_ua_gameplay:
    ld a,(MenuState)
    or a
    ret nz                  ; don't update sprites while menu is open
    ld a,(TurnPhase)
    or a
    ret nz                  ; don't bob during movement
    ld a,(AnimTimer)
    dec a
    ld (AnimTimer),a
    ret nz
    ld a,15
    ld (AnimTimer),a
    ld a,(AnimFrame)
    xor 1
    ld (AnimFrame),a
    call UpdatePlayerSprite
    ret

;--------------------------------------------------------------
;--------------------------------------------------------------
; Race Selection Screen
;--------------------------------------------------------------
ShowRaceSelect:
    call DisplayOff
    call ClearNameTable
    ; Root border
    xor a
    ld (BorderPriority),a
    ld b,2
    ld c,0
    ld d,28
    ld e,22
    call DrawRootBorder
    call InitSprites
    xor a
    ld (PlayerRace),a
    ; Title
    ld b,7
    ld c,2
    ld hl,PickRaceStr
    call PrintString
    ; Race names (col 8, rows 5,8,11,14)
    ld b,8
    ld c,5
    ld hl,FrogRaceName
    call PrintString
    ld b,8
    ld c,8
    ld hl,DingoRaceName
    call PrintString
    ld b,8
    ld c,11
    ld hl,ManulRaceName
    call PrintString
    ld b,8
    ld c,14
    ld hl,ChickenRaceName
    call PrintString
    ; Race sprites (col 4 = pixel 32, rows 5,8,11,14 = pixel Y 40,64,88,112)
    xor a
    ld (NumSprites),a
    ld d,32
    ld e,40
    ld c,SPR_PLAYER1
    call AddSprite16Simple
    ld d,32
    ld e,64
    ld c,SPR_DINGO1
    call AddSprite16Simple
    ld d,32
    ld e,88
    ld c,SPR_MANUL1
    call AddSprite16Simple
    ld d,32
    ld e,112
    ld c,SPR_CHICKEN1
    call AddSprite16Simple
    ; Draw cursor
    call _drawRaceCursor
    ; Help text
    ld b,5
    ld c,18
    ld hl,RaceConfirmStr
    call PrintString
    call DisplayOn
    ret

; Draw '>' cursor at selected race row, clear others
_drawRaceCursor:
    ld b,6
    ld c,5
    ld a,' '
    call _writeChar8
    ld b,6
    ld c,8
    ld a,' '
    call _writeChar8
    ld b,6
    ld c,11
    ld a,' '
    call _writeChar8
    ld b,6
    ld c,14
    ld a,' '
    call _writeChar8
    ; Draw '>' at selected row
    ld a,(PlayerRace)
    ld hl,_raceRows
    ld c,a
    ld b,0
    add hl,bc
    ld c,(hl)
    ld b,6
    ld a,'>'
    call _writeChar8
    ret

_raceRows: .db 5, 8, 11, 14

RaceSelectTick:
    ld a,(InputNew)
    or a
    ret z
    ; UP - previous race
    bit 0,a
    jr z,+
    ld a,(PlayerRace)
    or a
    jr z,+
    dec a
    ld (PlayerRace),a
    call PlayMenuSfx
    call _drawRaceCursor
    ret
+:
    ; DOWN - next race
    ld a,(InputNew)
    bit 1,a
    jr z,+
    ld a,(PlayerRace)
    cp 3
    jr z,+
    inc a
    ld (PlayerRace),a
    call PlayMenuSfx
    call _drawRaceCursor
    ret
+:
    ; B1 - confirm race
    ld a,(InputNew)
    bit 4,a
    ret z
    call PlayMenuSfx
    call LoadRaceSprites
    call StartNewGame       ; equips race defaults and starts play directly
    ret

;--------------------------------------------------------------
; LoadRaceSprites - load selected race's sprites to VRAM
;--------------------------------------------------------------
LoadRaceSprites:
    ; Look up sprite data pointers from RaceSpriteTable
    ld a,(PlayerRace)
    add a,a
    add a,a                 ; *4 (2 pointers per race)
    ld c,a
    ld b,0
    ld hl,RaceSpriteTable
    add hl,bc
    ; Frame 1 pointer
    ld e,(hl)
    inc hl
    ld d,(hl)               ; DE = frame 1 data
    inc hl
    push hl                 ; save table ptr for frame 2
    ; Load frame 1 to VRAM tile 256 ($2000) - 128 bytes
    di
    ld a,$00
    out ($bf),a
    ld a,$20 | $40
    out ($bf),a
    ld h,d
    ld l,e
    ld b,128
_lrs_f1:
    ld a,(hl)
    out ($be),a
    inc hl
    djnz _lrs_f1
    ei
    ; Frame 2 pointer
    pop hl
    ld e,(hl)
    inc hl
    ld d,(hl)
    ; Load frame 2 to VRAM tile 260 ($2080) - 128 bytes
    di
    ld a,$80
    out ($bf),a
    ld a,$20 | $40
    out ($bf),a
    ld h,d
    ld l,e
    ld b,128
_lrs_f2:
    ld a,(hl)
    out ($be),a
    inc hl
    djnz _lrs_f2
    ei
    ret

_closeAimMenu:
    xor a
    ld (BorderPriority),a
    call DisplayOff
    call ClearNameTable
    call RenderFullMap
    call RenderHUD
    call DisplayOn
    ret

;--------------------------------------------------------------
; FireWand - fire a ball of fire in a direction
; Input: A = direction bits from InputNew (bit0=up,1=down,2=left,3=right)
; Scans tiles from player position, hits first enemy for 10 damage
;--------------------------------------------------------------
FireWand:
    ; Convert direction bits to dx,dy
    bit 0,a
    jr nz,_fw_up
    bit 1,a
    jr nz,_fw_down
    bit 2,a
    jr nz,_fw_left
    ; Right
    ld b,1
    ld c,0
    jr _fw_scan
_fw_up:
    ld b,0
    ld c,-1
    jr _fw_scan
_fw_down:
    ld b,0
    ld c,1
    jr _fw_scan
_fw_left:
    ld b,-1
    ld c,0
_fw_scan:
    ; B=dx, C=dy. Start from player position.
    ld a,(PlayerX)
    ld d,a
    ld a,(PlayerY)
    ld e,a
    call PlayWandSfx
_fw_loop:
    ; Advance position
    ld a,d
    add a,b
    ld d,a
    ld a,e
    add a,c
    ld e,a
    ; Bounds check
    ld a,d
    cp MAP_WIDTH
    ret nc
    ld a,e
    cp MAP_HEIGHT
    ret nc
    ; Wall check
    push bc
    push de
    ld b,d
    ld c,e
    call GetMapTile
    pop de
    pop bc
    cp '#'
    ret z                   ; fizzle on wall

    ; Show fireball sprite at this position and wait
    push bc
    push de
    call BuildSpriteTable
    pop de                  ; restore tile X,Y BEFORE computing pixels
    push de
    ; Compute pixel position from tile coords
    ld a,d
    add a,a
    add a,a
    add a,a
    add a,a
    ld d,a                  ; pixel X
    ld a,e
    add a,a
    add a,a
    add a,a
    add a,a
    ld e,a                  ; pixel Y
    ld c,SPR_FIREBALL
    call AddSprite16Simple
    ; Wait 4 frames for the fireball to be visible
    halt
    halt
    halt
    halt
    pop de
    pop bc

    ; Enemy check
    push bc
    push de
    ld b,d
    ld c,e
    call FindEnemyAt
    pop de
    pop bc
    cp $FF
    jr z,_fw_loop           ; no enemy, keep going

    ; Hit an enemy! A = enemy index
    push de
    ; Get enemy data pointer
    ld hl,EnemyData
    ld de,ENEMY_SIZE
    or a
    jr z,_fw_gotEnemy
    ld b,a
_fw_seekEnemy:
    add hl,de
    djnz _fw_seekEnemy
_fw_gotEnemy:
    pop de
    ; HL -> enemy X
    ld d,(hl)
    inc hl
    ld e,(hl)               ; enemy Y
    inc hl                  ; HL -> HP
    ld a,(hl)
    sub 10
    jr c,_fw_kill
    jr z,_fw_kill
    ld (hl),a
    ret
_fw_kill:
    xor a
    ld (hl),a               ; HP = 0
    inc hl
    ld c,(hl)               ; type
    ld (hl),a               ; type = 0
    push de
    ld a,c
    call PlayDeathSfx
    pop de
    call TryDropItem
    ret

DeadTick:
WinTick:
    ld a,(InputNew)
    bit 4,a
    ret z
    call ShowTitleScreen
    xor a
    ld (GameState),a
    ret

;--------------------------------------------------------------
; Screen display routines
;--------------------------------------------------------------
ShowTitleScreen:
    call InitTitleMusic
    call DisplayOff
    call ClearNameTable
    call InitSprites
    ; Reload sprite tiles (resets frog to tile 0 after race select)
    di
    ld a,$00
    out ($bf),a
    ld a,$20 | $40
    out ($bf),a
    ld hl,PlayerSpr1Data
    ld bc,SpriteTileDataEnd-PlayerSpr1Data
_sts_loadSpr:
    ld a,(hl)
    out ($be),a
    inc hl
    dec bc
    ld a,b
    or c
    jp nz,_sts_loadSpr
    ei
    ; "Den" centered (col 14)
    ld b,14
    ld c,1
    ld hl,TitleString
    call PrintString
    ; "Monsters:" centered (row 8)
    ld b,12
    ld c,8
    ld hl,MonstersString
    call PrintString
    ; Monster labels (row 12) centered under each sprite
    ; Sprites at pixel X: 24,88,152,216 = hw col 3,11,19,27 (center col 4,12,20,28)
    ld b,3
    ld c,12
    ld hl,RatString
    call PrintString
    ld b,10
    ld c,12
    ld hl,SnakeString
    call PrintString
    ld b,19
    ld c,12
    ld hl,OrcString
    call PrintString
    ld b,25
    ld c,12
    ld hl,BadgerString
    call PrintString
    ; "Press Start" centered (row 16)
    ld b,10
    ld c,16
    ld hl,PressStartString
    call PrintString
    ; Build timestamp (row 19)
    ld b,5
    ld c,19
    ld hl,BuildTimeString
    call PrintString
    ; Copyright (row 20)
    ld b,1
    ld c,20
    ld hl,CopyrightString
    call PrintString
    ; Sprites: 4 characters (row 4, pixel Y=32) evenly spaced
    ; No labels - just the sprites
    xor a
    ld (NumSprites),a
    ld d,24
    ld e,32
    ld c,SPR_PLAYER1
    call AddSprite16Simple
    ld d,88
    ld e,32
    ld c,SPR_DINGO1
    call AddSprite16Simple
    ld d,152
    ld e,32
    ld c,SPR_MANUL1
    call AddSprite16Simple
    ld d,216
    ld e,32
    ld c,SPR_CHICKEN1
    call AddSprite16Simple
    ; 4 monsters (row 10, pixel Y=80) evenly spaced
    ld d,24
    ld e,80
    ld c,SPR_RAT
    call AddSprite16Simple
    ld d,88
    ld e,80
    ld c,SPR_SNAKE
    call AddSprite16Simple
    ld d,152
    ld e,80
    ld c,SPR_ORC
    call AddSprite16Simple
    ld d,216
    ld e,80
    ld c,SPR_BADGER
    call AddSprite16Simple
    call DisplayOn
    ret

ShowDeadScreen:
    call InitSadMusic
    call DisplayOff
    call ClearNameTable
    xor a
    ld (BorderPriority),a
    ld b,6
    ld c,8
    ld d,20
    ld e,10
    call DrawRootBorder
    call InitSprites
    ld b,11
    ld c,10
    ld hl,GameOverString
    call PrintString
    ld b,10
    ld c,14
    ld hl,PressRestartString
    call PrintString
    call DisplayOn
    ret

ShowWinScreen:
    call InitWinMusic
    call DisplayOff
    call ClearNameTable
    xor a
    ld (BorderPriority),a
    ld b,6
    ld c,8
    ld d,20
    ld e,10
    call DrawRootBorder
    call InitSprites
    ld b,10
    ld c,10
    ld hl,WinString
    call PrintString
    ld b,10
    ld c,14
    ld hl,PressRestartString
    call PrintString
    call DisplayOn
    ret
