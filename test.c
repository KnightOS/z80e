#include "asic.h"
#include "cpu.h"
#include "debugger.h"
#include "keyboard.h"
#include "status.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void flash(asic_t *device, const uint8_t *data, size_t length);

#include "tests/alu.c"
#include "tests/control.c"
#include "tests/debugger.c"
#include "tests/load.c"
#include "tests/arithmetic.c"
#include "tests/shifts.c"
#include "tests/io.c"
#include "tests/index.c"
#include "tests/performance.c"
#include "tests/block.c"
#include "tests/interrupts.c"
#include "tests/devices.c"

typedef struct {
    int (*execute)(void);
    char *name;
} test_t;

const test_t tests[] = {
    { test_performance, "performance" },
    { test_ADD_A_r, "ADD A, r" },
    { test_ADC_A_r, "ADC A, r" },
    { test_SUB_A_r, "SUB A, r" },
    { test_SBC_A_r, "SBC A, r" },
    { test_AND_A_r, "AND A, r" },
    { test_XOR_A_r, "XOR A, r" },
    { test_OR_A_r, "OR A, r" },
    { test_CP_r, "CP r" },
    { test_RST, "RST y" },
    { test_DJNZ, "DJNZ d" },
    { test_JR, "JR d" },
    { test_JR_cc, "JR cc, d" },
    { test_LD_rp_nn, "LD rp, nn" },
    { test_ADD_HL_rp, "ADD HL, rp" },
    { test_LD_BCptr_A, "LD (BC), A" },
    { test_LD_nnptr_HL, "LD (nn), HL" },
    { test_LD_nnptr_A, "LD (nn), A" },
    { test_LD_HL_nnptr, "LD HL, (nn)" },
    { test_LD_A_nnptr, "LD A, (nn)" },
    { test_INC_rp, "INC rp" },
    { test_INC_r, "INC r" },
    { test_DEC_rp, "DEC rp" },
    { test_DEC_r, "DEC r" },
    { test_LD_r_n, "LD r, n" },
    { test_RLCA, "RLCA" },
    { test_RRCA, "RRCA" },
    { test_RLA, "RLA" },
    { test_RRA, "RRA" },
    { test_CPL, "CPL" },
    { test_DAA, "DAA" },
    { test_LD_r_r, "LD r, r" },
    { test_POP_rp2, "POP rp2" },
    { test_RET_cc, "RET cc" },
    { test_RET, "RET" },
    { test_EXX, "EXX" },
    { test_JP_HL, "JP (HL)" },
    { test_LD_SP_HL, "LD SP, HL" },
    { test_JP_cc_nn, "JP cc, nn" },
    { test_JP_nn, "JP nn" },
    { test_OUT_n_A, "OUT (n), A" },
    { test_IN_A_n, "IN A, (n)" },
    { test_EX_SP_HL, "EX (SP), HL" },
    { test_EX_DE_HL, "EX DE, HL" },
    { test_CALL_cc_nn, "CALL cc, nn" },
    { test_PUSH_rp2, "PUSH rp2" },
    { test_CALL_nn, "CALL nn" },
    { test_alu_n, "alu[y] n" },
    { test_JP_IX__JP_IY, "JP IX / JP IY" },
    { test_ADD_IX_rp, "ADD IX, rp" },
    { test_prefix_reset, "prefix reset" },
    { test_index_offsets, "index offsets" },
    { test_ixh_ixl, "IXH/IXL" },
    { test_IM_set, "interrupt mode set" },
    { test_RLC, "RLC r" },
    { test_RRC, "RRC r" },
    { test_RL, "RL r" },
    { test_RR, "RR r" },
    { test_SLA, "SLA r" },
    { test_SRA, "SRA r" },
    { test_SLL, "SLL r" },
    { test_SRL, "SRL r" },
    { test_BIT, "BIT y, r" },
    { test_RES, "RES y, r" },
    { test_SET, "SET y, r" },
    { test_EI_DI, "EI/DI" },
    { test_NEG, "NEG" },
    { test_IN_C, "IN (C)" },
    { test_IN_r_C, "IN r, (C)" },
    { test_OUT_C_0, "OUT (C), 0" },
    { test_OUT_C_r, "OUT (C), r" },
    { test_SBC_HL_rp, "SBC HL, rp" },
    { test_ADC_HL_rp, "ADC HL, rp" },
    { test_LD_nn_rp, "LD (nn), rp" },
    { test_LD_rp_nn_ind, "LD rp, (nn)" },
    { test_LD_A_I, "LD A, I" },
    { test_RRD_RLD, "RRD/RLD" },
    { test_LDI, "LDI" },
    { test_LDIR, "LDIR" },
    { test_LDD, "LDD" },
    { test_LDDR, "LDDR" },
    { test_CPI, "CPI" },
    { test_CPD, "CPD" },
    { test_CPIR, "CPIR" },
    { test_CPDR, "CPDR" },
    { test_INI, "INI" },
    { test_IND, "IND" },
    { test_INIR, "INIR" },
    { test_INDR, "INDR" },
    { test_OUTI, "OUTI" },
    { test_OUTD, "OUTD" },
    { test_OTIR, "OTIR" },
    { test_OTDR, "OTDR" },
    { test_IM_1, "mode 1 interrupts" },
    { test_IM_2, "mode 2 interrupts" },
    { test_debugger_find_command, "debugger: find_best_command()" },
    { test_keyboard, "port 0x01 (keyboard)" },
    { test_status, "port 0x02 (status)" },
};

int main(int argc, char **argv) {
    int passed = 0, failed = 0;
    int i;
    const int min_width = 50;
    for (i = 0; i < sizeof(tests) / sizeof(test_t); i++) {
        printf("Testing %s ", tests[i].name);
        int length = min_width - strlen(tests[i].name);
        while (length--) printf(".");
        int result = tests[i].execute();
        if (result > 0) {
            printf("FAIL %d\n", result);
            failed++;
        } else if (result == -1) {
            // Silent test
        } else {
            printf("PASS\n");
            passed++;
        }
    }
    printf("%d passed, %d failed\n", passed, failed);
    return failed;
}

void flash(asic_t *device, const uint8_t *data, size_t length) {
    int i;
    for (i = 0; i < length; i++) {
        device->mmu->flash[i] = data[i];
    }
}
