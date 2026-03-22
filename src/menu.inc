;==============================================================
; Radial menu (3x3 bg tile grid) and inventory system
;==============================================================

;--------------------------------------------------------------
; DrawRadialMenu - 3x3 grid of 2x2 hw tiles centered on screen
; Center of 32x22 hw map = hw tiles (13,8) to (18,13)
; Top=Inventory, Left=Pickup, Bottom=Wait
;--------------------------------------------------------------
DrawRadialMenu:
    xor a
    ld (MenuCursor),a
    ld a,1
    ld (BorderPriority),a   ; priority = menu renders over sprites
    ; 3x3 grid of 16x16 icons = 6x6 hw tiles + 2 for border = 8x8
    ; Centered: hw tiles (12,7) to (19,14)
    ld b,12
    ld c,7
    ld d,8
    ld e,8
    call DrawRootBorder
    ; Draw icons in the 3x3 grid (each icon = 2x2 hw tiles)
    ; Top center (row 0, col 1) = Inventory bag icon
    call _drm_drawInvIcon
    ; Middle left (row 1, col 0) = Pickup icon
    call _drm_drawPickIcon
    ; Center (row 1, col 1) = Cancel X icon
    call _drm_drawCancelIcon
    ; Bottom center (row 2, col 1) = Wait icon
    call _drm_drawWaitIcon
    ; Description text below the border
    call DrawRadialDesc
    ret

; Draw 2x2 icon at specific grid position using _drb_writeTile
_drm_drawInvIcon:
    ; Top center: hw (15,8) and (16,8), (15,9) and (16,9)
    ld b,15
    ld c,8
    ld a,BG_ICON_INV_TL
    call _drb_writeTile
    ld b,16
    ld c,8
    ld a,BG_ICON_INV_TR
    call _drb_writeTile
    ld b,15
    ld c,9
    ld a,BG_ICON_INV_BL
    call _drb_writeTile
    ld b,16
    ld c,9
    ld a,BG_ICON_INV_BR
    call _drb_writeTile
    ret

_drm_drawPickIcon:
    ; Middle left: hw (13,10) and (14,10), (13,11) and (14,11)
    ld b,13
    ld c,10
    ld a,BG_ICON_PICK_TL
    call _drb_writeTile
    ld b,14
    ld c,10
    ld a,BG_ICON_PICK_TR
    call _drb_writeTile
    ld b,13
    ld c,11
    ld a,BG_ICON_PICK_BL
    call _drb_writeTile
    ld b,14
    ld c,11
    ld a,BG_ICON_PICK_BR
    call _drb_writeTile
    ret

_drm_drawCancelIcon:
    ; Center: hw (15,10) and (16,10), (15,11) and (16,11)
    ld b,15
    ld c,10
    ld a,BG_ICON_X_TL
    call _drb_writeTile
    ld b,16
    ld c,10
    ld a,BG_ICON_X_TR
    call _drb_writeTile
    ld b,15
    ld c,11
    ld a,BG_ICON_X_BL
    call _drb_writeTile
    ld b,16
    ld c,11
    ld a,BG_ICON_X_BR
    call _drb_writeTile
    ret

_drm_drawWaitIcon:
    ; Bottom center: hw (15,12) and (16,12), (15,13) and (16,13)
    ld b,15
    ld c,12
    ld a,BG_ICON_WAIT_TL
    call _drb_writeTile
    ld b,16
    ld c,12
    ld a,BG_ICON_WAIT_TR
    call _drb_writeTile
    ld b,15
    ld c,13
    ld a,BG_ICON_WAIT_BL
    call _drb_writeTile
    ld b,16
    ld c,13
    ld a,BG_ICON_WAIT_BR
    call _drb_writeTile
    ret

DrawRadialDesc:
    ; Clear description area below border (row 16)
    ld b,10
    ld c,16
    ld hl,_rmBlankDesc
    call PrintString
    ; Show what's selected
    ld a,(MenuCursor)
    or a
    jr z,_rdd_none
    cp 1
    jr z,_rdd_inv
    cp 2
    jr z,_rdd_pick
    cp 3
    jr z,_rdd_wait
_rdd_none:
    ld b,10
    ld c,16
    ld hl,_rmDesc0
    call PrintString
    ret
_rdd_inv:
    ld b,12
    ld c,16
    ld hl,_rmDesc1
    call PrintString
    ret
_rdd_pick:
    ld b,13
    ld c,16
    ld hl,_rmDesc2
    call PrintString
    ret
_rdd_wait:
    ld b,13
    ld c,16
    ld hl,_rmDesc3
    call PrintString
    ret

