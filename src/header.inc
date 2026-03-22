;==============================================================
; Constants
;==============================================================
.define MAP_WIDTH 16
.define MAP_HEIGHT 11
.define MAP_SIZE 176
.define MAX_ENEMIES 8
.define ENEMY_SIZE 5
.define ENEMY_X 0
.define ENEMY_Y 1
.define ENEMY_HP 2
.define ENEMY_TYPE 3
.define ENEMY_ATK 4
.define MAX_ROOMS 5
.define ROOM_DATA_SIZE 4
.define ATTR_PAL0 $00
.define ATTR_PAL1 $08

; Animation
.define ANIM_FRAMES 4
.define ANIM_PX_STEP 4
.define TURN_IDLE 0
.define TURN_PLAYER_ANIM 1
.define TURN_ENEMY_ANIM 2

; Sprite tile bases (relative to VRAM tile 256, for 8x16 mode)
.define SPR_PLAYER1 0
.define SPR_PLAYER2 4
.define SPR_RAT     8
.define SPR_SNAKE   12
.define SPR_ORC     16
.define SPR_WEAPON  20
.define SPR_SHIELD  24
.define SPR_BADGER  28
.define SPR_BODYARMOR 32
.define SPR_POTION   36
.define SPR_WAND     40
; Per-tier item sprites
.define SPR_WEAPON1  44
.define SPR_WEAPON2  48
.define SPR_WEAPON3  52
.define SPR_SHIELD1  56
.define SPR_SHIELD2  60
.define SPR_SHIELD3  64
.define SPR_ARMOR1   68
.define SPR_ARMOR2   72
.define SPR_ARMOR3   76

.define MAX_SPRITES 34

;==============================================================
; WLA-DX banking setup
;==============================================================
.memorymap
defaultslot 0
slotsize $8000
slot 0 $0000
slot 1 $c000
.endme

.rombankmap
bankstotal 1
banksize $8000
banks 1
.endro

;==============================================================
; RAM variables
;==============================================================
.ramsection "Variables" slot 1
RandomNumberGeneratorWord dw
Ch0Ptr dw
Ch0Timer db
Ch1Ptr dw
Ch1Timer db
Ch2Ptr dw
Ch2Timer db
GameState db
DungeonLevel db
PlayerX db
PlayerY db
PlayerHP db
PlayerMaxHP db
PlayerAtk db
PlayerDef db
PlayerWeapon db
PlayerArmor db
InputPrevious db
InputNew db
InputCurrent db
NumRooms db
RoomData dsb 20
MapBuffer dsb 176
EnemyData dsb 40
AnimTimer db
AnimFrame db
Ch0Start dw
Ch1Start dw
Ch2Start dw
TurnPhase db
AnimCounter db
PlayerPixelX db
PlayerPixelY db
PlayerDeltaX db
PlayerDeltaY db
EnemyOldX dsb 8
EnemyOldY dsb 8
SatBufY dsb 64
SatBufXN dsb 128
NumSprites db
SfxTimer db
; Inventory (0=empty, 1-9=item type)
InvLeftHand db      ; weapon slot
InvRightHand db     ; shield slot
InvBody db          ; armor slot
InvHead db          ; helmet slot
InvPocket1 db
InvPocket2 db
MenuState db        ; 0=none, 1=radial, 2=inventory
MenuCursor db
InvSubMenu db       ; 0=none, 1=equip highlighted, 2=drop highlighted
AttackPhase db
AttackDirX db
AttackDirY db
PlayerRace db           ; 0=frog, 1=dingo, 2=manul, 3=chicken
LastMoveDir db
LevelDropCount db       ; items dropped this level (for guaranteed drop)
; Enemy animation pixel tracking
EnemyPixelX dsb 8
EnemyPixelY dsb 8
EnemyDeltaX dsb 8
EnemyDeltaY dsb 8
; Attack animation
AttackTargetIdx db
AttackDamage db
; Item drafting
DraftRound db
DraftPool dsb 4
DraftCursor db
DraftPoolSize db
; Border drawing params
BorderX db
BorderY db
BorderW db
BorderH db
BorderPriority db       ; nonzero = use priority bit (menu over sprites)
.ends

; Turn phases (extended)
.define TURN_PLAYER_ATK_FWD 3
.define TURN_PLAYER_ATK_BACK 4

; SRAM save data addresses ($8000-$BFFF when enabled)
; 2 slots: Round 0 leftovers feed next player's Round 1,
;          Round 1 leftovers feed next player's Round 2
.define SRAM_MAGIC      $8000
.define SRAM_MAGIC_VAL  $A5
; Slot A: prev player's Round 0 leftovers (3 items) → this player's Round 1
.define SRAM_SLOTA_SZ   $8001
.define SRAM_SLOTA      $8002   ; 3 bytes max
; Slot B: prev player's Round 1 leftovers (2 items) → this player's Round 2
.define SRAM_SLOTB_SZ   $8005
.define SRAM_SLOTB      $8006   ; 2 bytes max

;==============================================================
; SDSC tag
;==============================================================
.sdsctag 1.2,"Ponny's SMS Roguelike","A simple roguelike","Ponny"
