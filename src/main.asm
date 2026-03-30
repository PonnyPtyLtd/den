.include "header.inc"

.bank 0 slot 0
.org $0000
;==============================================================
; Boot section
;==============================================================
    di
    im 1
    jp main

.org $0038
;==============================================================
; VBlank interrupt handler
;==============================================================
    push af
    push bc
    push de
    push hl
    in a,($bf)
    call CopySATToVRAM      ; copy sprite table first (time-critical)
    call UpdateMusic
    call UpdateSfx
    pop hl
    pop de
    pop bc
    pop af
    ei
    reti

.org $0066
;==============================================================
; Pause button handler
;==============================================================
    retn

;==============================================================
; Main program
;==============================================================
main:
    ld sp,$dff0

    ; Set up VDP registers
    ld hl,VdpData
    ld b,VdpDataEnd-VdpData
    ld c,$bf
    otir

    ; Clear VRAM
    ld a,$00
    out ($bf),a
    ld a,$40
    out ($bf),a
    ld bc,$4000
_clearVRAM:
    ld a,$00
    out ($be),a
    dec bc
    ld a,b
    or c
    jp nz,_clearVRAM

    ; Load palette (32 bytes)
    ld a,$00
    out ($bf),a
    ld a,$c0
    out ($bf),a
    ld hl,PaletteData
    ld b,(PaletteDataEnd-PaletteData)
    ld c,$be
    otir

    ; Load font tiles (8x8, tiles 0-94, VRAM $0000)
    ld a,$00
    out ($bf),a
    ld a,$40
    out ($bf),a
    ld hl,FontData
    ld bc,FontDataEnd-FontData
_loadFont:
    ld a,(hl)
    out ($be),a
    inc hl
    dec bc
    ld a,b
    or c
    jp nz,_loadFont

    ; Load background terrain tiles (tiles 95+, VRAM $0BE0)
    ld a,$E0
    out ($bf),a
    ld a,$0B | $40
    out ($bf),a
    ld hl,WallTLData
    ld bc,BgTileCount * 32
_loadBgTiles:
    ld a,(hl)
    out ($be),a
    inc hl
    dec bc
    ld a,b
    or c
    jp nz,_loadBgTiles

    ; Load sprite tiles (tiles 256+, VRAM $2000)
    ld a,$00
    out ($bf),a
    ld a,$20 | $40
    out ($bf),a
    ld hl,PlayerSpr1Data
    ld bc,SpriteTileDataEnd-PlayerSpr1Data
_loadSprTiles:
    ld a,(hl)
    out ($be),a
    inc hl
    dec bc
    ld a,b
    or c
    jp nz,_loadSprTiles

    ; Initialize sprites (hide all)
    call InitSprites

    ; Initialize music
    call InitDungeonMusic

    ; Initialize game and show title
    call GameInit
    call ShowTitleScreen

    ; Enable interrupts
    ei

GameLoop:
    halt
    call UpdateAnimation
    call GameTick
    jp GameLoop

;==============================================================
; Includes
;==============================================================
.include "vdp.inc"
.include "input.inc"
.include "utils.inc"
.include "render.inc"
.include "dungeon.inc"
.include "player.inc"
.include "enemies.inc"
.include "game.inc"
.include "menu.inc"
.include "sfx.inc"
.include "music.inc"
.include "data/strings.inc"
.include "data/tiles.inc"
.include "data/font.inc"
.include "buildtime.inc"
