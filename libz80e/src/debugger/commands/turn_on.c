#include "debugger/commands.h"
#include "debugger/debugger.h"
#include "ti/hardware/interrupts.h"

int command_turn_on(debugger_state_t *state, int argc, char **argv) {
	if (argc != 1) {
		state->print(state, "%s - Interrupt the CPU and raise the 'on button' interrupt\n", argv[0]);
		return 0;
	}

	depress_on_key(state->asic->interrupts);
	char *_argv[] = { "run", "50000" };
	command_run(state, 2, _argv);
	release_on_key(state->asic->interrupts);
	char *__argv[] = { "run" };
	return command_run(state, 1, __argv);
}
