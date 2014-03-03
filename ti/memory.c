#include "memory.h"
#include "cpu.h"
#include "ti.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

ti_mmu_t* ti_mmu_init(ti_device_type device_type) {
    ti_mmu_t *mmu = malloc(sizeof(ti_mmu_t));
    mmu->settings = malloc(sizeof(ti_mmu_settings_t));
    switch (device_type) {
        case TI83p:
        case TI73:
            mmu->settings->ram_pages = 3;
            mmu->settings->flash_pages = 0x20;
            break;
        case TI84p:
            mmu->settings->ram_pages = 8;
            mmu->settings->flash_pages = 0x40;
            break;
        case TI83pSE:
        case TI84pSE:
            mmu->settings->ram_pages = 8;
            mmu->settings->flash_pages = 0x80;
            break;
        case TI84pCSE:
            mmu->settings->ram_pages = 3;
            mmu->settings->flash_pages = 0x100;
            break;
    }
    mmu->ram = malloc(mmu->settings->ram_pages * 0x4000);
    memset(mmu->ram, 0, mmu->settings->ram_pages * 0x4000);
    mmu->flash = malloc(mmu->settings->flash_pages * 0x4000);
    memset(mmu->flash, 0xFF, mmu->settings->flash_pages * 0x4000);
    mmu->flash_unlocked = 0;
    // Default bank mappings
    mmu->banks[0].page = 0; mmu->banks[0].flash = 1;
    mmu->banks[1].page = 0; mmu->banks[1].flash = 1;
    mmu->banks[2].page = 0; mmu->banks[2].flash = 1;
    mmu->banks[3].page = 0; mmu->banks[3].flash = 0;
    return mmu;
}

void ti_mmu_free(ti_mmu_t *mmu) {
    free(mmu->ram);
    free(mmu->flash);
    free(mmu->settings);
    free(mmu);
}

uint8_t ti_read_byte(void *memory, uint16_t address) {
    ti_mmu_t *mmu = memory;
    ti_mmu_bank_state_t bank = mmu->banks[address / 0x4000];
    uint32_t mapped_address = address;
    mapped_address %= 0x4000;
    mapped_address += bank.page * 0x4000;
    if (bank.flash)
        return mmu->flash[mapped_address];
    else
        return mmu->ram[mapped_address];
}

void ti_write_byte(void *memory, uint16_t address, uint8_t value) {
    ti_mmu_t *mmu = memory;
    ti_mmu_bank_state_t bank = mmu->banks[address / 0x4000];
    uint32_t mapped_address = address;
    mapped_address %= 0x4000;
    mapped_address += bank.page * 0x4000;
    if (!bank.flash)
        mmu->ram[mapped_address] = value;
    else {
        // TODO: Flash write operations
    }
}

void mmu_force_write(void *memory, uint16_t address, uint8_t value) {
    ti_mmu_t *mmu = memory;
    ti_mmu_bank_state_t bank = mmu->banks[address / 0x4000];
    uint32_t mapped_address = address;
    mapped_address %= 0x4000;
    mapped_address += bank.page * 0x4000;
    if (!bank.flash)
        mmu->ram[mapped_address] = value;
    else {
        mmu->flash[mapped_address] = value;
    }
}
