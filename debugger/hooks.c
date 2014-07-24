#include <stdlib.h>
#include <stdio.h>

#include "hooks.h"
#include "cpu.h"
#include "memory.h"

typedef struct {
	void *data;
	hook_memory_callback callback;
	uint16_t range_start;
	uint16_t range_end;
} memory_hook_callback_t;

typedef struct {
	int used;
	int capacity;
	memory_hook_callback_t *callbacks;
} hook_memory_array_t;

typedef struct {
	void *data;
	hook_register_callback callback;
	registers flags;
} register_hook_callback_t;

typedef struct {
	int used;
	int capacity;
	register_hook_callback_t *callbacks;
} hook_register_array_t;

typedef struct {
	void *data;
	hook_execution_callback callback;
} execution_hook_callback_t;

typedef struct {
	int used;
	int capacity;
	execution_hook_callback_t *callbacks;
} hook_execution_array_t;

struct hook_info {
	z80cpu_t *cpu;
	ti_mmu_t *mmu;

	hook_memory_array_t *on_memory_read;
	hook_memory_array_t *on_memory_write;

	hook_register_array_t *on_register_read;
	hook_register_array_t *on_register_write;

	hook_execution_array_t *on_before_execution;
	hook_execution_array_t *on_after_execution;
};

hook_info_t *create_hook_set(asic_t *asic) {
	hook_info_t *info = malloc(sizeof(hook_info_t));
	info->cpu = asic->cpu;
	info->mmu = asic->mmu;

	info->cpu->hook = info;
	info->mmu->hook = info;

	info->on_memory_read = malloc(sizeof(hook_memory_array_t));
	info->on_memory_read->used = 0;
	info->on_memory_read->capacity = 10;
	info->on_memory_read->callbacks = malloc(sizeof(memory_hook_callback_t) * 10);

	info->on_memory_write = malloc(sizeof(hook_memory_array_t));
	info->on_memory_write->used = 0;
	info->on_memory_write->capacity = 10;
	info->on_memory_write->callbacks = malloc(sizeof(memory_hook_callback_t) * 10);

	info->on_register_read = malloc(sizeof(hook_register_array_t));
	info->on_register_read->used = 0;
	info->on_register_read->capacity = 10;
	info->on_register_read->callbacks = malloc(sizeof(register_hook_callback_t) * 10);

	info->on_register_write = malloc(sizeof(hook_register_array_t));
	info->on_register_write->used = 0;
	info->on_register_write->capacity = 10;
	info->on_register_write->callbacks = malloc(sizeof(register_hook_callback_t) * 10);

	info->on_before_execution = malloc(sizeof(hook_execution_array_t));
	info->on_before_execution->used = 0;
	info->on_before_execution->capacity = 10;
	info->on_before_execution->callbacks = malloc(sizeof(execution_hook_callback_t) * 10);

	info->on_after_execution = malloc(sizeof(hook_execution_array_t));
	info->on_after_execution->used = 0;
	info->on_after_execution->capacity = 10;
	info->on_after_execution->callbacks = malloc(sizeof(execution_hook_callback_t) * 10);

	return info;
}

uint8_t hook_on_memory_read(hook_info_t *info, uint16_t address, uint8_t value) {
	int i = 0;
	for (i = 0; i < info->on_memory_read->used; i++) {
		memory_hook_callback_t *cb = &info->on_memory_read->callbacks[i];
		if (address >= cb->range_start && address <= cb->range_end) {
			value = cb->callback(cb->data, address, value);
		}
	}
	return value;
}

uint8_t hook_on_memory_write(hook_info_t *info, uint16_t address, uint8_t value) {
	int i = 0;
	for (i = 0; i < info->on_memory_write->used; i++) {
		memory_hook_callback_t *cb = &info->on_memory_write->callbacks[i];
		if (address >= cb->range_start && address <= cb->range_end) {
			value = cb->callback(cb->data, address, value);
		}
	}
	return value;
}

int hook_add_memory_read(hook_info_t *info, uint16_t address_start, uint16_t address_end, void *data, hook_memory_callback callback) {
	if (info->on_memory_read->used == info->on_memory_read->capacity) {
		info->on_memory_read->capacity += 10;
		info->on_memory_read->callbacks = realloc(info->on_memory_read->callbacks, sizeof(memory_hook_callback_t) * info->on_memory_read->capacity);
	}
	info->on_memory_read->used++;
	memory_hook_callback_t *cb = &info->on_memory_read->callbacks[info->on_memory_read->used - 1];
	cb->data = data;
	cb->range_start = address_start;
	cb->range_end = address_end;
	cb->callback = callback;
	return info->on_memory_read->used - 1;
}

