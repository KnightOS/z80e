#include "asic.h"
#include "memory.h"
#include "cpu.h"
#include <stdint.h>
#include <stdlib.h>

asic_t* asic_init() {
    asic_t* device = malloc(sizeof(asic_t));
    device->cpu = z80cpu_init();
    device->mmu = ti_mmu_create();
    device->cpu->memory = (void*)device->mmu;
    return device;
}

void asic_free(asic_t* device) {
    z80cpu_free(device->cpu);
    mmu_destroy(device->mmu);
    free(device);
}
