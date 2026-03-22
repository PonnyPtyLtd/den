;==============================================================
; Music engine
;==============================================================
; Data format: 4 bytes per note (psg_tone_latch, psg_tone_data, psg_volume, duration)
; $FF = loop back to ChXStart
;
; SMS PSG (SN76489) tone values = 3579545 / (32 * freq_hz)
; Volume: $9x for ch0, $Bx for ch1, $Dx for ch2 (x=0 loudest, x=F mute)

;--------------------------------------------------------------
; Song init functions - set channel pointers and loop targets
;--------------------------------------------------------------
InitDungeonMusic:
    ld hl,MelodyData
    ld (Ch0Ptr),hl
    ld (Ch0Start),hl
    ld hl,BassData
    ld (Ch1Ptr),hl
    ld (Ch1Start),hl
    ld hl,HarmonyData
    ld (Ch2Ptr),hl
    ld (Ch2Start),hl
    ld a,1
    ld (Ch0Timer),a
    ld (Ch1Timer),a
    ld (Ch2Timer),a
    ret

InitSadMusic:
    ld hl,SadMelodyData
    ld (Ch0Ptr),hl
    ld (Ch0Start),hl
    ld hl,SadBassData
    ld (Ch1Ptr),hl
    ld (Ch1Start),hl
    ld hl,SadHarmonyData
    ld (Ch2Ptr),hl
    ld (Ch2Start),hl
    ld a,1
    ld (Ch0Timer),a
    ld (Ch1Timer),a
    ld (Ch2Timer),a
    ret

InitTitleMusic:
    ld hl,TitleMelodyData
    ld (Ch0Ptr),hl
    ld (Ch0Start),hl
    ld hl,TitleBassData
    ld (Ch1Ptr),hl
    ld (Ch1Start),hl
    ld hl,TitleHarmonyData
    ld (Ch2Ptr),hl
    ld (Ch2Start),hl
    ld a,1
    ld (Ch0Timer),a
    ld (Ch1Timer),a
    ld (Ch2Timer),a
    ret

InitWinMusic:
    ld hl,WinMelodyData
    ld (Ch0Ptr),hl
    ld (Ch0Start),hl
    ld hl,WinBassData
    ld (Ch1Ptr),hl
    ld (Ch1Start),hl
    ld hl,WinHarmonyData
    ld (Ch2Ptr),hl
    ld (Ch2Start),hl
    ld a,1
    ld (Ch0Timer),a
    ld (Ch1Timer),a
    ld (Ch2Timer),a
    ret

;--------------------------------------------------------------
; UpdateMusic - called every VBlank from ISR
;--------------------------------------------------------------
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
    ld hl,(Ch0Start)
    jr _ch0Read
+:
    out ($7F),a
    inc hl
    ld a,(hl)
    out ($7F),a
    inc hl
    ld a,(hl)
    out ($7F),a
    inc hl
    ld a,(hl)
    ld (Ch0Timer),a
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
    ld hl,(Ch1Start)
    jr _ch1Read
+:
    out ($7F),a
    inc hl
    ld a,(hl)
    out ($7F),a
    inc hl
    ld a,(hl)
    out ($7F),a
    inc hl
    ld a,(hl)
    ld (Ch1Timer),a
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
    ld hl,(Ch2Start)
    jr _ch2Read
+:
    out ($7F),a
    inc hl
    ld a,(hl)
    out ($7F),a
    inc hl
    ld a,(hl)
    out ($7F),a
    inc hl
    ld a,(hl)
    ld (Ch2Timer),a
    inc hl
    ld (Ch2Ptr),hl

    ret

;==============================================================
; DUNGEON MUSIC - soft, melodic, contemplative
;==============================================================
; Gentle flowing melody in D minor pentatonic
; Softer volumes, connected phrases, pleasant to think to

