#include "main.h"
#include "asic.c"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    asic_t* device = asic_init();
    asic_free(device);
    return 0;
}
