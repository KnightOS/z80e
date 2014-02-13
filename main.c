#include "asic.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    asic_t* device = asic_init(TI84pSE);
    device->mmu->flash[0] = 0x80;
    cpu_execute(device->cpu, 100000000);
    asic_free(device);
    return 0;
}
