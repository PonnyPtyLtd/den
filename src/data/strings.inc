;==============================================================
; Game text strings (null-terminated ASCII)
;==============================================================

TitleString:    .db "Den",0
CopyrightString: .db "(c) 2025 John ",34,"Ponny",34," Sherwood",0
PressStartString: .db "Press Start",0
GameOverString: .db "GAME OVER",0
WinString:      .db "You escaped!",0
PressRestartString: .db "Press Start",0

; Title screen showcase
CharactersString: .db "Characters:",0
MonstersString: .db "Monsters:",0
ItemsString:    .db "Items:",0
RatString:      .db "Rat",0
SnakeString:    .db "Snake",0
OrcString:      .db "Orc",0
BadgerString:   .db "Badger",0
SwordString:    .db "Sword",0
ShieldString:   .db "Shield",0
YouString:      .db "You!",0

; Inventory screen
InvTitleString:   .db "== INVENTORY ==",0
LeftHandString:   .db "Weapon:",0
RightHandString:  .db "Shield:",0
BodyString:       .db "Armor:",0
Pocket1String:    .db "Pocket1:",0
Pocket2String:    .db "Pocket2:",0
InvHelpString:    .db "U/D:Sel St:Act 2:Back",0

; Item sub-menu
EquipString:    .db "> Equip",0
DropString:     .db "> Drop",0

EmptyString:    .db "-empty-",0
PocketFullStr:  .db "Pockets full!",0

; Item names (9 items)
SharpStickStr:  .db "Sharp Stick",0
LacquerStr:     .db "Lacquer Blade",0
BogOakMaceStr:  .db "Bog Oak Mace",0
WoodShieldStr:  .db "Wood Shield",0
ShellShieldStr: .db "Shell Shield",0
OakShieldStr:   .db "Oak Shield",0
ReedArmorStr:   .db "Reed Armor",0
ShellArmorStr:  .db "Shell Armor",0
OakArmorStr:    .db "Oak Armor",0
HelmetStr:      .db "Helmet",0
PotionStr:      .db "Heal Potion",0
WandStr:        .db "Fire Wand",0

HeadString:       .db "Head:",0
UseString:        .db "> Use",0

; Race names
FrogShortStr:   .db "Frog",0
FrogRaceName:   .db "Poison Arrow Frog",0
DingoRaceName:  .db "Dingo",0
ManulRaceName:  .db "Manul",0
ChickenRaceName:.db "Polish Chicken",0
PickRaceStr:    .db "Choose your hero!",0
RaceConfirmStr: .db "Start:Go  U/D:Change",0

; Drafting
DraftTitleStr:  .db "Choose an item!",0
DraftHelpStr:   .db "L/R:Pick  Start:Take",0
DraftRoundStr:  .db "Pick",0
DraftOfStr:     .db "of",0

; Race name pointer table
RaceNameTable:
.dw FrogRaceName
.dw DingoRaceName
.dw ManulRaceName
.dw ChickenRaceName

; Item name pointer table (indexed by type 0-12)
ItemNameTable:
.dw EmptyString     ; 0
.dw SharpStickStr   ; 1
.dw LacquerStr      ; 2
.dw BogOakMaceStr   ; 3
.dw WoodShieldStr   ; 4
.dw ShellShieldStr  ; 5
.dw OakShieldStr    ; 6
.dw ReedArmorStr    ; 7
.dw ShellArmorStr   ; 8
.dw OakArmorStr     ; 9
.dw HelmetStr       ; 10
.dw PotionStr       ; 11
.dw WandStr         ; 12
