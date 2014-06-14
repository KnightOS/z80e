#ifndef STATUS_H
#define STATUS_H
#include "cpu.h"
#include "ti.h"
#include "asic.h"

z80iodevice_t init_status(asic_t *asic);
void free_status(z80iodevice_t status);

#endif

