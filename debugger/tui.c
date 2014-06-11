#include "tui.h"
#include "debugger.h"

#include <stdio.h>
#include <stdarg.h>
#include <readline/readline.h>
#include <readline/history.h>


int print_tui(struct debugger_state *a, const char *b, ...) {
	va_list list;
	va_start(list, b);
	return vprintf(b, list);
}

void tui_tick(asic_t *asic) {
	while (1) {
		char *result = readline("> ");
		if (result && *result) {
			if (strcmp(result, "exit") == 0) {
				break;
			}

			add_history(result);
			debugger_command_t *command = 0;
	
			int status = find_best_command(result, &command);
			if (status == -1) {
				printf("Error: Ambiguous command %s\n", result);
			} else if (status == 0) {
				printf("Error: Unknown command %s\n", result);
			} else {
				debugger_state_t state = { asic, print_tui };
				char *data[] = {result};
				printf("\nResult: %d", command->function(&state, 1, data));
			}
		}
	}
}
