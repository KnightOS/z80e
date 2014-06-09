#ifndef DEBUGGER_H
#define DEBUGGER_H

struct debugger_state;

typedef int (*debugger_function_t)(struct debugger_state *, int, char **);

typedef struct {
	const char *name;
	debugger_function_t function;
} debugger_command_t;

typedef struct {
	int count;
	int capacity;
	debugger_command_t **commands;
} debugger_list_t;

typedef struct debugger_state {
	void (*print)(struct debugger_state *, const char *, ...);
} debugger_state_t;

typedef struct {
	
} debugger_interface_t;

int find_best_command(const char *, debugger_command_t **);
void register_command(debugger_command_t *);

#endif
