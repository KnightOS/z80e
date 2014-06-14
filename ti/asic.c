#include "asic.h"
#include <stdint.h>
#include <stdlib.h>
#include "memory.h"
#include "memorymapping.h"
#include "cpu.h"
#include "keyboard.h"
#include "status.h"

void plug_devices(asic_t *asic) {
    /* Link port unimplemented */
    asic->cpu->devices[0x01] = init_keyboard();
    asic->cpu->devices[0x02] = init_status(asic);

    init_mapping_ports(asic);
}

void free_devices(asic_t *asic) {
    /* Link port unimplemented */
    free_keyboard(asic->cpu->devices[0x01]);
    free_status(asic->cpu->devices[0x02]);
    free_mapping_ports(asic);
}

asic_t *asic_init(ti_device_type type) {
    asic_t* device = malloc(sizeof(asic_t));
    device->cpu = cpu_init();
    device->mmu = ti_mmu_init(type);
    device->cpu->memory = (void*)device->mmu;
    device->cpu->read_byte = ti_read_byte;
    device->cpu->write_byte = ti_write_byte;
    device->battery = BATTERIES_GOOD;
    device->device = type;
    plug_devices(device);
    return device;
}

void asic_free(asic_t* device) {
    cpu_free(device->cpu);
    ti_mmu_free(device->mmu);
    free_devices(device);
    free(device);
}
