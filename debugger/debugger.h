#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <stdarg.h>
#include <stdio.h>

#include "asic.h"

typedef struct debugger_state debugger_state_t;
typedef struct debugger debugger_t;
typedef int (*debugger_function_t)(debugger_state_t *, int, char **);

typedef struct {
	const char *name;
	debugger_function_t function;
	int priority;
	void *state;
} debugger_command_t;

typedef struct {
	int count;
	int capacity;
	debugger_command_t **commands;
} debugger_list_t;

struct debugger_state {
	int (*print)(debugger_state_t *, const char *, ...);
	int (*vprint)(debugger_state_t *, const char *, va_list);
	void *state;
	void *interface_state;
	asic_t *asic;
	debugger_t *debugger;
	struct debugger_state (*create_new_state)(debugger_state_t *, void *, const char *command_name);
};

struct debugger {
	struct {
		int echo : 1;
		int echo_reg : 1;
	} flags;

	debugger_list_t commands;
	asic_t *asic;
};

debugger_t *init_debugger(asic_t *);
void free_debugger(debugger_t *);

int find_best_command(debugger_t *, const char *, debugger_command_t **);
void register_command(debugger_t *, const char *, debugger_function_t, void *, int);

#endif
