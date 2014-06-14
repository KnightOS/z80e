#include "commands.h"
#include "debugger.h"

int command_print_mappings(struct debugger_state *state, int argc, char **argv) {
    if (argc != 1) {
        state->print(state, "%s - Print the MMU page mappings.\n", argv[0]);
        return 0;
    }

    int i = 0;

    for (i = 0; i < 4; i++) {
        ti_mmu_bank_state_t mapping = state->asic->mmu->banks[i];
        state->print(state, "Page %d (0x%04X - 0x%04X): mapped to %s page %d (0x%04X - 0x%04X).\n",
            i, i * 0x4000, (i + 1) * 0x4000 - 1, mapping.flash ? "ROM" : "RAM", mapping.page,
            mapping.page * 0x4000, (mapping.page + 1) * 0x4000 - 1);
    }

    return 0;
}
