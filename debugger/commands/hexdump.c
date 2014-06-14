#include "commands.h"
#include "debugger.h"
#include "memory.h"

#include <stdlib.h>
#include <ctype.h>

int command_hexdump(struct debugger_state *state, int argc, char **argv) {
    if (argc > 3) {
        state->print(state, "%s [start] [length] - print an amount of bytes from the memory\n"
                " Prints the bytes starting from `start` (or PC), `length` (default: 64) bytes in total.\n", argv[0]);
        return 0;
    }

    ti_mmu_t *mmu = state->asic->mmu;

    uint16_t start = state->asic->cpu->registers.PC;
    if (argc > 1) {
        start  = parse_expression(state, argv[1]);
    }

    uint16_t length = 64;
    if (argc > 2) {
        length = parse_expression(state, argv[2]);
    }

    uint16_t i, total;

    while (length > 0) {
        state->print(state, "0x%04X ", start);

        for (i = 0; i < 8 && length - i > 0; i++) {
            state->print(state, "%02X ", ti_read_byte(mmu, start + i));
        }
        start += i;
        length -= i;
        total += i;

        state->print(state, " ");

        for (i = 0; i < 8 && length - i > 0; i++) {
            state->print(state, "%02X ", ti_read_byte(mmu, start + i));
        }
        start += i;
        length -= i;
        total += i;

        state->print(state, "%*s|", (16 - total) * 3 + (i < 8 ? 1 : 0), " ");
        for (i = 0; i < total; i++) {
            char c = ti_read_byte(mmu, start - 16 + i);
            if (isprint(c) && c != '\t') {
                state->print(state, "%c", c);
            } else {
                state->print(state, ".");
            }
        }
        state->print(state, "|\n");
        total = 0;
    }

    return 0;
}
