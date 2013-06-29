#include "cpu.h"
#include <string.h> /* memset */

z80cpu_t z80cpu_init() {
    z80cpu_t cpu;
    memset(&cpu, 0, sizeof(z80cpu_t));
    return cpu;
}
