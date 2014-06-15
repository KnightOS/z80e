#include "asic.h"
#include "memory.h"
#include "memorymapping.h"

#include <stdlib.h>
#include <string.h>

void reload_mapping(memory_mapping_state_t *state) {
    ti_mmu_bank_state_t *banks = state->asic->mmu->banks;

    banks[0].page = 0;
    banks[0].flash = 1;

    switch (state->map_mode) {
    case 0:
        banks[1].page = state->bank_a_page;
        banks[1].flash = state->bank_a_flash;
        banks[2].page = state->bank_b_page;
        banks[2].flash = state->bank_b_flash;
        if (state->asic->device == TI83p) {
            banks[3].page = 0;
            banks[3].flash = 0;
        } else {
            banks[3].page = state->ram_bank_page;
            banks[3].flash = 0;
        }
        break;
    case 1:
        banks[1].page = state->bank_a_page & 0xFE;
        banks[1].flash = state->bank_a_flash;
        if (state->asic->device == TI83p) {
            banks[2].page = state->bank_a_page;
            banks[2].flash = state->bank_a_flash;
        } else {
            banks[2].page = state->bank_a_page | 1;
            banks[2].flash = state->bank_a_flash;
        }
        banks[3].page = state->bank_b_page;
        banks[3].flash = state->bank_b_flash;
        break;
    }
}

uint8_t read_device_status_port(void *device) {
    memory_mapping_state_t *state = device;

    return 0; // Nothing to read yet
}

void write_device_status_port(void *device, uint8_t data) {
    memory_mapping_state_t *state = device;

    state->map_mode = data & 1;
}

uint8_t read_ram_paging_port(void *device) {
    memory_mapping_state_t *state = device;

    return state->ram_bank_page;
}

void write_ram_paging_port(void *device, uint8_t data) {
    memory_mapping_state_t *state = device;

    state->ram_bank_page = data & 0x7; // 0b111
    reload_mapping(state);
}

uint8_t read_bank_a_paging_port(void *device) {
    memory_mapping_state_t *state = device;

    uint8_t return_value = state->bank_a_page;
    if (state->bank_a_flash) {
        if (state->asic->device == TI83p) {
            return_value &= ~(1 << 6);
            return_value |= 1 << 6;
        } else {
            return_value &= ~(1 << 7);
            return_value |= 1 << 7;
        }
    }

    return return_value;
}

void write_bank_a_paging_port(void *device, uint8_t data) {
    memory_mapping_state_t *state = device;

    int is_flash = 0;

    if (state->asic->device == TI83p) {
        is_flash = data & (1 << 6);
        data &= 0x1F; // 0b11111
    } else {
        is_flash = data & (1 << 7);
        data &= 0x3F; // 0b111111
    }

    state->bank_a_flash = is_flash;
    state->bank_a_page = data;

    reload_mapping(state);
}

uint8_t read_bank_b_paging_port(void *device) {
    memory_mapping_state_t *state = device;

    uint8_t return_value = state->bank_b_page;
    if (state->bank_b_flash) {
        if (state->asic->device == TI83p) {
            return_value &= ~(1 << 6);
            return_value |= 1 << 6;
        } else {
            return_value &= ~(1 << 7);
            return_value |= 1 << 7;
        }
    }

    return return_value;
}

void write_bank_b_paging_port(void *device, uint8_t data) {
    memory_mapping_state_t *state = device;

    int is_flash = 0;

    if (state->asic->device == TI83p) {
        is_flash = data & (1 << 6);
        data &= 0x1F; // 0b11111
    } else {
        is_flash = data & (1 << 7);
        data &= 0x3F; // 0b111111
    }

    state->bank_b_flash = is_flash;
    state->bank_b_page = data;

    reload_mapping(state);
}

void init_mapping_ports(asic_t *asic) {
    memory_mapping_state_t *state = malloc(sizeof(memory_mapping_state_t));

    memset(state, 0, sizeof(memory_mapping_state_t));
    state->asic = asic;

    state->bank_a_flash = 1;
    state->bank_b_flash = 1; // horrible, isn't it?

    z80iodevice_t device_status_port = { state, read_device_status_port, write_device_status_port };
    z80iodevice_t ram_paging_port = { state, read_ram_paging_port, write_ram_paging_port };
    z80iodevice_t bank_a_paging_port = { state, read_bank_a_paging_port, write_bank_a_paging_port };
    z80iodevice_t bank_b_paging_port = { state, read_bank_b_paging_port, write_bank_b_paging_port };

    asic->cpu->devices[0x04] = device_status_port;

    if (asic->device != TI83p) {
        asic->cpu->devices[0x05] = ram_paging_port;
    }

    asic->cpu->devices[0x06] = bank_a_paging_port;
    asic->cpu->devices[0x07] = bank_b_paging_port;

    reload_mapping(state);
}

void free_mapping_ports(asic_t *asic) {
    free(asic->cpu->devices[0x06].device);
}