MelodyData:
    ; Phrase 1 - gentle descending melody
    .db $8D,$17,$97,30   ; D4 (soft!)
    .db $8C,$1A,$97,25   ; C4
    .db $8D,$1F,$97,30   ; A3
    .db $80,$00,$9F,15   ; rest
    ; Phrase 2 - warm rising answer
    .db $8C,$1A,$97,25   ; C4
    .db $8D,$17,$97,30   ; D4
    .db $80,$14,$97,35   ; F4
    .db $80,$00,$9F,15   ; rest
    ; Phrase 3 - gentle peak
    .db $8E,$11,$97,25   ; G4
    .db $80,$14,$97,20   ; F4
    .db $8D,$17,$97,25   ; D4
    .db $8C,$1A,$97,20   ; C4
    .db $8D,$1F,$97,30   ; A3
    .db $80,$00,$9F,20   ; rest
    ; Phrase 4 - soft resolution
    .db $8C,$1A,$97,25   ; C4
    .db $8D,$17,$97,35   ; D4 (home)
    .db $80,$00,$9F,30   ; breathe
    .db $FF

BassData:
    ; Gentle walking bass, very soft
    .db $AA,$2F,$B8,50   ; D3 (very soft)
    .db $A1,$28,$B8,50   ; F3
    .db $AB,$23,$B8,50   ; G3
    .db $AD,$1F,$B8,50   ; A3
    .db $FF

HarmonyData:
    ; Warm pad, barely audible
    .db $C1,$28,$DA,180  ; F3 whisper
    .db $CD,$1F,$DA,180  ; A3 whisper
    .db $FF

;==============================================================
; SAD MUSIC - D minor, slow and mournful (game over)
;==============================================================
SadMelodyData:
    ; Phrase 1 - mournful descent
    .db $8D,$17,$94,50   ; D4
    .db $8C,$1A,$94,50   ; C4
    .db $80,$1E,$94,60   ; Bb3 (the sad flat 6th)
    .db $80,$00,$9F,20   ; rest
    ; Phrase 2 - trying to rise, falling back
    .db $8C,$1A,$94,40   ; C4
    .db $8D,$17,$94,30   ; D4
    .db $8C,$1A,$94,40   ; C4
    .db $80,$1E,$94,60   ; Bb3
    .db $80,$00,$9F,20   ; rest
    ; Phrase 3 - deeper descent
    .db $8D,$1F,$94,50   ; A3
    .db $80,$1E,$94,50   ; Bb3
    .db $8D,$1F,$94,60   ; A3
    .db $80,$00,$9F,20   ; rest
    ; Phrase 4 - final resolution to low D
    .db $8B,$23,$94,40   ; G3
    .db $81,$28,$94,40   ; F3
    .db $87,$2A,$94,40   ; E3
    .db $8A,$2F,$94,60   ; D3
    .db $80,$00,$9F,40   ; long rest
    .db $FF

SadBassData:
    .db $AA,$2F,$B5,120  ; D3 - root
    .db $A1,$28,$B5,120  ; F3 - minor 3rd
    .db $AA,$2F,$B5,120  ; D3
    .db $AD,$1F,$B5,120  ; A3 - 5th
    .db $FF

SadHarmonyData:
    .db $C1,$28,$D8,240  ; F3 - minor 3rd pad, very quiet
    .db $CA,$2F,$D8,240  ; D3 - root pad
    .db $FF

