#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <stdarg.h>
#include <stdio.h>

#include "asic.h"

struct debugger_state;

typedef int (*debugger_function_t)(struct debugger_state *, int, char **);

typedef struct {
	int echo;
} global_debugger_state_t;

extern global_debugger_state_t gDebuggerState;

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

typedef struct debugger_state {
	int (*print)(struct debugger_state *, const char *, ...);
	int (*vprint)(struct debugger_state *, const char *, va_list);
	void *state;
	asic_t *asic;
} debugger_state_t;

int find_best_command(const char *, debugger_command_t **);
void register_command(const char *, debugger_function_t, void *, int);

#endif
