#include "ti/memory.h"
#include "core/cpu.h"
#include "ti/ti.h"
#include "log/log.h"

#include "debugger/debugger.h"
#include "disassembler/disassemble.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

ti_mmu_t* ti_mmu_init(ti_device_type device_type, log_t *log) {
	ti_mmu_t *mmu = malloc(sizeof(ti_mmu_t));
	switch (device_type) {
	case TI83p:
	case TI73:
		mmu->settings.ram_pages = 3;
		mmu->settings.flash_pages = 0x20;
		break;
	case TI84p:
		mmu->settings.ram_pages = 8;
		mmu->settings.flash_pages = 0x40;
		break;
	case TI83pSE:
	case TI84pSE:
		mmu->settings.ram_pages = 8;
		mmu->settings.flash_pages = 0x80;
		break;
	case TI84pCSE:
		mmu->settings.ram_pages = 3;
		mmu->settings.flash_pages = 0x100;
		break;
	}
	mmu->ram = malloc(mmu->settings.ram_pages * 0x4000);
	memset(mmu->ram, 0, mmu->settings.ram_pages * 0x4000);
	mmu->flash = malloc(mmu->settings.flash_pages * 0x4000);
	memset(mmu->flash, 0xFF, mmu->settings.flash_pages * 0x4000);
	mmu->flash_unlocked = 0;
	memset(mmu->flash_writes, 0, sizeof(flash_write_t) * 6);
	mmu->flash_write_index = 0;
	mmu->log = log;
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
	free(mmu);
}

void chip_reset(ti_mmu_t *mmu, uint32_t address, uint8_t value) {
	mmu->flash_write_index = 0;
}

void chip_write(ti_mmu_t *mmu, uint32_t address, uint8_t value) {
	mmu->flash[address] &= value;
}

void chip_erase_sector(ti_mmu_t *mmu, uint32_t address, uint8_t value) {
	uint32_t length = 0x10000;
	if ((address >> 16) + 1 == mmu->settings.flash_pages >> 2) {
		switch ((address >> 13) & 7) {
		case 0:
		case 1:
		case 2:
		case 3:
			length = 0x8000;
			break;
		case 4:
		case 5:
			length = 0x2000;
			break;
		case 6:
		case 7:
			length = 0x4000;
			break;
		}
	}
	memset(&mmu->flash[address & ~(length - 1)], 0xFF, length);
}

void chip_erase(ti_mmu_t *mmu, uint32_t address, uint8_t value) {
	memset(mmu->flash, 0xFF, mmu->settings.flash_pages * 0x4000);
	log_message(mmu->log, L_WARN, "mmu", "Erased entire Flash chip - you probably didn't want to do that.");
}

uint8_t ti_read_byte(void *memory, uint16_t address) {
	ti_mmu_t *mmu = memory;
	ti_mmu_bank_state_t bank = mmu->banks[address / 0x4000];
	uint32_t mapped_address = address;
	mapped_address %= 0x4000;
	mapped_address += bank.page * 0x4000;
	uint8_t byte = 0;
	if (bank.flash) {
		chip_reset(mmu, mapped_address, 0);
		byte = mmu->flash[mapped_address];
	} else {
		byte = mmu->ram[mapped_address];
	}
	byte = hook_on_memory_read(mmu->hook, address, byte);
	return byte;
}

struct flash_pattern {
	int length;
	const flash_write_t pattern[6];
	void (*handler)(ti_mmu_t *memory, uint32_t address, uint8_t value);
};

struct flash_pattern patterns[] = {
	{
		.length = 4,
		.pattern = {
			{ .address = 0xAAA, .address_mask = 0xFFF, .value = 0xAA, .value_mask = 0xFF },
			{ .address = 0x555, .address_mask = 0xFFF, .value = 0x55, .value_mask = 0xFF },
			{ .address = 0xAAA, .address_mask = 0xFFF, .value = 0xA0, .value_mask = 0xFF },
			{ .address = 0x000, .address_mask = 0x000, .value = 0x00, .value_mask = 0x00 },
		},
		.handler = chip_write
	},
	{
		.length = 6,
		.pattern = {
			{ .address = 0xAAA, .address_mask = 0xFFF, .value = 0xAA, .value_mask = 0xFF },
			{ .address = 0x555, .address_mask = 0xFFF, .value = 0x55, .value_mask = 0xFF },
			{ .address = 0xAAA, .address_mask = 0xFFF, .value = 0x80, .value_mask = 0xFF },
			{ .address = 0xAAA, .address_mask = 0xFFF, .value = 0xAA, .value_mask = 0xFF },
			{ .address = 0x555, .address_mask = 0xFFF, .value = 0x55, .value_mask = 0xFF },
			{ .address = 0x000, .address_mask = 0x000, .value = 0x00, .value_mask = 0x00 },
		},
		.handler = chip_erase_sector
	},
	{
		.length = 6,
		.pattern = {
			{ .address = 0xAAA, .address_mask = 0xFFF, .value = 0xAA, .value_mask = 0xFF },
			{ .address = 0x555, .address_mask = 0xFFF, .value = 0x55, .value_mask = 0xFF },
			{ .address = 0xAAA, .address_mask = 0xFFF, .value = 0x80, .value_mask = 0xFF },
			{ .address = 0xAAA, .address_mask = 0xFFF, .value = 0xAA, .value_mask = 0xFF },
			{ .address = 0x555, .address_mask = 0xFFF, .value = 0x55, .value_mask = 0xFF },
			{ .address = 0xAAA, .address_mask = 0xFFF, .value = 0x10, .value_mask = 0xFF },
		},
		.handler = chip_erase
	},
	{
		.length = 0
	}
	// TODO: More patterns
};

void ti_write_byte(void *memory, uint16_t address, uint8_t value) {
	ti_mmu_t *mmu = memory;
	ti_mmu_bank_state_t bank = mmu->banks[address / 0x4000];
	uint32_t mapped_address = address;
	mapped_address %= 0x4000;
	mapped_address += bank.page * 0x4000;

	value = hook_on_memory_write(mmu->hook, address, value);

	if (!bank.flash)
		mmu->ram[mapped_address] = value;
	else {
		if (mmu->flash_unlocked) {
			flash_write_t *w = &mmu->flash_writes[mmu->flash_write_index++];
			w->address = address;
			w->value = value;
			int partial_match = 0;
			struct flash_pattern *pattern;
			for (pattern = patterns; pattern->length; pattern++) {
				int i;
				for (i = 0; i < mmu->flash_write_index && i < pattern->length &&
						 (mmu->flash_writes[i].address & pattern->pattern[i].address_mask) == pattern->pattern[i].address &&
						 (mmu->flash_writes[i].value & pattern->pattern[i].value_mask) == pattern->pattern[i].value; i++) {
				}
				if (i == pattern->length) {
					pattern->handler(mmu, mapped_address, value);
					partial_match = 0;
					break;
				} else if (i == mmu->flash_write_index) {
					partial_match = 1;
				}
			}
			if (!partial_match) {
				chip_reset(mmu, mapped_address, value);
			}
		}
	}
}

void mmu_force_write(void *memory, uint16_t address, uint8_t value) {
	ti_mmu_t *mmu = memory;
	ti_mmu_bank_state_t bank = mmu->banks[address / 0x4000];
	uint32_t mapped_address = address;
	mapped_address %= 0x4000;
	mapped_address += bank.page * 0x4000;

	value = hook_on_memory_write(mmu->hook, address, value);

	if (!bank.flash)
		mmu->ram[mapped_address] = value;
	else {
		mmu->flash[mapped_address] = value;
	}
}
