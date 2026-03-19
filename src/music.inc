;==============================================================
; Music engine
;==============================================================
; Data format: 4 bytes per note (psg_tone_latch, psg_tone_data, psg_volume, duration)
; $FF = loop back to start
;
; SMS PSG (SN76489) tone values = 3579545 / (32 * freq_hz)
; Volume: $9x for ch0, $Bx for ch1, $Dx for ch2 (x=0 loudest, x=F mute)

InitMusic:
    ld hl,MelodyData
    ld (Ch0Ptr),hl
    ld hl,BassData
    ld (Ch1Ptr),hl
    ld hl,HarmonyData
    ld (Ch2Ptr),hl
    ld a,1
    ld (Ch0Timer),a
    ld (Ch1Timer),a
    ld (Ch2Timer),a
    ret

UpdateMusic:
    ; --- Channel 0 (melody) ---
    ld a,(Ch0Timer)
    dec a
    ld (Ch0Timer),a
    or a
    jr nz,_ch1

    ld hl,(Ch0Ptr)
_ch0Read:
    ld a,(hl)
    cp $FF
    jr nz,+
    ld hl,MelodyData
    jr _ch0Read
+:
    out ($7F),a         ; tone latch (channel + low 4 bits)
    inc hl
    ld a,(hl)
    out ($7F),a         ; tone data (high 6 bits)
    inc hl
    ld a,(hl)
    out ($7F),a         ; volume
    inc hl
    ld a,(hl)
    ld (Ch0Timer),a     ; duration in frames
    inc hl
    ld (Ch0Ptr),hl

_ch1:
    ; --- Channel 1 (bass) ---
    ld a,(Ch1Timer)
    dec a
    ld (Ch1Timer),a
    or a
    jr nz,_ch2

    ld hl,(Ch1Ptr)
_ch1Read:
    ld a,(hl)
    cp $FF
    jr nz,+
    ld hl,BassData
    jr _ch1Read
+:
    out ($7F),a         ; tone latch
    inc hl
    ld a,(hl)
    out ($7F),a         ; tone data
    inc hl
    ld a,(hl)
    out ($7F),a         ; volume
    inc hl
    ld a,(hl)
    ld (Ch1Timer),a     ; duration
    inc hl
    ld (Ch1Ptr),hl

_ch2:
    ; --- Channel 2 (harmony) ---
    ld a,(Ch2Timer)
    dec a
    ld (Ch2Timer),a
    or a
    ret nz

    ld hl,(Ch2Ptr)
_ch2Read:
    ld a,(hl)
    cp $FF
    jr nz,+
    ld hl,HarmonyData
    jr _ch2Read
+:
    out ($7F),a         ; tone latch
    inc hl
    ld a,(hl)
    out ($7F),a         ; tone data
    inc hl
    ld a,(hl)
    out ($7F),a         ; volume
    inc hl
    ld a,(hl)
    ld (Ch2Timer),a     ; duration
    inc hl
    ld (Ch2Ptr),hl

    ret

;==============================================================
; Music data - Puzzle roguelike (D Dorian)
;==============================================================
; D Dorian = D E F G A B C - brighter than natural minor
; Tone values: D4=381 E4=339 F4=320 G4=286 A4=254
;              C4=428 B3=453 A3=509 G3=571 F3=641 E3=679 D3=762

; Melody - channel 0
; 8 phrases, each 100 frames = ~13 seconds total
MelodyData:
    ; Phrase 1 - curious, ascending
    .db $8D,$17,$92,25   ; D4
    .db $83,$15,$92,25   ; E4
    .db $80,$14,$92,40   ; F4 (linger)
    .db $80,$00,$9F,10   ; rest
    ; Phrase 2 - settling back
    .db $83,$15,$92,20   ; E4
    .db $8D,$17,$92,20   ; D4
    .db $83,$15,$92,50   ; E4 (long, thoughtful)
    .db $80,$00,$9F,10   ; rest
    ; Phrase 3 - reaching higher
    .db $80,$14,$92,25   ; F4
    .db $8E,$11,$92,25   ; G4
    .db $8E,$0F,$92,40   ; A4 (peak)
    .db $80,$00,$9F,10   ; rest
    ; Phrase 4 - graceful descent
    .db $8E,$11,$92,20   ; G4
    .db $80,$14,$92,20   ; F4
    .db $83,$15,$92,50   ; E4 (long, resolving)
    .db $80,$00,$9F,10   ; rest
    ; Phrase 5 - playful
    .db $8D,$17,$92,25   ; D4
    .db $80,$14,$92,25   ; F4
    .db $83,$15,$92,25   ; E4
    .db $8D,$17,$92,15   ; D4 (quick)
    .db $80,$00,$9F,10   ; rest
    ; Phrase 6 - contemplative
    .db $8C,$1A,$92,25   ; C4
    .db $8D,$17,$92,25   ; D4
    .db $83,$15,$92,40   ; E4 (linger)
    .db $80,$00,$9F,10   ; rest
    ; Phrase 7 - ascending discovery (B natural = Dorian color)
    .db $8D,$1F,$92,20   ; A3
    .db $85,$1C,$92,20   ; B3
    .db $8C,$1A,$92,20   ; C4
    .db $8D,$17,$92,30   ; D4
    .db $80,$00,$9F,10   ; rest
    ; Phrase 8 - gentle resolution
    .db $83,$15,$92,20   ; E4
    .db $8D,$17,$92,20   ; D4
    .db $8C,$1A,$92,20   ; C4
    .db $8D,$17,$92,30   ; D4 (home)
    .db $80,$00,$9F,10   ; rest
    .db $FF               ; loop

; Bass - channel 1
; Walking pattern, legato (200 frames, loops 4x over melody)
; D3=762, A3=509, G3=571
BassData:
    .db $AA,$2F,$B3,50   ; D3 - root
    .db $AD,$1F,$B3,50   ; A3 - fifth
    .db $AB,$23,$B3,50   ; G3 - fourth
    .db $AD,$1F,$B3,50   ; A3 - fifth
    .db $FF               ; loop

; Harmony - channel 2
; Slow-shifting pad, slightly out of phase with melody (720 frames)
; for subtle variety on each repeat
; F3=641, G3=571, A3=509
HarmonyData:
    .db $C1,$28,$D6,180  ; F3 pad (warm minor 3rd)
    .db $CB,$23,$D6,180  ; G3 pad (bright 4th)
    .db $CD,$1F,$D6,180  ; A3 pad (open 5th)
    .db $CB,$23,$D6,180  ; G3 pad
    .db $FF               ; loop
