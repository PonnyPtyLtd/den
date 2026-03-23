;==============================================================
; Dungeon generation
;==============================================================

GenerateDungeon:
    call FillMapWalls
    call PlaceRooms
    call ConnectRooms
    call PlaceStairs
    call PlacePlayer
    call PlaceGreebles
    ret

;--------------------------------------------------------------
; FillMapWalls - fill MapBuffer with '#'
;--------------------------------------------------------------
FillMapWalls:
    ld hl,MapBuffer
    ld bc,MAP_SIZE
_fmw_loop:
    ld (hl),'#'
    inc hl
    dec bc
    ld a,b
    or c
    jr nz,_fmw_loop
    ret

;--------------------------------------------------------------
; PlaceRooms - generate 3-5 random rooms
;--------------------------------------------------------------
PlaceRooms:
    ld a,3
    call GetRandomRange
    add a,3                 ; 3-5 rooms
    ld (NumRooms),a
    ld ix,RoomData
    ld c,a
_pr_loop:
    push bc
    ; Width: 2 + rand(0..3)
    ld a,4
    call GetRandomRange
    add a,2
    ld (ix+2),a
    ; Height: 2 + rand(0..2)
    ld a,3
    call GetRandomRange
    add a,2
    ld (ix+3),a
    ; X: 1 + rand(0 .. MAP_WIDTH-2-w)
    ld a,MAP_WIDTH-1
    sub (ix+2)
    call GetRandomRange
    inc a
    ld (ix+0),a
    ; Y: 1 + rand(0 .. MAP_HEIGHT-2-h)
    ld a,MAP_HEIGHT-1
    sub (ix+3)
    call GetRandomRange
    inc a
    ld (ix+1),a
    ; Carve room
    call CarveRoom
    ld bc,ROOM_DATA_SIZE
    add ix,bc
    pop bc
    dec c
    jr nz,_pr_loop
    ret

;--------------------------------------------------------------
; CarveRoom - fill room rectangle with '.'
; Input: IX = room data [x, y, w, h]
;--------------------------------------------------------------
CarveRoom:
    ld d,(ix+3)             ; rows remaining
    ld e,(ix+1)             ; current Y
_cr_row:
    push de
    ld b,(ix+0)             ; start X
    ld c,e                  ; current Y
    call GetMapAddress      ; HL = row start
    ld b,(ix+2)             ; width
_cr_col:
    ld (hl),'.'
    inc hl
    djnz _cr_col
    pop de
    inc e
    dec d
    jr nz,_cr_row
    ret

;--------------------------------------------------------------
; ConnectRooms - L-shaped corridors between consecutive rooms
;--------------------------------------------------------------
ConnectRooms:
    ld a,(NumRooms)
    dec a
    ret z
    ld b,a
    ld iy,RoomData
_cc_loop:
    push bc
    ; Center of current room
    ld a,(iy+0)
    ld c,(iy+2)
    srl c
    add a,c
    ld d,a                  ; D = center X room 1
    ld a,(iy+1)
    ld c,(iy+3)
    srl c
    add a,c
    ld e,a                  ; E = center Y room 1
    ; Center of next room
    ld a,(iy+4)
    ld c,(iy+6)
    srl c
    add a,c
    ld h,a                  ; H = center X room 2
    ld a,(iy+5)
    ld c,(iy+7)
    srl c
    add a,c
    ld l,a                  ; L = center Y room 2
    ; Horizontal corridor at Y=E from X=D to X=H
    push hl
    push de
    call DrawHCorridor
    pop de
    pop hl
    ; Vertical corridor at X=H from Y=E to Y=L
    call DrawVCorridor
    ; Next room pair
    push de
    ld de,ROOM_DATA_SIZE
    add iy,de
    pop de
    pop bc
    djnz _cc_loop
    ret

;--------------------------------------------------------------
; DrawHCorridor - horizontal line of '.'
; Input: D=x1, H=x2, E=y
;--------------------------------------------------------------
DrawHCorridor:
    ld a,d
    cp h
    jr c,_dhc_ordered
    jr z,_dhc_done
    ld b,d
    ld d,h
    ld h,b
_dhc_ordered:
    ld a,h
    sub d
    inc a
    push af                 ; save length
    ld b,d
    ld c,e
    call GetMapAddress
    pop af
    ld b,a
_dhc_fill:
    ld (hl),'.'
    inc hl
    djnz _dhc_fill
_dhc_done:
    ret

;--------------------------------------------------------------
; DrawVCorridor - vertical line of '.'
; Input: H=x, E=y1, L=y2
;--------------------------------------------------------------
DrawVCorridor:
    ld a,e
    cp l
    jr c,_dvc_ordered
    jr z,_dvc_done
    ld b,e
    ld e,l
    ld l,b
_dvc_ordered:
    ld a,l
    sub e
    inc a
    ld d,a                  ; count
_dvc_fill:
    push de
    push hl
    ld b,h
    ld c,e
    call GetMapAddress
    ld (hl),'.'
    pop hl
    pop de
    inc e
    dec d
    jr nz,_dvc_fill
