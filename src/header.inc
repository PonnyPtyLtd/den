;==============================================================
; Constants
;==============================================================
.define MAP_WIDTH 32
.define MAP_HEIGHT 22
.define MAP_SIZE 704
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
InputPrevious db
InputNew db
NumRooms db
RoomData dsb 20
MapBuffer dsb 704
EnemyData dsb 40
.ends

;==============================================================
; SDSC tag
;==============================================================
.sdsctag 1.2,"Ponny's SMS Roguelike","A simple roguelike","Ponny"