int hook_add_memory_write(hook_info_t *info, uint16_t address_start, uint16_t address_end, void *data, hook_memory_callback callback) {
	if (info->on_memory_write->used == info->on_memory_write->capacity) {
		info->on_memory_write->capacity += 10;
		info->on_memory_write->callbacks = realloc(info->on_memory_write->callbacks, sizeof(memory_hook_callback_t) * info->on_memory_write->capacity);
	}
	info->on_memory_write->used++;
	memory_hook_callback_t *cb = &info->on_memory_write->callbacks[info->on_memory_write->used - 1];
	cb->data = data;
	cb->range_start = address_start;
	cb->range_end = address_end;
	cb->callback = callback;
	return info->on_memory_write->used - 1;
}

uint16_t hook_on_register_read(hook_info_t *info, registers flags, uint16_t value) 
{
	int i = 0;
	for (i = 0; i < info->on_register_read->used; i++) {
		register_hook_callback_t *cb = &info->on_register_read->callbacks[i];
		if (cb->flags & flags) {
			value = cb->callback(cb->data, flags, value);
		}
	}
	return value;
}

uint16_t hook_on_register_write(hook_info_t *info, registers flags, uint16_t value) 
{
	int i = 0;
	for (i = 0; i < info->on_register_write->used; i++) {
		register_hook_callback_t *cb = &info->on_register_write->callbacks[i];
		if (cb->flags & flags) {
			value = cb->callback(cb->data, flags, value);
		}
	}
	return value;
}

int hook_add_register_read(hook_info_t *info, registers flags, void *data, hook_register_callback callback) {
	if (info->on_register_read->used == info->on_register_read->capacity) {
		info->on_register_read->capacity += 10;
		info->on_register_read->callbacks = realloc(info->on_register_read->callbacks, sizeof(register_hook_callback_t) * info->on_register_read->capacity);
	}
	info->on_register_read->used++;
	register_hook_callback_t *cb = &info->on_register_read->callbacks[info->on_register_read->used - 1];
	cb->data = data;
	cb->callback = callback;
	cb->flags = flags;
	return info->on_register_read->used - 1;
}

int hook_add_register_write(hook_info_t *info, registers flags, void *data, hook_register_callback callback) {
	if (info->on_register_write->used == info->on_register_write->capacity) {
		info->on_register_write->capacity += 10;
		info->on_register_write->callbacks = realloc(info->on_register_write->callbacks, sizeof(register_hook_callback_t) * info->on_register_write->capacity);
	}
	info->on_register_write->used++;
	register_hook_callback_t *cb = &info->on_register_write->callbacks[info->on_register_write->used - 1];
	cb->data = data;
	cb->callback = callback;
	cb->flags = flags;
	return info->on_register_write->used - 1;
}

void hook_on_before_execution(hook_info_t *info, uint16_t address) {
	int i = 0;
	for (i = 0; i < info->on_before_execution->used; i++) {
		execution_hook_callback_t *cb = &info->on_before_execution->callbacks[i];
		cb->callback(cb->data, address);
	}
}

void hook_on_after_execution(hook_info_t *info, uint16_t address) {
	int i = 0;
	for (i = 0; i < info->on_after_execution->used; i++) {
		execution_hook_callback_t *cb = &info->on_after_execution->callbacks[i];
		cb->callback(cb->data, address);
	}
}

int hook_add_before_execution(hook_info_t *info, void *data, hook_execution_callback callback) {
	if (info->on_before_execution->used == info->on_before_execution->capacity) {
		info->on_before_execution->capacity += 10;
		info->on_before_execution->callbacks = realloc(info->on_before_execution->callbacks, sizeof(execution_hook_callback_t) * info->on_before_execution->capacity);
	}
	info->on_before_execution->used++;
	execution_hook_callback_t *cb = &info->on_before_execution->callbacks[info->on_before_execution->used - 1];
	cb->data = data;
	cb->callback = callback;
	return info->on_before_execution->used - 1;
}

int hook_add_after_execution(hook_info_t *info, void *data, hook_execution_callback callback) {
	if (info->on_after_execution->used == info->on_after_execution->capacity) {
		info->on_after_execution->capacity += 10;
		info->on_after_execution->callbacks = realloc(info->on_after_execution->callbacks, sizeof(execution_hook_callback_t) * info->on_after_execution->capacity);
	}
	info->on_after_execution->used++;
	execution_hook_callback_t *cb = &info->on_after_execution->callbacks[info->on_after_execution->used - 1];
	cb->data = data;
	cb->callback = callback;
	return info->on_after_execution->used - 1;
}

