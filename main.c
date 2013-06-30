#include "main.h"
#include "asic.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    asic_t* device = asic_init();
    uint8_t value = z80cpu_read_byte(device->cpu, 0x0000);
    printf("0x0000: %d\n", value);
    asic_free(device);
    return 0;
}
