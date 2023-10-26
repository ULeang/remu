# remu
a toy RISC-V emulator written in c(RV64IM)

## Features
- RV64IM_zicsr
- M,S,U privilege mode
- plic(supports up to 64 sources and 8 targets)
- uart(simple,incomplete,only basic keyboard IO)
- monitor for debug(hard to use yet)

## Installation

### dependencies
Arch
```
$ sudo pacman -S gcc riscv64-linux-gnu-gcc readline
```

### building
```
$ git clone https://github.com/ULeang/remu
$ cd remu && mkdir -p build
$ make
```
