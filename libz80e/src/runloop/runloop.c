#include "runloop/runloop.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>

typedef struct {
	int cycles_until_tick;
} timer_info_t;

long long get_time_nsec() {
	struct timespec sp;
	clock_gettime(CLOCK_MONOTONIC, &sp);

	return sp.tv_sec * 1000000000 + sp.tv_nsec;
}

struct runloop_state {
	asic_t *asic;
	long long last_end;
	int spare_cycles;
	timer_info_t timers_info[ASIC_TIMER_MAX];
};

typedef struct {
	int index;
	int after_cycle;
} timer_tick_t;

runloop_state_t *runloop_init(asic_t *asic) {
	runloop_state_t *state = malloc(sizeof(runloop_state_t));

	state->asic = asic;
	state->last_end = get_time_nsec();

	int i;
	for (i = 0; i < ASIC_TIMER_MAX; i++) {
		timer_info_t *info = &state->timers_info[i];
		z80_timer_t *timer = &state->asic->timers[i];
		if (timer->frequency > 0) {
			info->cycles_until_tick = asic->clock_rate / timer->frequency;
		}
	}

	return state;
}

int runloop_compare(const void *first, const void *second) {
	const timer_tick_t *a = first;
	const timer_tick_t *b = second;

	return a->after_cycle - b->after_cycle;
}

void runloop_tick_cycles(runloop_state_t *state, int cycles) {
	int total_cycles = 0;

	timer_tick_t ticks[ASIC_TIMER_MAX * 4]; // 4 ticks per cycle will be enough, I hope
	int current_tick = 0;

	int cycles_until_next_tick = cycles;

	int i;
	for (i = 0; i < state->asic->max_timer; i++) {
		z80_timer_t *timer = &state->asic->timers[i];
		timer_info_t *info = &state->timers_info[i];

		int tot_cycles = cycles;

		if (info->cycles_until_tick < tot_cycles) {
			retry:
			ticks[current_tick].index = i;
			ticks[current_tick].after_cycle = info->cycles_until_tick + (cycles - tot_cycles);
			tot_cycles -= info->cycles_until_tick;
			info->cycles_until_tick = state->asic->clock_rate / timer->frequency;
			current_tick++;
			if (info->cycles_until_tick <= tot_cycles) {
				goto retry;
			}
		} else {
			info->cycles_until_tick -= tot_cycles;
		}
	}

	qsort(ticks, current_tick, sizeof(timer_tick_t), runloop_compare);

	if (current_tick > 0) {
		cycles_until_next_tick = ticks[0].after_cycle;
	}

	int tick_i = 0;
	while (cycles > 0) {
		int ran = cycles_until_next_tick - cpu_execute(state->asic->cpu, cycles_until_next_tick);

		total_cycles += ran;
		cycles -= ran;

		if (total_cycles >= ticks[tick_i].after_cycle) {
			tick_i++;
			if (tick_i <= current_tick) {
				int index = ticks[tick_i - 1].index;
				state->asic->timers[index].on_tick(state->asic, state->asic->timers[index].timer_data);
				cycles_until_next_tick = ticks[tick_i].after_cycle - total_cycles;
			} else {
				cycles_until_next_tick = cycles;
			}
		}
	}
	state->spare_cycles = cycles;
}

void runloop_tick(runloop_state_t *state) {
	long long now = get_time_nsec();
	long long ticks_between = now - state->last_end;

	float seconds = (float)ticks_between / (float)1000000000;
	int cycles = seconds * (float)state->asic->clock_rate;

	if (cycles == 0)
		return;

	runloop_tick_cycles(state, cycles);
	state->last_end = now;
}
