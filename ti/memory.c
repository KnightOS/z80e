#include "memory.h"
#include "cpu.h"
#include "ti.h"

#include "debugger.h"
#include "disassemble.h"
#include "hooks.h"

#include <stdarg.h>
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
    uint8_t byte = 0;
    if (bank.flash)
        byte = mmu->flash[mapped_address];
    else
        byte = mmu->ram[mapped_address];
#if defined(ENABLE_HOOKS) && ENABLE_READ_BYTE_HOOK
    read_memory_struct_t hooks = { address, byte };
    call_read_memory_hooks(mmu, &hooks);
    return hooks.read_byte;
#else
    return byte;
#endif
}

void ti_write_byte(void *memory, uint16_t address, uint8_t value) {
    ti_mmu_t *mmu = memory;
    ti_mmu_bank_state_t bank = mmu->banks[address / 0x4000];
    uint32_t mapped_address = address;
    mapped_address %= 0x4000;
    mapped_address += bank.page * 0x4000;

#if defined(ENABLE_HOOKS) && ENABLE_WRITE_BYTE_HOOK
    write_memory_struct_t hooks = { address, value };
    call_write_memory_hooks(mmu, &hooks);
    value = hooks.write_byte;
#endif

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

int mmu_hex_command(struct debugger_state *state, int argc, char **argv) {
    if (argc != 3 && argc != 4) {
        state->print(state, "%s `start` `length` [column] - print an amount of bytes from the memory\n"
		" Prints the bytes starting from `start`, `length` bytes in total.\n"
		" The lines contain the `column` bytes, or 26 by default.\n", argv[0]);
	return 0;
    }

    ti_mmu_t *mmu = (ti_mmu_t *)state->state;

    uint16_t start = strtol(argv[1], NULL, 0);
    uint16_t length = strtol(argv[2], NULL, 0);

    int column = 26;
    if (argc == 4) {
        column = atoi(argv[3]);
    }

    uint16_t i = 0;
    for (i = 0; i < length; i++) {
        state->print(state, "%02X ", ti_read_byte(mmu, start + i));

        if (i % column == 0 && i > 0) {
            state->print(state, "\n");
        }
    }

    if (!(i % column == 0 && i > 0)) {
        state->print(state, "\n");
    }

    return 0;
}

struct mmu_disassemble_memory {
	struct disassemble_memory mem;
	ti_mmu_t *mmu;
	struct debugger_state *state;
};

int dis_print(struct disassemble_memory *s, const char *format, ...) {
    struct mmu_disassemble_memory *m = (struct mmu_disassemble_memory *)s;

    char buffer[50];

    va_list list;
    va_start(list, format);

    vsnprintf(buffer, 50, format, list);

    return m->state->print(m->state, "%s", buffer);
}

uint8_t dis_read(struct disassemble_memory *s, uint16_t pointer) {
    struct mmu_disassemble_memory *m = (struct mmu_disassemble_memory *)s;
    return ti_read_byte(m->mmu, pointer);
}

int mmu_dis_command(struct debugger_state *state, int argc, char **argv) {
    if (argc != 3) {
        state->print(state, "%s `start` `count` - Print the disassembled commands\n"
		" Prints `count` disassembled commands starting in memory from `start`.\n", argv[0]);
        return 0;
    }

    ti_mmu_t *mmu = (ti_mmu_t *)state->state;

    uint16_t start = strtol(argv[1], NULL, 0);
    uint16_t count = strtol(argv[2], NULL, 0);

    uint16_t i = 0;

    struct mmu_disassemble_memory str = { { dis_read, start }, mmu, state };

    for (i = 0; i < count; i++) {
        state->print(state, "0x%04X: ", str.mem.current);
        parse_instruction(&(str.mem), dis_print);
	state->print(state, "\n");
    }

    return 0;
}

void mmu_register(ti_mmu_t *mmu, const char *hex, const char *dis) {
	register_command(hex, mmu_hex_command, mmu);
	register_command(dis, mmu_dis_command, mmu);
}
