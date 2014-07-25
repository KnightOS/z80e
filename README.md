# z80e

A z80 emulator designed for debugging the [KnightOS kernel](https://github.com/KnightSoft/kernel). It emulates Texas Instruments calculators and is not recommended for general-purpose z80 debugging.
It's not done yet.

## Compiling

Native (Linux):

    $ cmake .
    $ make

Browser:

    $ emconfigure cmake .
    $ make

The tests and z80e files will be compiled to bytecode and then to javascript, exporting all external-use methods into `bin/z80e.js` and `bin/tests.js`. These can be used as you would with any other emscripten module, so you can run tests.js in node to run the tests, or add it to an HTML file.
To build on Windows or Mac, read the cmake docs and submit a pull request fixing this sentence once you figure it out.
