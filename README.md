# z80e

A z80 emulator designed for debugging the [KnightOS kernel](https://github.com/KnightSoft/kernel). It emulates Texas Instruments calculators and is not recommended for general-purpose z80 debugging.

## Usage

    z80e [flags...] path/to/kernel.rom

Without any flags, the emulator will immediately begin running the kernel.

The following flags are available:

* **--wait**: Goes directly to the debugger instead of immediately running the kernel.
* **--breakpoints [list...]**: Accepts a comma-delimited list of breakpoints in hexadecimal.
* **--discover**: Finds and lists all unit tests found in the kernel.
* **--tests [list...]**: Accepts a comma-delimited list of test numbers in hexadecimal, or `all`.

## Unit Tests

When running unit tests, z80e modifies emulation slightly.

* Port 0x15 reads 0xFF
* Port 0xFF reads:
  * 0xFF in test discovery mode
  * The high byte of the test number to run
* Port 0xFE reads:
  * 0xFF in test discovery mode
  * The low byte of the test number to run
* Write test results to port 0xFD:
  * 0: Test number not found
  * 1: Passed
  * 2: Failed
  * 3: Inconclusive
* Write debugging information to port 0xFC:
  * 0: Writes ASCII string at (HL) to stdout
  * 1: Writes registers to stdout

z80e will reset the machine for each test. In test discovery mode, write each available test number to port 0xFF/0xFE (high/low), then 0xFFFF when you are done. Each test (and discovery) has a maximum of 100000 cycles to run. Additionally, cycles are unbounded in test mode, and CPU timing may be inaccurate.

## Debugging

When a breakpoint is hit (or the device starts in --wait), z80e enters the debugger. The following commands are available:

* **dasm address**: Print a disassembly, starting from address. If address is omitted, PC - 8 will be used.
* **dump address**: Dumps a small amount of memory starting from address. If address is omitted, HL will be used.
* **listr**: Lists all registers and their current values.
* **breakpoint [add|remove] address**: Adds or removes a breakpoint. If address is omitted, PC will be used.
* **step**: Execute one instruction, then return to the debugger.
* **stepover**: Set a temporary breakpoint on the next instruction, then run to this breakpoint.
* **continue**: Exit the debugger and resume execution.
* **reset**: Reset the CPU.

You may prefix any of these with `*` to run them whenever the debugger is triggered. This may be used, for example, to automatically show a disassembly after each step. Use `clearauto` to get rid of these. You may also add any of these in `~/.z80rc`, one per line.

You may use a register in place of `address`. For example: `dasm HL` to disassemble starting from HL.
