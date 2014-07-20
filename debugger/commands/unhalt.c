#include "debugger.h"
#include "cpu.h"
#include "registers.h"

int command_unhalt(struct debugger_state *state, int argc, char **argv) {
        if (argc != 1) {
                state->print(state, "unhalt - Unhalts the running CPU.\n");
                return 0;
        }

	z80cpu_t *cpu = (z80cpu_t *)state->state;
	cpu->halted = 0;
        return 0;
}

