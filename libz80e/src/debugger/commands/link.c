#include "debugger/commands.h"
#include "debugger/debugger.h"
#include "ti/hardware/link.h"

#include <stdlib.h>
#include <string.h>

int command_link(struct debugger_state *state, int argc, char **argv) {
	if (argc < 3) {
		state->print(state, "%s [send|recv] [value|behavior]\n"
				"send a value will send a value to the link port. If you pass a file, it will be sent instead.\n"
				"recv [behavior] defines z80e's behavior when receiving link port data.\n"
				"Use 'print' to print each value, or a file name to write values to that file.\n", argv[0]);
		return 0;
	}

	if (strcasecmp(argv[1], "send") == 0) {
		int strl = 0;
		int i;
		for (i = 1; i < argc - 1; i++) {
			strl += strlen(argv[i + 1]) + 1;
		}

		char *data = malloc(strl);
		char *dpointer = data;
		for (i = 1; i < argc - 1; i++) {
			strcpy(dpointer, argv[i + 1]);
			dpointer += strlen(argv[i + 1]);
			*(dpointer++) = ' ';
		}
		*(dpointer - 1) = 0;

		uint8_t expr = parse_expression(state, data);

		free(data);

		if (!link_recv_byte(state->asic, expr)) {
			state->print(state, "Calculator is not ready to receive another byte.\n");
		} else {
			state->print(state, "Sent %02X to calculator's link assist.\n", expr);
		}
	} else {
		// TODO
	}

	return 0;
}