_rmBlankDesc: .db "                ",0
_rmDesc0: .db "Pick direction",0
_rmDesc1: .db "Inventory",0
_rmDesc2: .db "Pick up",0
_rmDesc3: .db "Skip turn",0

;--------------------------------------------------------------
; RadialMenuTick
; MenuCursor: 0=none, 1=Inventory(up), 2=Pickup(left), 3=Wait(down)
;--------------------------------------------------------------
RadialMenuTick:
    ; Check directions to highlight
    ld a,(InputNew)
    bit 0,a                 ; UP = Inventory
    jr z,+
    ld a,1
    ld (MenuCursor),a
    call PlayMenuSfx
    call DrawRadialDesc
+:
    ld a,(InputNew)
    bit 2,a                 ; LEFT = Pickup
    jr z,+
    ld a,2
    ld (MenuCursor),a
    call PlayMenuSfx
    call DrawRadialDesc
+:
    ld a,(InputNew)
    bit 1,a                 ; DOWN = Wait
    jr z,+
    ld a,3
    ld (MenuCursor),a
    call PlayMenuSfx
    call DrawRadialDesc
+:
    ; B1 confirms, B2 also confirms or closes
    ld a,(InputNew)
    bit 4,a                 ; B1
    jr nz,_rmt_confirm
    bit 5,a                 ; B2 again = close
    jr nz,_rmt_close
    ret

_rmt_confirm:
    ld a,(MenuCursor)
    cp 1
    jr z,_rmt_doInv
    cp 2
    jr z,_rmt_doPickup
    cp 3
    jr z,_rmt_doWait
    ; No selection - close
    jr _rmt_close

_rmt_doInv:
    ld a,2
    ld (MenuState),a
    xor a
    ld (MenuCursor),a
    ld (InvSubMenu),a
    call DrawInventory
    ret

_rmt_doPickup:
    ; Try to pick up item at player position
    call CloseMenu
    ld a,(PlayerX)
    ld b,a
    ld a,(PlayerY)
    ld c,a
    push bc
    call GetMapTile
    pop bc
    call IsItemTile
    ret nz                  ; no item here
    push bc
    call CharToItemType
    call PickupItem
    pop bc
    ret c                   ; pockets full
    ld a,'.'
    call SetMapTile
    ; Redraw that map tile and rebuild sprites
    call DisplayOff
    call RenderFullMap
    call DisplayOn
    call BuildSpriteTable
    ret

_rmt_doWait:
    call CloseMenu
    call SaveEnemyOldPositions
    call UpdateEnemies
    call StartEnemyAnimation
    ret

_rmt_close:
    call CloseMenu
    ret

;--------------------------------------------------------------
; CloseMenu
;--------------------------------------------------------------
CloseMenu:
    xor a
    ld (MenuState),a
    ld (InvSubMenu),a
    call DisplayOff
    call ClearNameTable
    call RenderFullMap
    call RenderHUD
    call DisplayOn
    ret

