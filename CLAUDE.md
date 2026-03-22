# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Sega Master System roguelike written in Z80 assembly using the WLA-DX assembler. Single 32KB ROM bank. Turn-based dungeon crawler with 5 floors, 4 playable races, item drafting, and lunge-based attack animations.

## Build Commands

```bash
make          # Assemble and link → build/game.sms
make run      # Build then launch in Mednafen emulator
make clean    # Remove build artifacts
make tiles-export  # Export all BG tiles to tiles_export.bmp (Ruby)
make tiles-import  # Import edited tiles from tiles_export.bmp (Ruby)
```

Prerequisites: `wla-z80`, `wlalink` (WLA-DX), `mednafen`, `ruby` (for tile tools).

## Architecture

### Assembly Structure

Single entry point `src/main.asm` includes all other files via `.include`. The game loop is: `halt` → `UpdateAnimation` → `GameTick` → loop. VBlank ISR copies the SAT buffer to VRAM and ticks music/SFX.

### Rendering: Two-Layer System

- **Background layer** (palette 0): 16x11 grid of 16x16 logical tiles, each mapped to 2x2 hardware 8x8 tiles. ASCII chars in MapBuffer (`#`=wall, `.`=floor, `>`=stairs) map to BG tile indices. Items on the ground are stored as ASCII in MapBuffer but rendered as floor BG + sprite overlay.
- **Sprite layer** (palette 1): 8x16 mode, 2 sprites per 16x16 entity. Player, enemies, and ground items all rendered as sprites. Tile bases defined as `SPR_*` constants in header.inc. SAT buffer (`SatBufY`/`SatBufXN`) is built each frame and DMA'd during VBlank.

### Game State Machine

GameState values: 0=title, 1=play, 2=dead, 3=win, 4=raceSelect, 5=draft. Turn phases during play: IDLE(0) → PLAYER_ANIM(1) → ENEMY_ANIM(2), with attack sub-phases PLAYER_ATK_FWD(3) and PLAYER_ATK_BACK(4) for the lunge animation.

### Entity Data

- **Enemies**: `EnemyData` array, 8 slots × 5 bytes each (X, Y, HP, Type, ATK). Type=0 means dead/empty. Enemy AI uses BFS flood-fill pathfinding (`DistMap`/`PathQueue`).
- **Items**: Stored as ASCII characters in MapBuffer. 12 item types mapped via `CharToItemType`/`ItemTypeToChar` lookup tables. Categories: weapons(1-3), shields(4-6), armor(7-9), helmet(10), potion(11), wand(12).
- **Inventory**: 6 RAM bytes: `InvLeftHand`(weapon), `InvRightHand`(shield), `InvBody`(armor), `InvHead`(helmet), `InvPocket1`, `InvPocket2`.

### Item Map Characters

`w`/`x`/`y` = weapon tiers 1-3, `s`/`S`/`T` = shield tiers 1-3, `a`/`A`/`B` = armor tiers 1-3, `h` = helmet, `p` = potion, `f` = wand. These appear in MapBuffer and the conversion tables in player.inc must stay in sync.

## Sprite Art Pipeline

Sprites are defined as 16x16 hex grids in `tools/sprites.txt` (one hex digit per pixel, palette index). `tools/sprite_encode.rb` converts these to SMS 4bpp planar format (TL, BL, TR, BR quadrant order for 8x16 sprite mode). The encoded data lands in `src/data/tiles.inc`.

See `SPRITE_STYLE_GUIDE.md` for pixel art style rules: 3-tone shading with top-left light source, dithered transitions, anti-aliased edges, 14x14 active area within 16x16 grid, chibi proportions for characters.

## Key Conventions

- All RAM variables declared in a single `.ramsection` in header.inc (slot 1, $C000+).
- Constants use `.define`, data tables use `.db`/`.dw`.
- Local labels use `_prefix_name` pattern (e.g., `_pi_up`, `_ae_gotPtr`). Anonymous forward refs use `+`.
- Stat recalculation after equip changes must recompute ATK/DEF from base + equipment bonuses (tables at bottom of player.inc).
- Drop system has guaranteed-drop logic: if `LevelDropCount` is 0 and the last enemy dies, an item always drops.
