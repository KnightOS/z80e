#include "debugger.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

int debugger_list_commands(debugger_state_t *state, int argc, char **argv) {
	if (argc != 1) {
		state->print(state,
			"list_commands - List all registered commands\nThis command takes no arguments.\n");
		return 0;
	}

	int i = 0;
	for (i = 0; i < state->debugger->commands.count; i++) {
		state->print(state, "%d. %s\n", i, state->debugger->commands.commands[i]->name);
	}
	return 0;
}

debugger_command_t list_command = {
	"list_commands", debugger_list_commands
};

debugger_t *init_debugger(asic_t *asic) {
	debugger_t *debugger = calloc(1, sizeof(debugger_t));

	debugger->commands.count = 1;
	debugger->commands.capacity = 10;
	debugger->commands.commands = malloc(sizeof(debugger_command_t *) * 10);
	debugger->commands.commands[0] = &list_command;

	debugger->asic = asic;

	return debugger;
}

void free_debugger(debugger_t *debugger) {
	free(debugger->commands.commands);
	free(debugger);
}

int compare_strings(const char *a, const char *b) {
	int i = 0;
	while (*a != 0 && *b != 0 && *(a++) == *(b++)) {
		i++;
	}
	return i;
}

int find_best_command(debugger_t *debugger, const char *f_command, debugger_command_t ** pointer) {
	int i;
	int max_match = 0;
	int match_numbers = 0;
	int command_length = strlen(f_command);
	int highest_priority = INT_MIN;
	int highest_priority_max = 0;

	debugger_command_t *best_command = 0;

	for (i = 0; i < debugger->commands.count; i++) {
		debugger_command_t *cmd = debugger->commands.commands[i];
		int match = compare_strings(f_command, cmd->name);

		if (command_length > strlen(cmd->name)) {
			continue; // ignore
		} else if (strlen(f_command) != match && match < command_length) {
			continue;
		} else if (match < max_match) {
			continue;
		} else if (match > max_match) {
			max_match = match;
			match_numbers = 0;
			highest_priority = cmd->priority;
			highest_priority_max = 0;
			best_command = cmd;
		} else if (match == max_match) {
			match_numbers++;
			if (cmd->priority > highest_priority) {
				highest_priority = cmd->priority;
				highest_priority_max = 0;
				best_command = cmd;
			} else if (cmd->priority == highest_priority) {
				highest_priority_max++;
			}
		}
	}

	*pointer = best_command;
	if ((max_match && match_numbers == 0) || (max_match && highest_priority_max < 1)) {
		return 1;
	}

	if (max_match == 0) {
		return 0;
	}

	if (match_numbers > 1 || highest_priority_max > 0) {
		return -1;
	}

	return 0;
}

void register_command(debugger_t *debugger, const char *name, debugger_function_t function, void *state, int priority) {
	debugger_list_t *list = &debugger->commands;
	debugger_command_t *command = malloc(sizeof(debugger_command_t));
	command->name = name;
	command->function = function;
	command->state = state;
	command->priority = priority;

	if (list->count >= list->capacity) {
		list->commands = realloc(list->commands, sizeof(debugger_command_t *) * (list->capacity + 10));
		list->capacity += 10;
	}

	list->count++;
	list->commands[list->count - 1] = command;
}

char **debugger_parse_commandline(const char *cmdline, int *argc) {
	char *buffer[10];
	int buffer_pos = 0;
	while (*cmdline != 0 && *cmdline != '\n') {
		int length = 0;
		int is_string = 0;
		const char *tmp = cmdline;
		while ((is_string || *tmp != ' ') && *tmp != '\n' && *tmp) {
			length++;
			if (*tmp == '\\' && is_string) {
				tmp++;
			} else if(*tmp == '"') {
				is_string = !is_string;
				tmp++;
			}

			tmp++;
		}
		is_string = 0;

		buffer[buffer_pos] = malloc(length + 1);

		memset(buffer[buffer_pos], 0, length + 1);

		tmp = cmdline;
		char *tmp2 = buffer[buffer_pos];
		while ((is_string || *tmp != ' ') && *tmp != '\n' && *tmp) {
			if (*tmp == '\\' && is_string) {
				tmp++;
				switch (*tmp) {
				case 't':
					*(tmp2++) = '\t';
					break;
				case 'n':
					*(tmp2++) = '\n';
					break;
				case 'r':
					*(tmp2++) = '\r';
					break;
				default:
					*(tmp2++) = *tmp;
				}
			} else if (*tmp == '"') {
				is_string = !is_string;
			} else {
				*(tmp2++) = *tmp;
			}
			tmp++;
		}

		cmdline = tmp;
		while (*cmdline == ' ') {
			cmdline++;
		}

		buffer_pos++;
	}
	char **result = malloc((buffer_pos + 1) * sizeof(char *));

	int i;
	for (i = 0; i < buffer_pos; i++) {
		result[i] = buffer[i];
	}
	result[buffer_pos] = 0;

	*argc = buffer_pos;
	return result;
}

int debugger_exec(debugger_state_t *state, const char *command_str) {
	debugger_command_t *command;
	int argc;
	char **cmdline = debugger_parse_commandline(command_str, &argc);
	int return_value = 0;

	int status = find_best_command(state->debugger, cmdline[0], &command);
	if (status == -1) {
		state->print(state, "Error: Ambiguous command %s\n", cmdline[0]);
		return_value = -1;
	} else if (status == 0) {
		state->print(state, "Error: Unknown command %s\n", cmdline[0]);
		return_value = -2;
	} else {
		state->state = command->state;
		return_value = command->function(state, argc, cmdline);
	}

	char **tmp = cmdline;
	while (*tmp) {
		free(*(tmp++));
	}
	free(cmdline);

	return return_value;
}