;--------------------------------------------------------------
; DrawInventory - full screen, 5 slots
;--------------------------------------------------------------
DrawInventory:
    call DisplayOff
    call ClearNameTable
    xor a
    ld (BorderPriority),a   ; no priority for full-screen menus
    ; Draw root border frame around inventory area
    ld b,1
    ld c,0
    ld d,30
    ld e,18
    call DrawRootBorder
    ; Start fresh sprites
    xor a
    ld (NumSprites),a
    ; Layout: sprite icons on left (pixel X=0), labels at col 4, names at col 13
    ; Stats on right side. Character sprite + race at bottom.
    ; Row 1: Weapon
    ld b,4
    ld c,1
    ld hl,LeftHandString
    call PrintString
    ld a,(InvLeftHand)
    ld b,13
    ld c,1
    call PrintItemName
    ld a,(InvLeftHand)
    ld e,8                  ; pixel Y for row 1
    call _invShowItemSprite
    ; Row 3: Shield
    ld b,4
    ld c,3
    ld hl,RightHandString
    call PrintString
    ld a,(InvRightHand)
    ld b,13
    ld c,3
    call PrintItemName
    ld a,(InvRightHand)
    ld e,24
    call _invShowItemSprite
    ; Row 5: Armor
    ld b,4
    ld c,5
    ld hl,BodyString
    call PrintString
    ld a,(InvBody)
    ld b,13
    ld c,5
    call PrintItemName
    ld a,(InvBody)
    ld e,40
    call _invShowItemSprite
    ; Row 7: Head
    ld b,4
    ld c,7
    ld hl,HeadString
    call PrintString
    ld a,(InvHead)
    ld b,13
    ld c,7
    call PrintItemName
    ld a,(InvHead)
    ld e,56
    call _invShowItemSprite
    ; Row 9: Pocket 1
    ld b,4
    ld c,9
    ld hl,Pocket1String
    call PrintString
    ld a,(InvPocket1)
    ld b,13
    ld c,9
    call PrintItemName
    ld a,(InvPocket1)
    ld e,72
    call _invShowItemSprite
    ; Row 11: Pocket 2
    ld b,4
    ld c,11
    ld hl,Pocket2String
    call PrintString
    ld a,(InvPocket2)
    ld b,13
    ld c,11
    call PrintItemName
    ld a,(InvPocket2)
    ld e,88
    call _invShowItemSprite
    ; Stats on right
    ld b,25
    ld c,1
    ld hl,_invAtkLabel
    call PrintString
    ld b,25
    ld c,3
    ld hl,_invDefLabel
    call PrintString
    ld b,25
    ld c,5
    ld hl,_invHPLabel
    call PrintString
    ; Write stat values
    di
    ; ATK value at hw (29,1)
    ld a,$00 + (1*64) + (29*2)
    ld l,a
    ld a,$38 + (1*64 + 29*2) / 256
    ; Actually, let me just use PrintString position for simplicity
    ei
    ; Use a simpler approach: print "XX" strings
    ld a,(PlayerAtk)
    ld b,29
    ld c,1
    call _invPrintNum
    ld a,(PlayerDef)
    ld b,29
    ld c,3
    call _invPrintNum
    ld a,(PlayerMaxHP)
    ld b,29
    ld c,5
    call _invPrintNum
    ; Character sprite at bottom right
    ld d,208
    ld e,112
    ld c,SPR_PLAYER1
    call AddSprite16Simple
    ; Race name next to sprite
    ld a,(PlayerRace)
    add a,a
    ld c,a
    ld b,0
    ld hl,RaceNameTable
    add hl,bc
    ld a,(hl)
    inc hl
    ld h,(hl)
    ld l,a
    ld b,20
    ld c,15
    call PrintString
    ; Draw cursor
    call DrawInvCursor
    call DisplayOn
    ret

; Show item sprite icon at pixel (0, E) if item type A is non-zero
_invShowItemSprite:
    or a
    ret z                   ; empty slot, no sprite
    ; Map item type to sprite base
    ld hl,ItemSprBaseTable
    ld c,a
    ld b,0
    add hl,bc
    ld c,(hl)               ; C = sprite base
    ld d,0                  ; pixel X = 0
    jp AddSprite16Simple    ; tail call

;--------------------------------------------------------------
; AutoEquipItem - auto-equip an item if the right slot is empty
; Input: A = item type just picked up (in a pocket)
; Moves item from pocket to equipment slot if slot empty
;--------------------------------------------------------------
AutoEquipItem:
    or a
    ret z
    ld b,a                  ; B = item type
    ; Determine target slot
    cp 4
    jr c,_aei_weapon        ; 1-3 = weapon -> left hand
    cp 7
    jr c,_aei_shield        ; 4-6 = shield -> right hand
    cp 10
    jr c,_aei_armor         ; 7-9 = armor -> body
    cp ITEM_HELMET
    jr z,_aei_helmet        ; 10 = helmet -> head
    ret                     ; consumables don't auto-equip

_aei_weapon:
    ld a,(InvLeftHand)
    or a
    ret nz                  ; slot occupied
    ld a,b
    ld (InvLeftHand),a
    jr _aei_clearPocket

_aei_shield:
    ld a,(InvRightHand)
    or a
    ret nz
    ld a,b
    ld (InvRightHand),a
    jr _aei_clearPocket

_aei_armor:
    ld a,(InvBody)
    or a
    ret nz
    ld a,b
    ld (InvBody),a
    jr _aei_clearPocket

_aei_helmet:
    ld a,(InvHead)
    or a
    ret nz
    ld a,b
    ld (InvHead),a

_aei_clearPocket:
    ; Find and clear the item from whichever pocket it's in
    ld a,(InvPocket1)
    cp b
    jr nz,+
    xor a
    ld (InvPocket1),a
    jr _aei_recalc
+:
    ld a,(InvPocket2)
    cp b
    jr nz,_aei_recalc
    xor a
    ld (InvPocket2),a
_aei_recalc:
    call RecalcStats
    ret

;--------------------------------------------------------------
; SRAM routines for draft persistence
;--------------------------------------------------------------
EnableSRAM:
    ld a,$08
    ld ($FFFC),a
    ret

DisableSRAM:
    xor a
    ld ($FFFC),a
    ret

