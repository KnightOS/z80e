#include "debugger/commands.h"
#include "debugger/debugger.h"
#include "ti/hardware/timers.h"

#include <wordexp.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int command_timer(struct debugger_state *state, int argc, char **argv) {
	if (argc < 3) {
		state->print(state, "%s [int|set [port] [value]]\n", argv[0]);
		return 0;
	}

	if (strcasecmp(argv[1], "int") == 0) {
		state->asic->interrupts->interrupted.first_crystal = 1;
		state->asic->cpu->interrupt = !state->asic->cpu->interrupt;
	} else {
		uint8_t port = parse_expression_z80e(state, argv[2]);
		uint8_t val = parse_expression_z80e(state, argv[3]);

		uint8_t *timer = state->asic->cpu->devices[port].device;
		timer[(port - 0x30) % 3] = val;
	}

	return 0;
}
