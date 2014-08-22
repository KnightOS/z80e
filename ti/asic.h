#ifndef ASIC_H
#define ASIC_H

#include <stdint.h>

typedef struct asic asic_t;

#include "cpu.h"
#include "memory.h"
#include "ti.h"
#include "runloop.h"
#include "hooks.h"
#include "interrupts.h"

typedef enum {
    BATTERIES_REMOVED,
    BATTERIES_LOW,
    BATTERIES_GOOD
} battery_state;


typedef void (*timer_tick)(asic_t *, void *);
typedef struct z80_hardware_timers z80_hardware_timers_t;
typedef struct z80_hardware_timer z80_hardware_timer_t;

enum {
	TIMER_IN_USE = (1 << 0),
	TIMER_ONE_SHOT = (1 << 1)
};

struct z80_hardware_timer {
	int cycles_until_tick;

	int flags;
	double frequency;
	timer_tick on_tick;
	void *data;
};

struct z80_hardware_timers {
	int max_timers;
	z80_hardware_timer_t *timers;
};

typedef enum {
    DEBUGGER_DISABLED,
    DEBUGGER_ENABLED,
    DEBUGGER_LONG_OPERATION,
    DEBUGGER_LONG_OPERATION_INTERRUPTABLE
} debugger_state;

typedef struct {
    int stopped: 1;
    debugger_state debugger: 2;
    runloop_state_t *runloop;
} ti_emulation_state_t;

struct asic {
    z80cpu_t* cpu;
    ti_emulation_state_t *state;
    ti_device_type device;
    ti_mmu_t* mmu;
    battery_state battery;
    int battery_remove_check;
    int clock_rate;
    ti_interrupts_t *interrupts;
    z80_hardware_timers_t *timers;
    hook_info_t *hook;
};

asic_t* asic_init(ti_device_type);
void asic_free(asic_t*);

int asic_set_clock_rate(asic_t *, int);

int asic_add_timer(asic_t *, int, double, timer_tick, void *);
void asic_remove_timer(asic_t *, int);

#endif