; Save draft leftovers to SRAM after a round
; Round 0 leftovers → Slot A (next player's Round 1)
; Round 1 leftovers → Slot B (next player's Round 2)
; Round 2 leftovers → discarded
; Input: A = round just completed (0-2)
SaveDraftLeftovers:
    cp 2
    ret z                   ; Round 2 leftovers discarded
    call EnableSRAM
    push af
    ld a,SRAM_MAGIC_VAL
    ld (SRAM_MAGIC),a
    pop af
    or a
    jr z,_sdl_slotA
    ; Round 1 → Slot B
    ld a,(DraftPoolSize)
    ld (SRAM_SLOTB_SZ),a
    ld b,a
    or a
    jr z,_sdl_done
    ld hl,DraftPool
    ld de,SRAM_SLOTB
    jr _sdl_copy
_sdl_slotA:
    ; Round 0 → Slot A
    ld a,(DraftPoolSize)
    ld (SRAM_SLOTA_SZ),a
    ld b,a
    or a
    jr z,_sdl_done
    ld hl,DraftPool
    ld de,SRAM_SLOTA
_sdl_copy:
    ld a,(hl)
    ld (de),a
    inc hl
    inc de
    djnz _sdl_copy
_sdl_done:
    call DisableSRAM
    ret

; Load draft pool from SRAM
; Round 1 reads Slot A (prev player's Round 0 leftovers)
; Round 2 reads Slot B (prev player's Round 1 leftovers)
; Input: A = round (1 or 2)
; Output: carry set if valid data loaded
LoadDraftFromSRAM:
    push af
    call EnableSRAM
    ld a,(SRAM_MAGIC)
    cp SRAM_MAGIC_VAL
    jr nz,_ldfs_none
    pop af
    cp 1
    jr z,_ldfs_slotA
    ; Round 2 → Slot B
    ld a,(SRAM_SLOTB_SZ)
    or a
    jr z,_ldfs_nodata
    ld (DraftPoolSize),a
    ld b,a
    ld hl,SRAM_SLOTB
    jr _ldfs_load
_ldfs_slotA:
    ; Round 1 → Slot A
    ld a,(SRAM_SLOTA_SZ)
    or a
    jr z,_ldfs_nodata
    ld (DraftPoolSize),a
    ld b,a
    ld hl,SRAM_SLOTA
_ldfs_load:
    ld de,DraftPool
_ldfs_loop:
    ld a,(hl)
    ld (de),a
    inc hl
    inc de
    djnz _ldfs_loop
    call DisableSRAM
    scf
    ret
_ldfs_none:
    pop af
_ldfs_nodata:
    call DisableSRAM
    or a
    ret

;--------------------------------------------------------------
; DrawAimMenu - small directional overlay for wand aiming
;--------------------------------------------------------------
DrawAimMenu:
    ld a,1
    ld (BorderPriority),a
    ld b,12
    ld c,7
    ld d,8
    ld e,8
    call DrawRootBorder
    ; Direction arrows
    ld b,15
    ld c,8
    ld hl,_aimUp
    call PrintString
    ld b,13
    ld c,10
    ld hl,_aimLeft
    call PrintString
    ld b,17
    ld c,10
    ld hl,_aimRight
    call PrintString
    ld b,15
    ld c,12
    ld hl,_aimDown
    call PrintString
    ; Label
    ld b,11
    ld c,14
    ld hl,_aimLabel
    call PrintString
    ret

_aimUp:    .db "^",0
_aimDown:  .db "v",0
_aimLeft:  .db "<",0
_aimRight: .db ">",0
_aimLabel: .db "Fire wand!",0

; Item type -> sprite base lookup
ItemSprBaseTable:
.db 0              ; 0 = empty
.db SPR_WEAPON1    ; 1 = sharp stick
.db SPR_WEAPON2    ; 2 = lacquer blade
.db SPR_WEAPON3    ; 3 = bog oak mace
.db SPR_SHIELD1    ; 4 = wood shield
.db SPR_SHIELD2    ; 5 = shell shield
.db SPR_SHIELD3    ; 6 = oak shield
.db SPR_ARMOR1     ; 7 = reed armor
.db SPR_ARMOR2     ; 8 = shell armor
.db SPR_ARMOR3     ; 9 = oak armor
.db SPR_SHIELD1    ; 10 = helmet
.db SPR_POTION     ; 11 = potion
.db SPR_WAND       ; 12 = wand

_invAtkLabel: .db "ATK:",0
_invDefLabel: .db "DEF:",0
_invHPLabel:  .db "MHP:",0

; Print 2-digit number at position B,C
; A = number (0-99), B = column, C = row
_invPrintNum:
    ; Divide by 10
    ld d,0
_ipn_div:
    cp 10
    jr c,_ipn_done
    sub 10
    inc d
    jr _ipn_div
_ipn_done:
    ld e,a                  ; E = ones digit
    ; Write tens digit (save/restore position around _writeChar8)
    push bc
    push de
    ld a,d
    add a,'0'
    call _writeChar8        ; clobbers B,C
    pop de
    pop bc
    ; Write ones digit at next column
    inc b
    push bc
    ld a,e
    add a,'0'
    call _writeChar8
    pop bc
    ret

PrintItemName:
    push bc
    add a,a
    ld c,a
    ld b,0
    ld hl,ItemNameTable
    add hl,bc
    ld a,(hl)
    inc hl
    ld h,(hl)
    ld l,a
    pop bc
    jp PrintString          ; tail call

;--------------------------------------------------------------
; DrawInvCursor
;--------------------------------------------------------------
DrawInvCursor:
    ; Clear cursors at col 3, rows 1,3,5,7,9,11
    ld b,3
    ld c,1
    ld a,' '
    call _writeChar8
    ld b,3
    ld c,3
    ld a,' '
    call _writeChar8
    ld b,3
    ld c,5
    ld a,' '
    call _writeChar8
    ld b,3
    ld c,7
    ld a,' '
    call _writeChar8
    ld b,3
    ld c,9
    ld a,' '
    call _writeChar8
    ld b,3
    ld c,11
    ld a,' '
    call _writeChar8
    ; Draw cursor at selected row
    ld a,(MenuCursor)
    ld hl,_invCursorRows
    ld c,a
    ld b,0
    add hl,bc
    ld c,(hl)
    ld b,3
    ld a,'>'
    call _writeChar8
    ; Draw sub-menu if active
    ld a,(InvSubMenu)
    or a
    ret z
    ; Show sub-menu options below selected slot
    ld a,(MenuCursor)
    ld hl,_invCursorRows
    ld c,a
    ld b,0
    add hl,bc
    ld c,(hl)
    inc c
    push bc
    ; Only show Equip for non-consumables
    push bc
    call _itsm_minOpt
    pop bc
    cp 2
    jr z,_dic_drop          ; consumable = skip Equip line entirely
    ld a,(InvSubMenu)
    cp 1
    jr nz,+
    ld b,14
    ld hl,EquipString
    call PrintString
    jr _dic_drop
+:
    ld b,14
    ld hl,_dimEquip
    call PrintString
_dic_drop:
    pop bc
    inc c
    push bc
    ld a,(InvSubMenu)
    cp 2
    jr nz,+
    ld b,14
    ld hl,DropString
    call PrintString
    jr _dic_use
+:
    ld b,14
    ld hl,_dimDrop
    call PrintString
_dic_use:
    pop bc
    inc c
    ; Only show Use for consumables
    push bc
    call _itsm_maxOpt
    pop bc
    cp 3
    ret nz                  ; not consumable, don't show Use at all
    ld a,(InvSubMenu)
    cp 3
    jr nz,+
    ld b,14
    ld hl,UseString
    call PrintString
    ret
+:
    ld b,14
    ld hl,_dimUse
    call PrintString
    ret

_dimEquip: .db "  Equip",0
_dimDrop:  .db "  Drop",0
_dimUse:   .db "  Use",0

; Returns min sub-menu option: 2 (Drop) for consumables, 1 (Equip) for equipment
_itsm_minOpt:
    call GetSlotPtr
    ld a,(hl)
    cp ITEM_POTION
    jr z,_imino_2
    cp ITEM_WAND
    jr z,_imino_2
    ld a,1                  ; Equip available
    ret
_imino_2:
    ld a,2                  ; Drop is first option (no Equip)
    ret

; Returns max sub-menu option for current slot: 3 if consumable, 2 otherwise
_itsm_maxOpt:
    call GetSlotPtr
    ld a,(hl)
    cp ITEM_POTION
    jr z,_imo_3
    cp ITEM_WAND
    jr z,_imo_3
    ld a,2                  ; Equip + Drop only
    ret
_imo_3:
    ld a,3                  ; Equip + Drop + Use
    ret

_invCursorRows: .db 1, 3, 5, 7, 9, 11

_writeChar8:
    push af
    ld h,0
    ld l,c
    add hl,hl
    add hl,hl
    add hl,hl
    add hl,hl
    add hl,hl
    ld c,b
    ld b,0
    add hl,bc
    add hl,hl
    ld bc,$3800
    add hl,bc
    di
    ld a,l
    out ($bf),a
    ld a,h
    or $40
    out ($bf),a
    pop af
    sub $20
    out ($be),a
    xor a
    out ($be),a
    ei
    ret

;--------------------------------------------------------------
; InventoryTick
;--------------------------------------------------------------
InventoryTick:
    ld a,(InvSubMenu)
    or a
    jr nz,_it_subMenu

    ; Normal inventory navigation
    ld a,(InputNew)
    or a
    ret z
    ; UP
    bit 0,a
    jr z,_it_noUp
    ld a,(MenuCursor)
    or a
    jr z,_it_noUp
    dec a
    ld (MenuCursor),a
    call PlayMenuSfx
    call DrawInventory
    ret
_it_noUp:
    ld a,(InputNew)
    bit 1,a
    jr z,_it_noDown
    ld a,(MenuCursor)
    cp 5
    jr z,_it_noDown
    inc a
    ld (MenuCursor),a
    call PlayMenuSfx
    call DrawInventory
    ret
_it_noDown:
    ; B1 - open sub-menu for selected slot
    ld a,(InputNew)
    bit 4,a
    jr z,_it_noB1
    ; Check if slot has an item
    call GetSlotPtr
    ld a,(hl)
    or a
    ret z                   ; empty slot, nothing to do
    call _itsm_minOpt       ; start at first valid option
    ld (InvSubMenu),a
    call DrawInventory
    ret
_it_noB1:
    ; B2 - close inventory
    ld a,(InputNew)
    bit 5,a
    ret z
    call CloseMenu
    call BuildSpriteTable
    ret

_it_subMenu:
    ; Sub-menu: UP=equip, DOWN=drop, LEFT=use, B1=confirm, B2=cancel
    ld a,(InputNew)
    bit 0,a                 ; UP = previous option
    jr z,+
    call _itsm_minOpt
    ld b,a                  ; B = min
    ld a,(InvSubMenu)
    dec a
    cp b
    jr nc,_itsm_setOpt
    call _itsm_maxOpt       ; wrap to max
    jr _itsm_setOpt
+:
    ld a,(InputNew)
    bit 1,a                 ; DOWN = next option
    jr z,+
    call _itsm_maxOpt
    ld b,a                  ; B = max
    ld a,(InvSubMenu)
    cp b
    jr nc,_itsm_wrap
    inc a
    jr _itsm_setOpt
_itsm_wrap:
    call _itsm_minOpt       ; wrap to min
_itsm_setOpt:
    ld (InvSubMenu),a
    call PlayMenuSfx
    call DrawInventory
    ret
+:
    ld a,(InputNew)
    bit 4,a                 ; B1 = confirm
    jr z,+
    ld a,(InvSubMenu)
    cp 1
    jr z,_it_tryEquip
    cp 2
    jr z,_it_doDrop
    cp 3
    jr z,_it_doUse
    ret
_it_tryEquip:
    ; Can't equip consumables
    call GetSlotPtr
    ld a,(hl)
    cp ITEM_POTION
    ret z
    cp ITEM_WAND
    ret z
    jr _it_doEquip
+:
    ld a,(InputNew)
    bit 5,a                 ; B2 = cancel
    ret z
    xor a
    ld (InvSubMenu),a
    call DrawInventory
    ret

_it_doEquip:
    call EquipSelected
    xor a
    ld (InvSubMenu),a
    call DrawInventory
    ret

_it_doDrop:
    call DropSelected
    xor a
    ld (InvSubMenu),a
    call DrawInventory
    ret

_it_doUse:
    call UseSelected
    jr nc,_it_useStay       ; check carry BEFORE clearing InvSubMenu
    xor a
    ld (InvSubMenu),a
    call CloseMenu
    call BuildSpriteTable
    ; Check if wand aiming (wand handles its own turn later)
    ld a,(WandAiming)
    or a
    jr z,_it_useNotWand
    ; Show directional aim menu
    call DrawAimMenu
    ret
_it_useNotWand:
    ; Using an item costs a turn - trigger enemy phase
    call SaveEnemyOldPositions
    call BuildDistMap
    call UpdateEnemies
    call StartEnemyAnimation
    ret
_it_useStay:
    call DrawInventory
    ret

;--------------------------------------------------------------
; EquipSelected - move item to appropriate slot
;--------------------------------------------------------------
EquipSelected:
    call GetSlotPtr         ; HL = slot
    ld a,(hl)
    or a
    ret z
    ld b,a                  ; B = item type

    ; Determine destination based on item category
    ld a,(MenuCursor)
    cp 4                    ; pockets start at index 4
    jr c,_es_unequip        ; equip slot = unequip to pocket

    ; From pocket: route to correct equipment slot
    ld a,b
    cp ITEM_HELMET
    jr z,_es_toHead         ; type 10 = helmet
    cp 4
    jr c,_es_toWeapon       ; types 1-3 = weapon
    cp 7
    jr c,_es_toShield       ; types 4-6 = shield
    ; types 7-9 = armor
    ld a,(InvBody)
    ld c,a
    ld a,b
    ld (InvBody),a
    ld (hl),c               ; swap pocket <-> body
    jr _es_recalc

_es_toHead:
    ld a,(InvHead)
    ld c,a
    ld a,b
    ld (InvHead),a
    ld (hl),c
    jr _es_recalc

_es_toWeapon:
    ld a,(InvLeftHand)
    ld c,a
    ld a,b
    ld (InvLeftHand),a
    ld (hl),c
    jr _es_recalc

_es_toShield:
    ld a,(InvRightHand)
    ld c,a
    ld a,b
    ld (InvRightHand),a
    ld (hl),c
    jr _es_recalc

_es_unequip:
    ; Equipment slot -> first empty pocket
    ld a,(InvPocket1)
    or a
    jr nz,+
    ld a,b
    ld (InvPocket1),a
    xor a
    ld (hl),a
    jr _es_recalc
+:
    ld a,(InvPocket2)
    or a
    ret nz                  ; no room
    ld a,b
    ld (InvPocket2),a
    xor a
    ld (hl),a

_es_recalc:
    call RecalcStats
    call PlayMenuSfx
    ret

;--------------------------------------------------------------
; DropSelected - drop item at player feet
;--------------------------------------------------------------
DropSelected:
    call GetSlotPtr
    ld a,(hl)
    or a
    ret z                   ; empty slot
    push hl                 ; save slot ptr
    push af                 ; save item type
    ; Try player tile, then adjacent tiles
    ld a,(PlayerX)
    ld d,a
    ld a,(PlayerY)
    ld e,a
    ; Try center
    ld b,d
    ld c,e
    call _ds_tryTile
    jr z,_ds_placed
    ; Try right
    ld b,d
    inc b
    ld c,e
    call _ds_tryTile
    jr z,_ds_placed
    ; Try left
    ld b,d
    dec b
    ld c,e
    call _ds_tryTile
    jr z,_ds_placed
    ; Try down
    ld b,d
    ld c,e
    inc c
    call _ds_tryTile
    jr z,_ds_placed
    ; Try up
    ld b,d
    ld c,e
    dec c
    call _ds_tryTile
    jr z,_ds_placed
    ; No room anywhere
    pop af
    pop hl
    ret
_ds_placed:
    ; B,C = valid drop position
    pop af                  ; A = item type
    push bc
    call ItemTypeToChar
    pop bc
    call SetMapTile
    pop hl                  ; HL = slot ptr
    xor a
    ld (hl),a
    call RecalcStats
    call PlayMenuSfx
    ret

; Check if tile at B,C is floor. Returns Z if yes.
_ds_tryTile:
    ld a,b
    cp MAP_WIDTH
    jr nc,_ds_tryFail
    ld a,c
    cp MAP_HEIGHT
    jr nc,_ds_tryFail
    push bc
    push de
    call GetMapTile
    pop de
    pop bc
    cp '.'
    ret                     ; Z if floor
_ds_tryFail:
    or 1                    ; clear Z
    ret

;--------------------------------------------------------------
; GetSlotPtr - get RAM address of inventory slot
; Input: MenuCursor (0-4)
; Output: HL
;--------------------------------------------------------------
GetSlotPtr:
    ld a,(MenuCursor)
    or a
    jr z,_gsp_lh
    cp 1
    jr z,_gsp_rh
    cp 2
    jr z,_gsp_body
    cp 3
    jr z,_gsp_head
    cp 4
    jr z,_gsp_p1
    ld hl,InvPocket2
    ret
_gsp_lh:
    ld hl,InvLeftHand
    ret
_gsp_rh:
    ld hl,InvRightHand
    ret
_gsp_body:
    ld hl,InvBody
    ret
_gsp_head:
    ld hl,InvHead
    ret
_gsp_p1:
    ld hl,InvPocket1
    ret

;--------------------------------------------------------------
; PickupItem - item -> first empty pocket
; A = item type, carry set if full
;--------------------------------------------------------------
PickupItem:
    ld b,a
    ld a,(InvPocket1)
    or a
    jr nz,+
    ld a,b
    ld (InvPocket1),a
    or a
    ret
+:
    ld a,(InvPocket2)
    or a
    jr nz,+
    ld a,b
    ld (InvPocket2),a
    or a
    ret
+:
    scf
    ret

;--------------------------------------------------------------
; RecalcStats - ATK from weapon, DEF from shield, MaxHP from armor
;--------------------------------------------------------------
RecalcStats:
    ; ATK
    ld a,(InvLeftHand)
    or a
    jr z,_rs_noWpn
    ld (PlayerWeapon),a
    dec a                   ; 0-index
    ld hl,WeaponBonusTable
    ld c,a
    ld b,0
    add hl,bc
    ld a,(hl)
    add a,4                 ; base ATK
    ld (PlayerAtk),a
    jr _rs_shield
_rs_noWpn:
    ld a,4
    ld (PlayerAtk),a
    xor a
    ld (PlayerWeapon),a
_rs_shield:
    ; DEF
    ld a,(InvRightHand)
    or a
    jr z,_rs_noDef
    cp 4
    jr c,_rs_noDef          ; not a shield
    sub 4                   ; 0-index
    ld hl,ShieldDefTable
    ld c,a
    ld b,0
    add hl,bc
    ld a,(hl)
    ld (PlayerDef),a
    jr _rs_helmet           ; fall through to helmet check, not skip it
_rs_noDef:
    xor a
    ld (PlayerDef),a
_rs_helmet:
    ; Add helmet DEF (+1 if equipped)
    ld a,(InvHead)
    cp ITEM_HELMET
    jr nz,_rs_armor
    ld a,(PlayerDef)
    inc a                   ; helmet adds 1 DEF
    ld (PlayerDef),a
_rs_armor:
    ; Body armor adds to DEF (stacks with shield + helmet)
    ld a,(InvBody)
    or a
    jr z,_rs_noArm
    cp 7
    jr c,_rs_noArm
    ld (PlayerArmor),a
    sub 7                   ; 0-index
    ld hl,ArmorDefTable
    ld c,a
    ld b,0
    add hl,bc
    ld b,a
    ld a,(PlayerDef)
    add a,(hl)              ; add armor DEF to existing DEF
    ld (PlayerDef),a
    jr _rs_done
_rs_noArm:
    xor a
    ld (PlayerArmor),a
_rs_done:
    ret

; Armor DEF table (types 7-9)
ArmorDefTable: .db 1, 3, 5   ; reed +1, shell +3, oak +5

;--------------------------------------------------------------
; UseSelected - use a consumable item from inventory
; Sets carry flag if item was consumed (caller should close menu)
;--------------------------------------------------------------
UseSelected:
    call GetSlotPtr
    ld a,(hl)
    or a
    ret z                   ; empty = no carry (Z clears carry)

    ; Check if it's a consumable
    cp ITEM_POTION
    jr z,_us_potion
    cp ITEM_WAND
    jr z,_us_wand
    ; Not consumable
    or a                    ; clear carry
    ret

_us_potion:
    ; Heal 10 HP (cap at MaxHP)
    push hl
    ld a,(PlayerHP)
    add a,10
    ld b,a
    ld a,(PlayerMaxHP)
    cp b
    jr nc,+
    ld b,a                  ; cap at MaxHP
+:
    ld a,b
    ld (PlayerHP),a
    call PlayMenuSfx
    pop hl
    xor a
    ld (hl),a               ; consume item
    scf                     ; signal: used
    ret

_us_wand:
    ; Enter aiming mode - consume wand, close inventory, wait for direction
    xor a
    ld (hl),a               ; consume wand
    ld a,1
    ld (WandAiming),a       ; flag: next direction input fires the wand
    scf                     ; signal: used (closes inventory)
    ret

;--------------------------------------------------------------
; FindNearestEnemy - find closest alive enemy to player
; Output: IX = enemy data ptr, carry set if none found
;--------------------------------------------------------------
FindNearestEnemy:
    ld ix,EnemyData
    ld b,MAX_ENEMIES
    ld c,$FF                ; best distance so far
    ld de,0                 ; best IX will be saved
_fne_loop:
    push bc
    ld a,(ix+ENEMY_TYPE)
    or a
    jr z,_fne_skip
    ; Calculate Manhattan distance
    ld a,(PlayerX)
    sub (ix+ENEMY_X)
    jr nc,+
    neg
+:
    ld b,a                  ; |dx|
    ld a,(PlayerY)
    sub (ix+ENEMY_Y)
    jr nc,+
    neg
+:
    add a,b                 ; distance = |dx| + |dy|
    cp c
    jr nc,_fne_skip         ; not closer
    ld c,a                  ; new best distance
    push ix
    pop de                  ; DE = best enemy ptr
_fne_skip:
    push de
    ld de,ENEMY_SIZE
    add ix,de
    pop de
    pop bc
    djnz _fne_loop
    ; DE = best enemy, C = distance ($FF if none found)
    ld a,c
    cp $FF
    jr z,_fne_none
    push de
    pop ix                  ; IX = nearest enemy
    or a                    ; clear carry = found
    ret
_fne_none:
    scf                     ; carry = not found
    ret
