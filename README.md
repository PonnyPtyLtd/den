# Den

A Sega Master System roguelike written in Z80 assembly. Turn-based dungeon crawler with 5 floors, 4 playable races, item drafting, and lunge-based attack animations. Single 32KB ROM.

## Prerequisites

- [WLA-DX](https://github.com/vhelin/wla-dx) (wla-z80 and wlalink)
- [Mednafen](https://mednafen.github.io/) emulator
- Ruby (for tile import/export tools)

On Mac, all available via Homebrew.

## Build & Run

```bash
make        # Build the ROM → build/game.sms
make run    # Build and launch in Mednafen
make clean  # Remove build artifacts
```

## Tile Editing

```bash
make tiles-export  # Export BG and sprite tiles to BMPs
make tiles-import  # Import edited tiles back in
```

Sprites are defined as hex grids in `tools/sprites.txt` and encoded via `tools/sprite_encode.rb`.
