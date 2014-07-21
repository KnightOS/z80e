#ifndef DEBUGGER_HOOKS_H
#define DEBUGGER_HOOKS_H

#include <stdint.h>

#include "asic.h"
#include "cpu.h"

typedef int (*generic_function_pointer)(void *, ...);

typedef struct {
	void *state;
	generic_function_pointer func;
} hook_t;


typedef struct {
	uint16_t capacity;
	uint16_t used;
	hook_t *array;
} hook_array_t;

void init_hooks();
void deinit_hooks();

typedef struct {
	uint16_t memory_location;
	uint8_t byte;
	void *state;
} memory_hook_t;
typedef int (*memory_hook)(ti_mmu_t *, memory_hook_t *);

uint32_t register_hook_memory_read(memory_hook, void *);
void call_memory_read_hooks(ti_mmu_t *, memory_hook_t *);

uint32_t register_hook_memory_write(memory_hook, void *);
void call_memory_write_hooks(ti_mmu_t *, memory_hook_t *);

typedef struct register_hook_struct {
	uint8_t register_id;
	uint16_t contents;
	void *state;
} register_hook_struct_t;

typedef int (*register_hook)(z80cpu_t *, register_hook_struct_t *);

uint32_t register_hook_register_read(register_hook, void *);
void call_register_read_hooks(z80cpu_t *, register_hook_struct_t *);

uint32_t register_hook_register_write(register_hook, void *);
void call_register_write_hooks(z80cpu_t *, register_hook_struct_t *);

#endif
