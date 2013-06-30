#include "memory.h"
#include "cpu.h"
#include <stdint.h>
#include <stdlib.h>

ti_mmu_t* ti_mmu_init(/*ti_mmu_settings_t settings*/) {
    ti_mmu_t* mmu = malloc(sizeof(ti_mmu_t));
    mmu->ram = malloc(1234);
    mmu->ram[0] = 10;
    mmu->ram[1] = 42;
    mmu->ram[2] = 63;
    return mmu;
}

void ti_mmu_free(ti_mmu_t* mmu) {
    free(mmu->ram);
    free(mmu);
}

uint8_t ti_read_byte(void* memory, uint16_t address) {
    ti_mmu_t* mmu = (ti_mmu_t*)memory;
    return mmu->ram[address];
}

void ti_write_byte(void* memory, uint16_t address, uint8_t value) {
    // TODO
}
