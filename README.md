# Flappy DTEK-V

A bare-metal Flappy Bird clone for the **DTEK-V** board (RISC-V 32IMZICSR).
Runs at a fixed 60 FPS driven by a hardware timer
interrupt, renders to the 320×240 VGA framebuffer, and uses the push button for
input plus the seven-segment displays for the score.

![demo](docs/demo.gif)

## Features

- Bare-metal freestanding C, no libc, custom `crt0` (`boot.S`)
- Fixed 60 FPS via timer-interrupt frame pacing
- Double-buffered VGA rendering (320×240, 8-bit `RRRGGGBB` color)
- Integer-only physics (Q16.16 fixed-point, no FPU required)
- Sprite blitting with transparency and clipping
- Score on the hex displays, high-score tracking, parallax ground scroll

## Hardware

| Peripheral    | Use                          |
| ------------- | ---------------------------- |
| VGA (320×240) | Framebuffer output           |
| Timer         | 60 Hz frame interrupt        |
| Push button   | Flap / start / restart       |
| Seven-segment | Current score and high score |
| LEDs          | Lit while the button is held |

Target core: `rv32imzicsr`, ABI `ilp32` (soft-float / integer-only).

## Layout

```
boot.S            Startup: stack/gp setup, .bss zeroing, isr_handler
linker.lds        Memory map and section placement
Makefile          Build + run
include/          Public interfaces (drivers + game)
drivers/          Hardware access (vga, timer, button, hex, leds)
src/              Game logic (game, render, sprite, lib, main)
assets/           Generated sprite data (image arrays)
```

The codebase keeps a strict split: `drivers/` is the only code that touches
memory-mapped registers, `src/` contains hardware-independent game logic, and
`include/` holds the interfaces between them.

## Prerequisites

**RISC-V cross-compiler** — build from source or install a pre-built package:

```sh
# Arch Linux
sudo pacman -S riscv32-elf-gcc

# From source (all distros)
git clone https://github.com/riscv/riscv-gnu-toolchain
cd riscv-gnu-toolchain
./configure --prefix=/opt/riscv --with-arch=rv32gc --with-abi=ilp32
sudo make -j$(nproc)
export PATH=/opt/riscv/bin:$PATH
```

**DTEK-V board tools** — required to load binaries onto the physical board.
Download `dtekv-tools.zip` from your course resources, then:

```sh
unzip dtekv-tools.zip
cd dtekv-tools && make
```

This produces `dtekv-run`, `dtekv-download`, and `dtekv-upload` in that directory.

## Build

```sh
make build       # produces main.bin (+ main.elf and a disassembly dump)
make clean
```

Override the toolchain prefix if yours differs:

```sh
make build TOOLCHAIN=riscv64-unknown-elf-
```

Key flags: `-nostdlib -fno-builtin -march=rv32imzicsr -mabi=ilp32`.

## Run

**Physical board:**
```sh
make run         # builds main.bin and loads it onto the board via dtekv-run
```

**Online emulator:** upload `main.bin` at [dtekv.fritiof.dev](https://dtekv.fritiof.dev/).

## Controls

| Action  | Input                          |
| ------- | ------------------------------ |
| Flap    | Press the button while playing |
| Start   | Press from the menu            |
| Restart | Press on the game-over screen  |

## Notes

- Physics is fixed-point Q16.16 (`fixed_t` in `include/game.h`); `1.0 px` is
  `FIXED_ONE`. There is no floating point anywhere in the binary, so no
  soft-float library is linked.
- Pipe gap positions use a deterministic xorshift32 RNG seeded from the frame
  counter at game start.

## License

BSD 3-Clause. See [COPYING](COPYING).
