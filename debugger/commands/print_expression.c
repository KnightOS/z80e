#include "commands.h"
#include "debugger.h"

int command_print_expression(struct debugger_state *state, int argc, char **argv) {
	if (argc != 2) {
		state->print(state, "%s `expression` - Print an expression\n Don't forget to quote your expression!\n", argv[0]);
		return 0;
	}

	uint16_t expr = parse_expression(state, argv[1]);

	state->print(state, "The result of the expression is: 0x%04X (%u)\n", expr, expr);

	return 0;
}
