# z80e

A z80 emulator designed for debugging the [KnightOS kernel](https://github.com/KnightSoft/kernel). It emulates Texas Instruments calculators and is not recommended for general-purpose z80 debugging.
It's not done yet.

A list of keybindings can be found in [KEYBINDINGS.md](KEYBINDINGS.md)

## Why z80e?

Most TI emulators have some of a shared set of problems, which z80e hopes to avoid. z80e is...

**Portable**. z80e has been described as "portable as fuck". The actual emulation code uses maybe a half dozen external functions and it's been known to run on Linux, BSD, OSX, Windows, TI-Nspires, TI-Nspires running Linux, Rockbox, and two hobby operating systems for i686. It also was forked to form the basis of [CEmu](https://github.com/CE-Programming/CEmu).

**Flexible**. You can run z80e with a simple terminal interface, a curses debugger, a gtk application (planned), in a web browser (in-progress), etc

**Powerful**. We've built a powerful GDB-like debugger, a curses-based debugger, a web debugger, and a gtk debugger (planned).

**Clean**. If you know C (z80 assembly would help, too), you can understand and help improve the codebase.

**Permissive**. MIT licensed and easy to distribute, modify, or do whatever with.

## Compiling

First make sure you have [scas](https://github.com/KnightOS/scas) installed with
libscas built (SCAS_LIBRARY=1 if using cmake).

Native (Linux):

    $ cmake .
    $ make

Change `cmake .` to `cmake -Denable-sdl=YES .` to build the SDL (graphical) frontend.

Compiling on Windows with cygwin requires ncurses and GNU readline.

Browser:

    $ emconfigure cmake .
    $ make

The tests and z80e files will be compiled to bytecode and then to javascript, exporting all external-use methods into `bin/z80e.js` and `bin/tests.js`. These can be used as you would with any other emscripten module, so you can run tests.js in node to run the tests, or add it to an HTML file.
To build on Windows or Mac, read the cmake docs and submit a pull request fixing this sentence once you figure it out.

## Help, Bugs, Feedback

If you need help with KnightOS, want to keep up with progress, chat with
developers, or ask any other questions about KnightOS, you can hang out in the
IRC channel: [#knightos on irc.freenode.net](http://webchat.freenode.net/?channels=knightos).
 
To report bugs, please create [a GitHub issue](https://github.com/KnightOS/KnightOS/issues/new) or contact us on IRC.
 
If you'd like to contribute to the project, please see the [contribution guidelines](http://www.knightos.org/contributing).
