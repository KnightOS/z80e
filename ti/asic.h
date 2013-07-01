#ifndef ASIC_H
#define ASIC_H
#include "cpu.h"
#include "memory.h"
#include "ti.h"
#include <stdint.h>


typedef struct {
    z80cpu_t* cpu;
    ti_mmu_t* mmu;
} asic_t;

asic_t* asic_init(ti_device_type);
void asic_free(asic_t*);

#endif
