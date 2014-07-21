#include "asic.h"
#include "debugger.h"

int command_stop(struct debugger_state *state, int argc, char **argv) {
	state->asic->state->stopped = 1;
	return 0;
}
