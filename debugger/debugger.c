#include "debugger.h"

#include <stdlib.h>
#include <string.h>
debugger_list_t *gDebuggerList = 0;

int compare_strings(const char *a, const char *b) {
	int i = 0;
	while(*a != 0 && *b != 0 && *(a++) == *(b++))
		i++;
	return i;
}

int find_best_command(const char *f_command, debugger_command_t ** pointer) {
	if (gDebuggerList == 0)
		return 0;

	int i;
	int max_match = 0;
	int match_numbers = 0;
	debugger_command_t *best_command = 0;

	for (i = 0; i < gDebuggerList->count; i++) {
		debugger_command_t *cmd = gDebuggerList->commands[i];
		int match = compare_strings(f_command, cmd->name);
		if (match > max_match) {
			max_match = match;
			match_numbers = 1;
			best_command = cmd;
		} else if (match > 0 && match == max_match)
			match_numbers++;
	}

	*pointer = best_command;
	if (max_match && match_numbers == 1)
		return 1;
	return match_numbers > 1 ? -1 : 0;
}

void register_command(debugger_command_t *command) {
	if (gDebuggerList == 0) {
		gDebuggerList = malloc(sizeof(debugger_list_t));
		gDebuggerList->count = 0;
		gDebuggerList->capacity = 10;
		gDebuggerList->commands = malloc(sizeof(debugger_command_t *) * 10);
	}

	if (gDebuggerList->count >= gDebuggerList->capacity) {
		gDebuggerList->commands = realloc(gDebuggerList->commands, sizeof(debugger_command_t *) * (gDebuggerList->capacity + 10));
		gDebuggerList->capacity += 10;
	}

	gDebuggerList->count++;
	gDebuggerList->commands[gDebuggerList->count - 1] = command;
}
