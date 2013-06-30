#include "main.h"
#include "asic.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    asic_t* device = asic_init();
    uint16_t i = 0;
    for (i; i < 3; i++) {
        printf("0x%04x: %d\n", i, z80cpu_read_byte(device->cpu, i));
    }
    asic_free(device);
    return 0;
}
