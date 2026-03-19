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
    push hl
    in a,($bf)
    call UpdateMusic
    pop hl
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

    ; Load palette (32 bytes - both palettes)
    ld a,$00
    out ($bf),a
    ld a,$c0
    out ($bf),a
    ld hl,PaletteData
    ld b,(PaletteDataEnd-PaletteData)
    ld c,$be
    otir

    ; Load font tiles
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

    ; Initialize music
    call InitMusic

    ; Initialize game and show title
    call GameInit
    call ShowTitleScreen

    ; Enable interrupts - music starts on first VBlank
    ei

GameLoop:
    halt
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
.include "music.inc"
.include "data/strings.inc"
.include "data/font.inc"
