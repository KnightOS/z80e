#include "asic.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    asic_t* device = asic_init(TI84pSE);
    z80cpu_write_byte(device->cpu, 0x0000, 0x32);
    z80cpu_write_byte(device->cpu, 0xC000, 0x48);
    printf("0x%02x\n", z80cpu_read_byte(device->cpu, 0x0000));
    printf("0x%02x\n", z80cpu_read_byte(device->cpu, 0xC000));
    asic_free(device);
    return 0;
}
