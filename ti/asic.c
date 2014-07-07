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

void asic_null_write(void *ignored, uint8_t value) {

}

void asic_mirror_ports(asic_t *asic) {
    int i;

    switch (asic->device) {
    case TI83p: 
        for (i = 0x08; i < 0x10; i++) {
            asic->cpu->devices[i] = asic->cpu->devices[i & 0x07];
            asic->cpu->devices[i].write_out = asic_null_write;
        }

        for (i = 0x14; i < 0x100; i++) {
            asic->cpu->devices[i] = asic->cpu->devices[i & 0x07];
            asic->cpu->devices[i].write_out = asic_null_write;
        }
        break;

    default:
        for (i = 0x60; i < 0x80; i++) {
            asic->cpu->devices[i] = asic->cpu->devices[i - 0x20];
        }
    }
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
    asic_mirror_ports(device);
    return device;
}

void asic_free(asic_t* device) {
    cpu_free(device->cpu);
    ti_mmu_free(device->mmu);
    free_devices(device);
    free(device);
}
