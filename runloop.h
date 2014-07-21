#ifndef RUNLOOP_H
#define RUNLOOP_H

#include "asic.h"

typedef struct runloop_state runloop_state_t;

runloop_state_t *runloop_init(asic_t *);

void runloop_tick_cycles(runloop_state_t *, int);
void runloop_tick(runloop_state_t *);

#endif
