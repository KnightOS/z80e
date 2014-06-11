#include "tui.h"
#include "debugger.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <readline/readline.h>
#include <readline/history.h>


int print_tui(struct debugger_state *a, const char *b, ...) {
	va_list list;
	va_start(list, b);
	return vprintf(b, list);
}

char **tui_parse_commandline(const char *cmdline, int *argc) {
	char *buffer[10];
	int buffer_pos = 0;
	while (*cmdline != 0 && *cmdline != '\n') {
		int length = 0;
		int is_string = 0;
		const char *tmp = cmdline;
		while (*tmp != ' ' && *tmp != '\n' && *tmp) {
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
		while (*tmp != ' ' && *tmp != '\n' && *tmp) {
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

void tui_tick(asic_t *asic) {
	while (1) {
		char *result = readline(" z80e > ");
		if (result && *result) {
			if (strcmp(result, "exit") == 0) {
				break;
			}

			add_history(result);
			debugger_command_t *command = 0;

			int argc = 0;
			char **cmdline = tui_parse_commandline(result, &argc);

			int status = find_best_command(result, &command);
			if (status == -1) {
				printf("Error: Ambiguous command %s\n", result);
			} else if (status == 0) {
				printf("Error: Unknown command %s\n", result);
			} else {
				debugger_state_t state = { asic, print_tui };
				printf("Result: %d\n", command->function(&state, argc, cmdline));
			}

			char **tmp = cmdline;
			while (*tmp != 0) {
				free(*tmp);
				tmp++;
			}

			free(cmdline);
			free(result);
		} else if (result == NULL) {
			break;
		}
	}
}
