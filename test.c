#include "asic.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void flash(asic_t *device, uint8_t *data);

#include "tests/alu.c"
#include "tests/control.c"
#include "tests/load.c"
#include "tests/arithmetic.c"

typedef int (*test_function_t)();
typedef struct {
    test_function_t execute;
    char* name;
} test_t;

const test_t tests[] = {
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
    { test_LD_r_n, "LD r, n" }
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
        } else {
            printf("PASS\n");
            passed++;
        }
    }
    printf("%d passed, %d failed\n", passed, failed);
    return failed;
}

void flash(asic_t *device, uint8_t *data) {
    int i;
    for (i = 0; i < sizeof(data) / sizeof(uint8_t); i++) {
        device->mmu->flash[i] = data[i];
    }
}
