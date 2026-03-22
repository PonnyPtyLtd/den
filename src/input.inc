;==============================================================
; Joypad input with edge detection
;==============================================================
; SMS port $DC: bit=0 when pressed, bit=1 when not
; Bit 0=Up, 1=Down, 2=Left, 3=Right, 4=Btn1, 5=Btn2

ReadJoypad:
    ; Returns A = newly pressed buttons (1=newly pressed)
    in a,($DC)
    cpl                     ; invert: 1=pressed
    and $3F                 ; mask to 6 buttons
    ld b,a                  ; B = current state (1=pressed)
    ld (InputCurrent),a     ; save raw held state
    ld a,(InputPrevious)
    cpl                     ; NOT previous (1 where NOT pressed last frame)
    and b                   ; 1 = pressed now AND not last frame
    ld c,a                  ; C = new presses
    ld a,b
    ld (InputPrevious),a    ; store current for next frame
    ld a,c                  ; return new presses
    ret
