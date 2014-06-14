#ifndef ASIC_H
#define ASIC_H
#include "cpu.h"
#include "memory.h"
#include "ti.h"
#include <stdint.h>

typedef enum {
    BATTERIES_REMOVED,
    BATTERIES_LOW,
    BATTERIES_GOOD
} battery_state;

typedef struct {
    z80cpu_t* cpu;
    ti_device_type device;
    ti_mmu_t* mmu;
    battery_state battery;
    int battery_remove_check;
} asic_t;

asic_t* asic_init(ti_device_type);
void asic_free(asic_t*);

#endif
