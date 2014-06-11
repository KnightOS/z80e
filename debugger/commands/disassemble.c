#include "memory.h"

#include "debugger.h"
#include "disassemble.h"

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

struct mmu_disassemble_memory {
        struct disassemble_memory mem;
        ti_mmu_t *mmu;
        struct debugger_state *state;
};

int disassemble_print(struct disassemble_memory *s, const char *format, ...) {
    struct mmu_disassemble_memory *m = (struct mmu_disassemble_memory *)s;

    char buffer[50];

    va_list list;
    va_start(list, format);

    vsnprintf(buffer, 50, format, list);

    return m->state->print(m->state, "%s", buffer);
}

uint8_t disassemble_read(struct disassemble_memory *s, uint16_t pointer) {
    struct mmu_disassemble_memory *m = (struct mmu_disassemble_memory *)s;
    return ti_read_byte(m->mmu, pointer);
}

int command_disassemble(struct debugger_state *state, int argc, char **argv) {
    if (argc != 3) {
        state->print(state, "%s `start` `count` - Print the disassembled commands\n"
                " Prints `count` disassembled commands starting in memory from `start`.\n", argv[0]);
        return 0;
    }

    ti_mmu_t *mmu = (ti_mmu_t *)state->state;

    uint16_t start = strtol(argv[1], NULL, 0);
    uint16_t count = strtol(argv[2], NULL, 0);

    uint16_t i = 0;

    struct mmu_disassemble_memory str = { { disassemble_read, start }, mmu, state };

    for (i = 0; i < count; i++) {
        state->print(state, "0x%04X: ", str.mem.current);
        parse_instruction(&(str.mem), disassemble_print);
        state->print(state, "\n");
    }

    return 0;
}
