#include "asic.h"
#include "memory.h"
#include "cpu.h"
#include <stdint.h>
#include <stdlib.h>

asic_t* asic_init() {
    asic_t* device = malloc(sizeof(asic_t));
    device->cpu = z80cpu_init();
    device->mmu = ti_mmu_init();
    device->cpu->memory = (void*)device->mmu;
    device->cpu->read_byte = ti_read_byte;
    device->cpu->write_byte = ti_write_byte;
    return device;
}

void asic_free(asic_t* device) {
    z80cpu_free(device->cpu);
    ti_mmu_free(device->mmu);
    free(device);
}
