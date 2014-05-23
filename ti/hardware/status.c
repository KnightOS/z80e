#include "status.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "asic.h"
#include "cpu.h"

typedef struct {
    asic_t *asic;
    ti_device_type device;
} status_t;

uint8_t read_status(void *_status) {
    status_t *status = (status_t*)_status;
    uint8_t value = 0;
    if (status->asic->battery_remove_check) {
        if (status->asic->battery != BATTERIES_REMOVED) {
            value |= 1;
        }
    } else {
        if (status->asic->battery == BATTERIES_GOOD) {
            value |= 1;
        }
    }
    if (status->asic->mmu->flash_unlocked) {
        value |= 4;
    }
    // Bits 3-6 unimplemented on the TI-83+ and TI-73 (link assist)
    switch (status->device) {
    case TI73:
    case TI83p:
        value |= 128;
        break;
    default:
        value |= 16;
        break;
    }
    return value;
}

void write_status(void *_status, uint8_t value) {
    status_t *status = (status_t*)_status;
    if (status->device == TI83p || status->device == TI73) {
        return;
    }
    // TODO: Acknowledge interrupts
}

z80iodevice_t init_status(asic_t *asic, ti_device_type type) {
    status_t *state = malloc(sizeof(status_t));
    state->asic = asic;
    state->device = type;
    z80iodevice_t device = { state, read_status, write_status };
    return device;
}

void free_status(z80iodevice_t status) {
    // do nothing
}
