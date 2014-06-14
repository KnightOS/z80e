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
	int capacity;
	int used;
	hook_t *array;
} hook_array_t;

hook_array_t *read_memory_hooks;
hook_array_t *write_memory_hooks;
hook_array_t *read_register_hooks;
hook_array_t *write_register_hooks;

void init_hooks();
void deinit_hooks();

typedef struct {
	void *state;
	uint16_t memory_location;
	uint8_t read_byte;
} read_memory_struct_t;

typedef int (*read_memory_hook)(ti_mmu_t *, read_memory_struct_t *);
void register_hook_read_memory(read_memory_hook, void *);
void call_read_memory_hooks(ti_mmu_t *, read_memory_struct_t *);

typedef struct {
	void *state;
	uint16_t memory_location;
	uint8_t write_byte;
} write_memory_struct_t;

typedef int (*write_memory_hook)(ti_mmu_t *, write_memory_struct_t *);
void register_hook_write_memory(write_memory_hook, void *);
void call_write_memory_hooks(ti_mmu_t *, write_memory_struct_t *);

typedef struct register_hook_struct {
	void *state;
	uint8_t register_id;
	uint16_t contents;
} register_hook_struct_t;

typedef int (*register_hook)(z80cpu_t *, register_hook_struct_t *);

void register_hook_read_register(register_hook, void *);
void call_read_register_hooks(z80cpu_t *, register_hook_struct_t *);

void register_hook_write_register(register_hook, void *);
void call_write_register_hooks(z80cpu_t *, register_hook_struct_t *);

#endif
