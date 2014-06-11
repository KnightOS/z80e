#include "debugger.h"
#include "memory.h"

#include <stdlib.h>
#include <ctype.h>

int command_hexdump(struct debugger_state *state, int argc, char **argv) {
    if (argc != 3 && argc != 4) {
        state->print(state, "%s `start` `length` - print an amount of bytes from the memory\n"
                " Prints the bytes starting from `start`, `length` bytes in total.\n", argv[0]);
        return 0;
    }

    ti_mmu_t *mmu = (ti_mmu_t *)state->state;

    uint16_t start = strtol(argv[1], NULL, 0);
    uint16_t length = strtol(argv[2], NULL, 0);

    int column = 26;
    if (argc == 4) {
        column = atoi(argv[3]);
    }

    uint16_t i = 0;
    for (i = 0; i < length; i++) {
        if (i % 16 == 0) {
            state->print(state, "%02X  ");
        }
        state->print(state, "%02X ", ti_read_byte(mmu, start + i));
        if (i % column == 0 && i > 0) {
            int j = 0;
            state->print(state," |");
            for (j = 0; j < 16; j++) {
                char byte = ti_read_byte(mmu, start + i - 16 + j);
                if (isprint(byte) && byte != '\t') {
                    state->print(state, "%c", byte);
                } else {
                    state->print(state, ".");
                }
            }
            state->print(state, "|\n");
        }
    }

    if (i % 16 != 0) {
        int over = i % 16;
        int j;
        state->print(state, "%*s |", 3 * (16 - over), " ");
        for (j = 0; j < over; j++) {
            char byte = ti_read_byte(mmu, start + i - 16 + j);
            if (isprint(byte) && byte != '\t') {
                state->print(state, "%c", byte);
            } else {
                state->print(state, ".");
            }
        }     
        state->print(state, "|\n");
    }

    return 0;
}
