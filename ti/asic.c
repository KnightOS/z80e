#include "asic.h"

uint8_t asic_test_device_in(void *device);

asic_t* asic_init(ti_device_type type) {
    asic_t *device = malloc(sizeof(asic_t));
    device->cpu = cpu_init();
    device->mmu = ti_mmu_init(type);
    device->cpu->memory = (void *)device->mmu;
    device->cpu->read_byte = ti_read_byte;
    device->cpu->write_byte = ti_write_byte;
    return device;
}

void asic_free(asic_t *device) {
    cpu_free(device->cpu);
    ti_mmu_free(device->mmu);
    free(device);
}
