#include "debugger.h"
#include "memory.h"

#include <stdlib.h>
#include <ctype.h>

int command_hexdump(struct debugger_state *state, int argc, char **argv) {
    if (argc != 3) {
        state->print(state, "%s `start` `length` - print an amount of bytes from the memory\n"
                " Prints the bytes starting from `start`, `length` bytes in total.\n", argv[0]);
        return 0;
    }

    ti_mmu_t *mmu = (ti_mmu_t *)state->state;

    uint16_t start = strtol(argv[1], NULL, 0);
    uint16_t length = strtol(argv[2], NULL, 0);


    int count = length;
    int i = 0;
    while (count >= 16) {
        state->print(state, "%04X ", start + i);
        int j;
        for (j = 0; j < 16; j++) {
            state->print(state, "%02X ", ti_read_byte(mmu, start + i + j));
        }

        state->print(state, " |");
        for (j = 0; j < 16; j++) {
            char byte = ti_read_byte(mmu, start + i + j);
            if (isprint(byte) && byte != '\t') {
                state->print(state, "%c", byte);
            } else {
                state->print(state, ".");
            }
        }
        state->print(state, "|\n");

        i += 16;
        count -= 16;
    }

    if (count == 0) {
        return 0;
    }

    state->print(state, "%04X ", start + i);
    int j;
    for (j = 0; j < count; j++) {
        state->print(state, "%02X ", ti_read_byte(mmu, start + i + j));
    }

    state->print(state, "%*s |", (16 - count) * 3, " ");
    for (j = 0; j < count; j++) {
        char byte = ti_read_byte(mmu, start + i + j);
        if (isprint(byte) && byte != '\t') {
            state->print(state, "%c", byte);
        } else {
            state->print(state, ".");
        }
    }
    state->print(state, "|\n");

    return 0;
}
