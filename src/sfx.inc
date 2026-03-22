;==============================================================
; Sound effects - uses PSG noise channel (ch 3)
;==============================================================

UpdateSfx:
    ld a,(SfxTimer)
    or a
    ret z
    dec a
    ld (SfxTimer),a
    ret nz
    ; Timer expired - mute noise channel
    ld a,$FF
    out ($7F),a
    ret

;--------------------------------------------------------------
; PlayAttackSfx - "bashing" thud sound
; Input: A = damage dealt
; Periodic noise at low rate = deep thud
;--------------------------------------------------------------
PlayAttackSfx:
    cp 5
    jr nc,_pas_heavy
    cp 3
    jr nc,_pas_med
    ; Light bash
    ld a,$E2                ; periodic, low rate (deep)
    out ($7F),a
    ld a,$F2                ; moderate volume
    out ($7F),a
    ld a,5
    ld (SfxTimer),a
    ret
_pas_med:
    ; Solid bash
    ld a,$E1                ; periodic, medium rate
    out ($7F),a
    ld a,$F0                ; loud
    out ($7F),a
    ld a,7
    ld (SfxTimer),a
    ret
_pas_heavy:
    ; Heavy slam
    ld a,$E0                ; periodic, high rate (crunchier)
    out ($7F),a
    ld a,$F0                ; max volume
    out ($7F),a
    ld a,10
    ld (SfxTimer),a
    ret

;--------------------------------------------------------------
; PlayHurtSfx - short "beep" when taking damage
; Input: A = damage taken
; White noise at high rate, very short = sharp pip
;--------------------------------------------------------------
PlayHurtSfx:
    cp 5
    jr nc,_phs_heavy
    cp 3
    jr nc,_phs_med
    ; Light pip
    ld a,$E4                ; white noise, high rate
    out ($7F),a
    ld a,$F4                ; quiet
    out ($7F),a
    ld a,2                  ; very short
    ld (SfxTimer),a
    ret
_phs_med:
    ; Sharp beep
    ld a,$E4
    out ($7F),a
    ld a,$F1
    out ($7F),a
    ld a,3
    ld (SfxTimer),a
    ret
_phs_heavy:
    ; Loud crack
    ld a,$E5                ; white noise, medium rate
    out ($7F),a
    ld a,$F0
    out ($7F),a
    ld a,4
    ld (SfxTimer),a
    ret

;--------------------------------------------------------------
; PlayMenuSfx - short click for menu navigation
;--------------------------------------------------------------
PlayMenuSfx:
    ld a,$E0
    out ($7F),a
    ld a,$F4
    out ($7F),a
    ld a,2
    ld (SfxTimer),a
    ret

;--------------------------------------------------------------
; PlayDeathSfx - monster death scream based on type
; Input: A = enemy type (1=rat, 2=snake, 3=orc, 4=badger)
;--------------------------------------------------------------
PlayDeathSfx:
    cp 1
    jr z,_pds_rat
    cp 2
    jr z,_pds_snake
    cp 4
    jr z,_pds_badger
    ; Orc: deep grunt
    ld a,$E2                ; periodic, low rate
    out ($7F),a
    ld a,$F0                ; loud
    out ($7F),a
    ld a,8
    ld (SfxTimer),a
    ret
_pds_rat:
    ; Rat: high-pitched squeak
    ld a,$E0                ; periodic, highest rate
    out ($7F),a
    ld a,$F1
    out ($7F),a
    ld a,4
    ld (SfxTimer),a
    ret
_pds_snake:
    ; Snake: hiss
    ld a,$E5                ; white noise, medium rate
    out ($7F),a
    ld a,$F0
    out ($7F),a
    ld a,6
    ld (SfxTimer),a
    ret
; Wand zap - crackling energy
PlayWandSfx:
    ld a,$E4                ; white noise, high rate
    out ($7F),a
    ld a,$F0
    out ($7F),a
    ld a,10
    ld (SfxTimer),a
    ret

_pds_badger:
    ; Honey Badger: angry snarl
    ld a,$E6                ; white noise, low rate = rumble
    out ($7F),a
    ld a,$F0                ; max volume
    out ($7F),a
    ld a,12
    ld (SfxTimer),a
    ret
