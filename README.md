# z80e

A z80 emulator designed to run unit tests on the [KnightOS kernel](https://github.com/KnightSoft/kernel).
This is very much a work in progress and doesn't work well at the moment. I will likey also extend this to
be a general-purpose developer-oriented z80 emulator for
[various](https://github.com/KnightSoft/KnightOS#supported-devices) Texas Instruments calculators.

## Usage

    z80e [flags...] path/to/kernel.rom

With no flags, this will just spin up the emulator and begin running. The following flags are available:

* **--wait**: Don't start emulating immediately, and instead go directly into the debugger.
* **--tests [numbers...]**: Run the listed tests (in hex, comma delimited). Example: `01,4B,7C`. You may
  also specify `--tests all` to discover and run all unit tests. `--tests list` will find all tests in the
  provided ROM dump and print the test numbers.
* **--breakpoints [addresses...]**: Sets breakpoints at the specified addresses (in hex, comma delimited).
* **--symbols [file]**: Imports a [sass](https://github.com/KnightSoft/sass) symbol file, whose symbols
  may be used in place of addresses when working with the debugger (or --breakpoints).

## Unit Tests

When run with --test, z80e will run one or more unit tests, print the results, and exits with a status code
indicating the number of failing tests. The emulation is slightly altered to allow for testing, by changing
a number of hardware ports for use interacting with the emulator:

* Port 0x15 (usually ASIC version on TI calculators) reads the following value:
  * 0xFF when executing a single test
  * 0xFE when requesting all tests (see below)
* Port 0xFF reads the low word of the test number to execure
* Port 0xFE reads the high word of the test number to execure
* Write to port 0xFD to indicate test status:
  * 0: Test not found
  * 1: Test passed
  * 2: Test failed
  * 3: Test inconclusive
* Write to port 0xFC to write to a log. The value written changes the behavior:
  * 0: Print the ASCII text at (HL) to stdout
  * 1: Print the value of A to stdout
  * 1: Print the value of A to stdout
  * 1: Print the value of A to stdout
  * 1: Print the value of A to stdout

### Requesting all tests

The emulator will ask the emulated system for a list of available unit test numbers before running all
tests.

## Debugger
