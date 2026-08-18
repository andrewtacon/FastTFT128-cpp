# Fast TFT 128 - native C++ MakeCode extension

This is a **C++ PXT extension** for the BBC micro:bit MakeCode target. PXT can
compile C++ library APIs into native micro:bit code; the browser-side simulator
is supplied by `sim.ts`.

## Hardware

Designed for the common 1.8-inch ST7735 module used by the Joy-IT RB-TFT1.8.

| TFT | micro:bit |
|---|---|
| SCK / SCL | P13 |
| SDA / MOSI | P15 |
| DC / RS | P1 |
| CS | P16 |
| RES | 3V |
| VCC | 3V |
| GND | GND |

## Add to MakeCode

The normal distribution mechanism is a GitHub repository. Create a GitHub
repository containing these files, then in the micro:bit MakeCode editor choose
**Settings -> Extensions** and enter the repository URL.

PXT loads the C++ library and generates its JavaScript/Blocks API. The browser
simulator uses `sim.ts` so the blocks can also be tested in the simulator.

C++ extensions require cloud compilation when building the actual micro:bit
program.

## Why this is faster

The original TypeScript extension performs many individual `spiWrite()` calls.
This extension uses the native C++ PXT SPI API and `spiTransfer()` for pixel
buffers. The full 128x128 RGB565 framebuffer is 32768 bytes.

Default SPI speed is 16 MHz. After verifying the display is reliable, use the
`set SPI speed` block to test 20 MHz.

## Memory

The framebuffer requires 32768 bytes of RAM. This is intended primarily for
micro:bit V2. If using micro:bit V1, use direct `fillRect` operations rather
than the framebuffer API.

## Important

The simulator is only a software preview; it does not drive a physical TFT.
The C++ source is the code compiled into the micro:bit program.
