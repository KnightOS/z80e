#ifndef TIMERS_H
#define TIMERS_H

#include <z80e/ti/asic.h>
#include <stdint.h>

struct crystal_timer {
    asic_t *asic;
    uint8_t frequency;
    uint8_t loop;
    uint8_t counter;
};

void init_crystal_timers(asic_t *asic);
void free_crystal_timers(asic_t *asic);

#endif
