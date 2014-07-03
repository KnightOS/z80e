# z80e

A z80 emulator designed for debugging the [KnightOS kernel](https://github.com/KnightSoft/kernel). It emulates Texas Instruments calculators and is not recommended for general-purpose z80 debugging.
It's not done yet.

## Compiling

Native (Linux):

    $ cmake .
    $ make

Browser:

    $ cmake -DCMAKE_TOOLCHAIN_FILE=/usr/lib/emscripten/cmake/Platform/Emscripten.cmake \
        -DCMAKE_BUILD_TYPE=Debug \
        -DEMSCRIPTEN \
        -G "Unix Makefiles" \
        .
    $ make

This assumes emscripten is installed in `/usr`. To build on Windows or Mac, read the cmake docs and
submit a pull request fixing this sentence once you figure it out.