_dvc_done:
    ret

;--------------------------------------------------------------
; PlaceStairs - '>' in center of last room
;--------------------------------------------------------------
PlaceStairs:
    ld a,(NumRooms)
    dec a
    add a,a
    add a,a
    ld c,a
    ld b,0
    ld ix,RoomData
    add ix,bc
    ld a,(ix+0)
    ld b,(ix+2)
    srl b
    add a,b
    ld b,a
    ld a,(ix+1)
    ld c,(ix+3)
    srl c
    add a,c
    ld c,a
    call GetMapAddress
    ld (hl),'>'
    ret

;--------------------------------------------------------------
; PlacePlayer - in center of first room
;--------------------------------------------------------------
PlacePlayer:
    ld ix,RoomData
    ld a,(ix+0)
    ld b,(ix+2)
    srl b
    add a,b
    ld (PlayerX),a
    ld a,(ix+1)
    ld c,(ix+3)
    srl c
    add a,c
    ld (PlayerY),a
    ret

;--------------------------------------------------------------
; PlaceGreebles - scatter decorative details on interior walls
; Max 2 of each type, 1/3 chance of 0/1/2 per type
;--------------------------------------------------------------
PlaceGreebles:
    xor a
    ld (NumGreebles),a
    ld c,0                  ; C = type index (0-6)
_pgr_typeLoop:
    ld a,c
    cp 7
    ret z
    ; Look up tile for this type
    push bc
    ld b,0
    ld hl,GreebleTileTable
    add hl,bc
    ld b,(hl)               ; B = greeble tile index
    ; Roll count: 0, 1, or 2
    push bc
    ld a,3
    call GetRandomRange      ; A = 0, 1, or 2
    pop bc                   ; B = tile
    or a
    jr z,_pgr_typeNext
_pgr_countLoop:
    push af                  ; save remaining count
    push bc                  ; save B=tile, C=type
    ld a,b
    call _pgr_tryPlace
    pop bc
    pop af
    dec a
    jr nz,_pgr_countLoop
_pgr_typeNext:
    pop bc
    inc c
    jr _pgr_typeLoop

; Try to place one greeble. A = tile index. 20 retries.
_pgr_tryPlace:
    push af                  ; save tile on stack
    ld e,20
_pgr_retry:
    push de
    ; Random position (avoid borders)
    ld a,MAP_WIDTH-2
    call GetRandomRange
    inc a
    ld d,a                   ; D = X (1..14)
    ld a,MAP_HEIGHT-2
    call GetRandomRange
    inc a
    ld e,a                   ; E = Y (1..9)
    ; Check center is wall
    ld b,d
    ld c,e
    push de
    call GetMapTile
    pop de
    cp '#'
    jr nz,_pgr_miss
    ; Check up
    ld b,d
    ld a,e
    dec a
    ld c,a
    push de
    call GetMapTile
    pop de
    cp '#'
    jr nz,_pgr_miss
    ; Check down
    ld b,d
    ld a,e
    inc a
    ld c,a
    push de
    call GetMapTile
    pop de
    cp '#'
    jr nz,_pgr_miss
    ; Check left
    ld a,d
    dec a
    ld b,a
    ld c,e
    push de
    call GetMapTile
    pop de
    cp '#'
    jr nz,_pgr_miss
    ; Check right
    ld a,d
    inc a
    ld b,a
    ld c,e
    push de
    call GetMapTile
    pop de
    cp '#'
    jr nz,_pgr_miss
    ; Check not already greebled
    ld b,d
    ld c,e
    push de
    call CheckGreeble
    pop de
    or a
    jr nz,_pgr_miss
    ; Success! Store greeble.
    pop bc                   ; discard retry counter
    ld a,(NumGreebles)
    cp 14
    jr nc,_pgr_full
    ; GreebleData + count*3
    ld c,a
    add a,a
    add a,c                  ; A = count * 3
    ld c,a
    ld b,0
    ld hl,GreebleData
    add hl,bc
    ld (hl),d                ; X
    inc hl
    ld (hl),e                ; Y
    inc hl
    pop af                   ; tile index
    ld (hl),a
    ld a,(NumGreebles)
    inc a
    ld (NumGreebles),a
    ret
_pgr_miss:
    pop de
    dec e
    jr nz,_pgr_retry
_pgr_full:
    pop af                   ; clean tile from stack
    ret

; Check if position B,C has a greeble. A=tile if found, 0 if not.
CheckGreeble:
    ld a,(NumGreebles)
    or a
    ret z
    ld d,a
    ld hl,GreebleData
_cg_loop:
    ld a,(hl)
    cp b
    jr nz,_cg_skip
    inc hl
    ld a,(hl)
    cp c
    jr nz,_cg_skip2
    inc hl
    ld a,(hl)               ; found - return tile
    ret
_cg_skip:
    inc hl
_cg_skip2:
    inc hl
    inc hl
    dec d
    jr nz,_cg_loop
    xor a                    ; not found
    ret
