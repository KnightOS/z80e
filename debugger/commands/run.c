#include <string.h>

#include "asic.h"
#include "commands.h"
#include "debugger.h"
#include "disassemble.h"
#include "runloop.h"

struct run_disassemble_state {
    struct disassemble_memory memory;
    debugger_state_t *state;
};

uint8_t run_command_read_byte(struct disassemble_memory *state, uint16_t pointer) {
    struct run_disassemble_state *dstate = (struct run_disassemble_state *)state;

    return dstate->state->asic->cpu->read_byte(dstate->state->asic->cpu->memory, pointer);
}

int run_command_write(struct disassemble_memory *state, const char *format, ...) {
    struct run_disassemble_state *dstate = (struct run_disassemble_state *)state;

    va_list list;
    va_start(list, format);

    return dstate->state->vprint(dstate->state, format, list);
}

int command_run(debugger_state_t *state, int argc, char **argv) {
    state->asic->state->stopped = 0;
    uint16_t instructions = -1;

    struct run_disassemble_state dstate;
    dstate.memory.read_byte = run_command_read_byte;
    dstate.memory.current = state->asic->cpu->registers.PC;
    dstate.state = state;

    int oldHalted = 0;

    if ((argc == 2 && strcmp(argv[1], "--help") == 0) || argc > 2) {
        state->print(state, "run [instructions] - run a specified number of instructions\n"
		" If no number is specified, the emulator will run until interrupted (^C).\n");
	return 0;
    } else if(argc == 2) {
        instructions = parse_expression(state, argv[1]);
        state->asic->state->	debugger = DEBUGGER_LONG_OPERATION;
        for (; instructions > 0; instructions--) {
            hook_on_before_execution(state->asic->hook, state->asic->cpu->registers.PC);
            if (instructions != 1 && state->asic->state->stopped) {
                state->asic->state->stopped = 0;
                break;
            }

            if (instructions == 1) {
                state->asic->state->stopped = 0;
            }

            if (state->debugger->flags.echo) {
                if (!state->asic->cpu->halted) {
                    state->print(state, "0x%04X: ", state->asic->cpu->registers.PC);
                    dstate.memory.current = state->asic->cpu->registers.PC;
                    parse_instruction(&(dstate.memory), run_command_write);
                    state->print(state, "\n");
                }
            } else {
                if (state->asic->cpu->halted && !oldHalted) {
                    state->print(state, "CPU is halted\n");
                }
            }

            if (state->debugger->flags.echo_reg) {
                print_state(&state->asic->cpu->registers);
            }

            oldHalted = state->asic->cpu->halted;

            runloop_tick_cycles(state->asic->state->runloop, 1);
            hook_on_after_execution(state->asic->hook, state->asic->cpu->registers.PC);
            if (state->asic->state->stopped) {
                state->asic->state->stopped = 0;
                break;
            }
        }
        state->asic->state->debugger = DEBUGGER_ENABLED;
        return 0;
    }

    state->asic->state->debugger = DEBUGGER_LONG_OPERATION_INTERRUPTABLE;;
    while (1) {
        hook_on_before_execution(state->asic->hook, state->asic->cpu->registers.PC);
        if (state->asic->state->stopped) {
            state->asic->state->stopped = 0;
            break;
        }
        if (state->debugger->flags.echo) {
            if (!state->asic->cpu->halted) {
                state->print(state, "0x%04X: ", state->asic->cpu->registers.PC);
                dstate.memory.current = state->asic->cpu->registers.PC;
                parse_instruction(&(dstate.memory), run_command_write);
                state->print(state, "\n");
            }
        } else {
            if (state->asic->cpu->halted && !oldHalted) {
                state->print(state, "CPU is halted\n");
            }
        }

        if (state->debugger->flags.echo_reg) {
            print_state(&state->asic->cpu->registers);
        }

        oldHalted = state->asic->cpu->halted;

        runloop_tick_cycles(state->asic->state->runloop, 1);

        hook_on_before_execution(state->asic->hook, state->asic->cpu->registers.PC);
        if (state->asic->state->stopped) {
            state->asic->state->debugger = DEBUGGER_ENABLED;
            return 0;
        }
    }
    state->asic->state->debugger = DEBUGGER_ENABLED;
    return 0;
}

int command_step(debugger_state_t *state, int argc, char **argv) {
    char *_argv[] = { "run", "1" };
    return command_run(state, 2, _argv);
}
