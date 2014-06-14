typedef struct {
    asic_t *asic;
    int map_mode;

    uint8_t bank_a_page;
    int bank_a_flash;

    uint8_t bank_b_page;
    int bank_b_flash;

    uint8_t ram_bank_page;
} memory_mapping_state_t;

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
        if (state->device == TI83p) {
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

    if (state->device == TI83p) {
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
        if (state->device == TI83p) {
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

    if (state->device == TI83p) {
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