;==============================================================
; WIN MUSIC - D major, fast power metal (victory!)
;==============================================================
; F#3=606=$025E  F#4=303=$012F  C#5=202=$00CA  D5=190=$00BE
; B4=227=$00E3
WinMelodyData:
    ; Phrase 1 - triumphant arpeggio ascending
    .db $8D,$17,$91,10   ; D4
    .db $8F,$12,$91,10   ; F#4
    .db $8E,$0F,$91,10   ; A4
    .db $8E,$0B,$91,20   ; D5 - hold at top!
    .db $80,$00,$9F,5    ; rest
    ; Phrase 2 - arpeggio ending on B
    .db $8D,$17,$91,10   ; D4
    .db $8F,$12,$91,10   ; F#4
    .db $8E,$0F,$91,10   ; A4
    .db $83,$0E,$91,20   ; B4
    .db $80,$00,$9F,5    ; rest
    ; Phrase 3 - fast descending hero run
    .db $8E,$0B,$91,8    ; D5
    .db $8A,$0C,$91,8    ; C#5
    .db $83,$0E,$91,8    ; B4
    .db $8E,$0F,$91,8    ; A4
    .db $8E,$11,$91,8    ; G4
    .db $8F,$12,$91,8    ; F#4
    .db $83,$15,$91,8    ; E4
    .db $8D,$17,$91,15   ; D4 (land)
    .db $80,$00,$9F,5    ; rest
    ; Phrase 4 - gallop rhythm
    .db $8D,$17,$91,6    ; D4
    .db $8D,$17,$91,6    ; D4
    .db $8E,$0F,$91,12   ; A4
    .db $8D,$17,$91,6    ; D4
    .db $8D,$17,$91,6    ; D4
    .db $83,$0E,$91,12   ; B4
    .db $8D,$17,$91,6    ; D4
    .db $8D,$17,$91,6    ; D4
    .db $8E,$0B,$91,20   ; D5 - climax!
    .db $80,$00,$9F,10   ; rest
    .db $FF

WinBassData:
    ; Galloping D bass
    .db $AA,$2F,$B2,8    ; D3
    .db $AA,$2F,$B2,8    ; D3
    .db $AD,$1F,$B2,12   ; A3
    .db $AA,$2F,$B2,8    ; D3
    .db $AA,$2F,$B2,8    ; D3
    .db $AD,$1F,$B2,12   ; A3
    ; G bass
    .db $AB,$23,$B2,8    ; G3
    .db $AB,$23,$B2,8    ; G3
    .db $AA,$2F,$B2,12   ; D3
    .db $AB,$23,$B2,8    ; G3
    .db $AB,$23,$B2,8    ; G3
    .db $AA,$2F,$B2,12   ; D3
    ; A bass
    .db $AD,$1F,$B2,8    ; A3
    .db $AD,$1F,$B2,8    ; A3
    .db $AE,$25,$B2,12   ; F#3
    .db $AD,$1F,$B2,8    ; A3
    .db $AD,$1F,$B2,8    ; A3
    .db $AA,$2F,$B2,12   ; D3
    .db $FF

WinHarmonyData:
    .db $CF,$12,$D4,56   ; F#4 - major 3rd over D
    .db $C3,$0E,$D4,56   ; B4 - 3rd over G
    .db $CA,$0C,$D4,56   ; C#5 - 3rd over A
    .db $CF,$12,$D4,56   ; F#4 - back to D
    .db $FF

;==============================================================
; TITLE MUSIC - C major, bright and welcoming
;==============================================================
; C3=828=$033C  G3=571=$023B  F3=641=$0281  E3=679=$02A7
; C4=428=$01AC  D4=381=$017D  E4=339=$0153  F4=320=$0140
; G4=286=$011E  A4=254=$00FE  B4=227=$00E3  C5=214=$00D6
TitleMelodyData:
    ; Whisper. Low register, barely audible.
    .db $8D,$1F,$99,50   ; A3 (low, almost silent)
    .db $80,$00,$9F,90   ; long silence
    .db $8D,$17,$99,40   ; D4
    .db $80,$00,$9F,120  ; very long silence
    .db $FF

TitleBassData:
    ; Ghost drone
    .db $AA,$2F,$BB,250  ; D3 (whisper)
    .db $FF

TitleHarmonyData:
    .db $C3,$15,$D5,120  ; E4 pad
    .db $CE,$11,$D5,120  ; G4 pad
    .db $FF
