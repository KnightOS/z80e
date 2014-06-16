#include "commands.h"
#include "debugger.h"
#include "hooks.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

int read_hook_printf(struct debugger_state *state, const char *a, ...) {
    va_list list;
    va_start(list, a);

    return vprintf(a, list);
}

typedef struct {
    int argc;
    char **arguments;
    char *expression;
    debugger_command_t *command;
    asic_t *asic;
} on_read_hook_state_t;

int on_read_read_hook(ti_mmu_t *mmu, read_memory_struct_t *stat) {
    on_read_hook_state_t *state = stat->state;
    debugger_state_t deb_state = { read_hook_printf, state->command->state, state->asic };
    uint16_t val = parse_expression(&deb_state, state->expression);

    if (stat->memory_location == val) {
        state->command->function(&deb_state, state->argc, state->arguments);
    }

    return 0;
}

int command_on_read(struct debugger_state *state, int argc, char **argv) {
    if (argc < 3) {
        state->print(state, "%s location command [arguments...] - run a command on reading a memory location\n"
            "Warning: This command (and hooks in global) are still a WIP, and this probably won't do what you want it to do!\n", argv[0]);
        return 0;
    }

    debugger_command_t *command;

    int status = find_best_command(argv[2], &command);
    if (status == -1) {
         state->print(state, "ERROR: Ambiguous command!\n");
         return 1;
    } else if (status == 0) {
         state->print(state, "ERROR: Unknown command!\n");
         return 1;
    }

    char **new_arguments = malloc(sizeof(char *) * (argc - 2));

    int i;
    for (i = 0; i < argc - 2; i++) {
        int len = strlen(argv[i + 2]);
        new_arguments[i] = malloc(len);
        memcpy(new_arguments[i], argv[i + 2], len);
    }

    size_t strsize = strlen(argv[1]);
    char *expression = malloc(strsize);
    memcpy(expression, argv[1], strsize);

    on_read_hook_state_t *stat = malloc(sizeof(on_read_hook_state_t));

    stat->argc = argc - 2;
    stat->arguments = new_arguments;
    stat->expression = expression;
    stat->command = command;
    stat->asic = state->asic;

    register_hook_read_memory(on_read_read_hook, stat);

    return 0;
}
