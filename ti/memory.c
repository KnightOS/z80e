#include "memory.h"
#include "cpu.h"
#include <stdint.h>
#include <stdlib.h>

ti_mmu_t* ti_mmu_init() {
    ti_mmu_t* mmu = malloc(sizeof(ti_mmu_t));
    return mmu;
}

void ti_mmu_free(ti_mmu_t* mmu) {
    free(mmu);
}

uint8_t ti_read_memory(z80cpu_t* cpu, uint16_t address) {
    // TODO
    return 0;
}

void ti_write_memory(z80cpu_t* cpu, uint16_t address, uint8_t value) {
    // TODO
}
