#include "cpu.h"
#include <string.h>
#include <stdlib.h>

z80cpu_t* z80cpu_init() {
    z80cpu_t* cpu = malloc(sizeof(z80cpu_t));
    return cpu;
}

void z80cpu_free(z80cpu_t* cpu) {
    free(cpu);
}

uint8_t z80cpu_read_byte(z80cpu_t* cpu, uint16_t address) {
    return mmu_read_byte(cpu->memory, address);
}

void z80cpu_write_byte(z80cpu_t* cpu, uint16_t address, uint8_t value) {
    mmu_write_byte(cpu->memory, address, value);
}
