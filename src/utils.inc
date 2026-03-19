;==============================================================
; Utility routines
;==============================================================

;--------------------------------------------------------------
; GetRandomNumber - 16-bit LFSR PRNG
; Returns pseudo-random byte in A
;--------------------------------------------------------------
GetRandomNumber:
    push hl
        ld hl,(RandomNumberGeneratorWord)
        ld a,h
        rrca
        rrca
        xor h
        rrca
        xor l
        rrca
        rrca
        rrca
        rrca
        xor l
        rra
        adc hl,hl
        jr nz,+
        ld hl,$733c
+:      ld a,r
        xor l
        ld (RandomNumberGeneratorWord),hl
    pop hl
    ret

;--------------------------------------------------------------
; GetRandomRange - random number in range [0, A-1]
; Input: A = range (must be > 0)
; Output: A = random value
;--------------------------------------------------------------
GetRandomRange:
    ld b,a
    call GetRandomNumber
_grr_mod:
    cp b
    ret c
    sub b
    jr _grr_mod

;--------------------------------------------------------------
; GetMapAddress - calculate MapBuffer address
; Input: B=X (0-31), C=Y (0-21)
; Output: HL = MapBuffer + Y*32 + X
; Clobbers: BC
;--------------------------------------------------------------
GetMapAddress:
    ld h,0
    ld l,c
    add hl,hl
    add hl,hl
    add hl,hl
    add hl,hl
    add hl,hl              ; Y * 32
    ld c,b
    ld b,0
    add hl,bc              ; + X
    ld bc,MapBuffer
    add hl,bc              ; + base
    ret

;--------------------------------------------------------------
; GetMapTile - read tile from map buffer
; Input: B=X, C=Y
; Output: A = tile (ASCII)
; Clobbers: HL, BC
;--------------------------------------------------------------
GetMapTile:
    call GetMapAddress
    ld a,(hl)
    ret

;--------------------------------------------------------------
; SetMapTile - write tile to map buffer
; Input: B=X, C=Y, A=tile
; Clobbers: HL, BC
;--------------------------------------------------------------
SetMapTile:
    push af
    call GetMapAddress
    pop af
    ld (hl),a
    ret

;--------------------------------------------------------------
; DisplayOff - disable display, keep VBlank IRQ enabled
;--------------------------------------------------------------
DisplayOff:
    ld a,%10100000
    out ($bf),a
    ld a,$81
    out ($bf),a
    ret

;--------------------------------------------------------------
; DisplayOn - enable display and VBlank IRQ
;--------------------------------------------------------------
DisplayOn:
    ld a,%11100000
    out ($bf),a
    ld a,$81
    out ($bf),a
    ret
