#include "asic.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void flash(asic_t *device, uint8_t *data);

int test_ADD_A_r(asic_t *device) {
    uint8_t test[] = { 0x80 };
    device->cpu->registers.A = 0x10;
    device->cpu->registers.B = 0x20;
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 1);
    if (device->cpu->registers.A != 0x30 ||
        device->cpu->registers.B != 0x20 ||
        device->cpu->registers.flags.Z != 0 ||
        device->cpu->registers.flags.C != 0 ||
        cycles != -3) {
        return 1;
    }
    return 0;
}

typedef int (*test_function_t)(asic_t *device);
typedef struct {
    test_function_t execute;
    char* name;
} test_t;

const test_t tests[] = {
    { test_ADD_A_r, "ADD A, r" }
};

int main(int argc, char **argv) {
    int passed = 0, failed = 0;
    int i;
    const int min_width = 50;
    for (i = 0; i < sizeof(tests) / sizeof(test_t); i++) {
        printf("Testing %s ", tests[i].name);
        int length = min_width - strlen(tests[i].name);
        while (length--) printf(".");
        asic_t *device = asic_init(TI83p);
        int result = tests[i].execute(device);
        asic_free(device);
        if (result > 0) {
            printf("FAIL\n");
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
