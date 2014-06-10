#include "hooks.h"

#include <stdlib.h>
#include <stdio.h>

hook_array_t *alloc_hook_array() {
	hook_array_t *data = malloc(sizeof(hook_array_t));
	data->capacity = 10;
	data->used = 0;
	data->array = malloc(sizeof(generic_function_pointer) * 10);
	return data;
}

void hook_array_free(hook_array_t *arr) {
	free(arr->array);
	free(arr);
}

void hook_array_resize(hook_array_t *data, int newcapacity) {
	data->capacity = newcapacity;
	data->array = realloc(data->array, sizeof(generic_function_pointer) * newcapacity);
}

void hook_array_push(hook_array_t *data, generic_function_pointer func)
{
	if (data->used == data->capacity)
		hook_array_resize(data, data->capacity + 10);

	data->array[data->used++] = func;
}

hook_array_t *read_memory_hooks = 0;
hook_array_t *write_memory_hooks = 0;
hook_array_t *read_register_hooks = 0;
hook_array_t *write_register_hooks = 0;

void init_hooks() {
	read_memory_hooks = alloc_hook_array();
	write_memory_hooks = alloc_hook_array();
	read_register_hooks = alloc_hook_array();
	write_register_hooks = alloc_hook_array();
}

void deinit_hooks() {
	hook_array_free(read_memory_hooks);
	hook_array_free(write_memory_hooks);
	hook_array_free(read_register_hooks);
	hook_array_free(write_register_hooks);

	read_memory_hooks = 0;
	write_memory_hooks = 0;
	read_register_hooks = 0;
	write_register_hooks = 0;
}

void register_hook_read_memory(read_memory_hook hook) {
	if (!read_memory_hooks) {
		return;
	}

	hook_array_push(read_memory_hooks, (generic_function_pointer)hook);
}

void call_read_memory_hooks(ti_mmu_t *asic, read_memory_struct_t *truct) {
	if (!read_memory_hooks) {
		return;
	}

	int result = 0;
	int i = 0;
	for (i = 0; i < read_memory_hooks->used && result == 0; i++) {
		result = ((read_memory_hook)read_memory_hooks->array[i])(asic, truct);
	}
}

void register_hook_write_memory(write_memory_hook hook) {
	if (!write_memory_hooks) {
		return;
	}

	hook_array_push(write_memory_hooks, (generic_function_pointer)hook);
}

void call_write_memory_hooks(ti_mmu_t *asic, write_memory_struct_t *truct) {
	if (!write_memory_hooks) {
		return;
	}

	int result = 0;
	int i = 0;
	for (i = 0; i < write_memory_hooks->used && result == 0; i++) {
		result = ((write_memory_hook)write_memory_hooks->array[i])(asic, truct);
	}
}

void register_hook_read_register(register_hook hook) {
	if (!read_register_hooks) {
		return;
	}

	hook_array_push(read_register_hooks, (generic_function_pointer)hook);
}

void call_read_register_hooks(z80cpu_t *cpu, register_hook_struct_t *truct) {
	if (!read_register_hooks) {
		return;
	}

	int result = 0;
	int i = 0;
	for (i = 0; i < read_register_hooks->used && result == 0; i++) {
		result = ((register_hook)read_register_hooks->array[i])(cpu, truct);
	}
}

void register_hook_write_register(register_hook hook) {
	if (!write_register_hooks) {
		return;
	}

	hook_array_push(write_register_hooks, (generic_function_pointer)hook);
}

void call_write_register_hooks(z80cpu_t *cpu, register_hook_struct_t *truct) {
	if (!write_register_hooks) {
		return;
	}

	int result = 0;
	int i = 0;
	for (i = 0; i < write_register_hooks->used && result == 0; i++) {
		result = ((register_hook)write_register_hooks->array[i])(cpu, truct);
	}
}
