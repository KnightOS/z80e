#include "commands.h"
#include "debugger.h"
#include "memory.h"

int command_stack(struct debugger_state *state, int argc, char **argv) {
    if (argc > 2) {
        state->print(state, "%s [count] - print first `count` (or 10) items on the stack\n", argv[0]);
    }

    char *_argv[] = {argv[0], "(SP)", argc > 1 ? argv[1] : "10"};

    return command_hexdump(state, 3, _argv);
}
