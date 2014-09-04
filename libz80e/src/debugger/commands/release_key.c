#include "debugger/commands.h"
#include "debugger/debugger.h"
#include "ti/hardware/keyboard.h"

int command_release_key(debugger_state_t *state, int argc, char **argv) {
	if (argc != 2) {
		state->print(state, "%s - Depress the specified key code, in hex.\n", argv[0]);
		return 0;
	}

	uint8_t key = parse_expression(state, argv[1]);
	release_key(state->asic->cpu->devices[0x01].device, key);
	return 0;
}
