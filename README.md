# SMS Hello World

A Sega Master System ROM written in Z80 assembly.

## Prerequisites

Mac:
Use homebrew to install the tools:

- [WLA-DX](https://github.com/vhelin/wla-dx) (wla-z80 and wlalink)
- [Mednafen](https://mednafen.github.io/) emulator

## Build & Run

```bash
make        # Build the ROM
make run    # Build and run in Mednafen
make clean  # Remove build artifacts
```

Output: `build/game.sms`
