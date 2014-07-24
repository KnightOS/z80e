#include "tui.h"
#include "debugger.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#ifndef EMSCRIPTEN
#include <readline/readline.h>
#include <readline/history.h>

int print_tui(struct debugger_state *a, const char *b, ...) {
	va_list list;
	va_start(list, b);
	return vprintf(b, list);
}

int vprint_tui(struct debugger_state *a, const char *b, va_list list) {
	return vprintf(b, list);
}
#endif

char **tui_parse_commandline(const char *cmdline, int *argc) {
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

#ifndef EMSCRIPTEN
struct debugger_state tui_new_state(struct debugger_state *state, void *d_state, const char *command_name) {
	debugger_state_t stat = { print_tui, vprint_tui, d_state, state->asic, tui_new_state };
	return stat;
}

void tui_tick(asic_t *asic) {
	while (1) {
                char prompt_buffer[29];
		ti_mmu_bank_state_t *st = &asic->mmu->banks[asic->cpu->registers.PC / 0x1000];
		snprintf(prompt_buffer, 29, "z80e [%c:%02X:0x%04X] %s> ", st->flash ? 'F' : 'R', st->page, asic->cpu->registers.PC, asic->cpu->halted ? "HALT " : "");
		char *result = readline(prompt_buffer);
		if (result) {
			int from_history = 0;

			if (*result == 0) {
				HIST_ENTRY *hist = history_get(where_history());
				if (hist == 0) {
					free(result);
					continue;
				}
				result = (char *)hist->line;
				from_history = 1;
			}
			if (strcmp(result, "exit") == 0) {
				break;
			}

			add_history(result);
			debugger_command_t *command = 0;

			int argc = 0;
			char **cmdline = tui_parse_commandline(result, &argc);

			if (strcmp(cmdline[0], "set") == 0) {
				if (argc != 2) {
					printf("Invalid use of 'set'!\n");
				} else {
					if (strcmp(cmdline[1], "echo") == 0) {
						gDebuggerState.echo = 1;
					} else if (strcmp(cmdline[1], "echo_reg") == 0) {
						gDebuggerState.echo_reg = 1;
					} else {
						printf("Unknown variable '%s'!\n", cmdline[1]);
					}
				}
			} else if (strcmp(cmdline[0], "unset") == 0) {
				if (argc != 2) {
					printf("Invalid use of 'unset'!\n");
				} else {
					if (strcmp(cmdline[1], "echo") == 0) {
						gDebuggerState.echo = 0;
					} else if (strcmp(cmdline[1], "echo_reg") == 0) {
						gDebuggerState.echo_reg = 0;
					} else {
						printf("Unknown variable '%s'!\n", cmdline[1]);
					}
				}
			} else {
				int status = find_best_command(cmdline[0], &command);
				if (status == -1) {
					printf("Error: Ambiguous command %s\n", result);
				} else if (status == 0) {
					printf("Error: Unknown command %s\n", result);
				} else {
					debugger_state_t state = { print_tui, vprint_tui, command->state, asic, tui_new_state };
					int output = command->function(&state, argc, cmdline);
					if (output != 0) {
						printf("The command returned %d\n", output);
					}
				}
			}

			char **tmp = cmdline;
			while (*tmp != 0) {
				free(*tmp);
				tmp++;
			}

			free(cmdline);
			if (!from_history) {
				free(result);
			}
		} else if (result == NULL) {
			break;
		}
	}
}
#endif
