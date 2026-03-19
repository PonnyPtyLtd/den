;==============================================================
; Game state machine
;==============================================================
; GameState: 0=title, 1=playing, 2=dead, 3=win

GameInit:
    xor a
    ld (GameState),a
    ld (InputPrevious),a
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
    ret

;--------------------------------------------------------------
; TitleTick - seed RNG while waiting, start on Button 1
;--------------------------------------------------------------
TitleTick:
    ; Continuously stir RNG while on title screen
    ld hl,(RandomNumberGeneratorWord)
    inc hl
    ld (RandomNumberGeneratorWord),hl
    ld a,(InputNew)
    bit 4,a
    ret z
    call StartNewGame
    ret

;--------------------------------------------------------------
; StartNewGame
;--------------------------------------------------------------
StartNewGame:
    ld a,1
    ld (DungeonLevel),a
    ld a,20
    ld (PlayerHP),a
    ld (PlayerMaxHP),a
    ld a,4
    ld (PlayerAtk),a
    call GenerateLevel
    ret

;--------------------------------------------------------------
; GenerateLevel - generate dungeon and full render
;--------------------------------------------------------------
GenerateLevel:
    call GenerateDungeon
    call SpawnEnemies
    call DisplayOff
    call ClearNameTable
    call RenderFullMap
    call RenderHUD
    call DisplayOn
    ld a,1
    ld (GameState),a
    ret

;--------------------------------------------------------------
; PlayTick - main gameplay turn
;--------------------------------------------------------------
PlayTick:
    ld a,(InputNew)
    and $0F                 ; direction bits only
    ret z                   ; no direction = no turn
    ; Erase entities
    call EraseEntities
    ; Player move/attack
    call ProcessPlayerInput
    ; Check stairs
    ld a,(PlayerX)
    ld b,a
    ld a,(PlayerY)
    ld c,a
    call GetMapTile
    cp '>'
    jr nz,_pt_noStairs
    ; Descend
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
    ; Enemy turns
    call UpdateEnemies
    ; Check death
    ld a,(PlayerHP)
    or a
    jr nz,_pt_alive
    call ShowDeadScreen
    ld a,2
    ld (GameState),a
    ret
_pt_alive:
    call DrawEntities
    call RenderHUD
    ret

;--------------------------------------------------------------
; DeadTick / WinTick - wait for Button 1 to return to title
;--------------------------------------------------------------
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
    call DisplayOff
    call ClearNameTable
    ld b,5
    ld c,8
    ld hl,TitleString
    call PrintString
    ld b,9
    ld c,12
    ld hl,PressStartString
    call PrintString
    call DisplayOn
    ret

ShowDeadScreen:
    call DisplayOff
    call ClearNameTable
    ld b,11
    ld c,10
    ld hl,GameOverString
    call PrintString
    ld b,6
    ld c,14
    ld hl,PressRestartString
    call PrintString
    call DisplayOn
    ret

ShowWinScreen:
    call DisplayOff
    call ClearNameTable
    ld b,10
    ld c,10
    ld hl,WinString
    call PrintString
    ld b,6
    ld c,14
    ld hl,PressRestartString
    call PrintString
    call DisplayOn
    ret
