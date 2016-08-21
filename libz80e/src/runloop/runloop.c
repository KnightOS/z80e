#include "runloop/runloop.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/un.h>
#include <poll.h>
#include <errno.h>
/* Why the heck does "get the current time" have to be so god-dammed platform specific */
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif
#ifdef APPLE
#include <mach/mach_time.h>
#define ORWL_NANO (+1.0E-9)
#define ORWL_GIGA UINT64_C(1000000000)
static double orwl_timebase = 0.0;
static uint64_t orwl_timestart = 0;
#endif
#include "ti/hardware/link.h"

long long get_time_nsec() {
#ifdef EMSCRIPTEN
	return emscripten_get_now() * 1000000;
#else
#ifdef APPLE
	if (!orwl_timestart) {
		mach_timebase_info_data_t tb = { 0 };
		mach_timebase_info(&tb);
		orwl_timebase = tb.numer;
		orwl_timebase /= tb.denom;
		orwl_timestart = mach_absolute_time();
	}
	struct timespec t;
	double diff = (mach_absolute_time() - orwl_timestart) * orwl_timebase;
	t.tv_sec = diff * ORWL_NANO;
	t.tv_nsec = diff - (t.tv_sec * ORWL_GIGA);
	return t.tv_nsec;
#else
	struct timespec sp;
	clock_gettime(CLOCK_MONOTONIC, &sp);

	return sp.tv_sec * 1000000000 + sp.tv_nsec;
#endif
#endif
}

typedef struct {
	int index;
	int after_cycle;
} timer_tick_t;

struct runloop_state {
	asic_t *asic;
	long long last_end;
	int spare_cycles;
	timer_tick_t *ticks;
	int max_tick_count;
};

runloop_state_t *runloop_init(asic_t *asic) {
	runloop_state_t *state = calloc(sizeof(runloop_state_t), 1);

	state->asic = asic;
	state->last_end = get_time_nsec();
	int i;
	for (i = 0; i < asic->timers->max_timers; i++) {
		z80_hardware_timer_t *timer = &asic->timers->timers[i];
		if (timer->flags & TIMER_IN_USE) {
			timer->cycles_until_tick = asic->clock_rate / timer->frequency;
		}
	}

	state->ticks = calloc(sizeof(timer_tick_t), 40);
	state->max_tick_count = 40;

	return state;
}

int runloop_compare(const void *first, const void *second) {
	const timer_tick_t *a = first;
	const timer_tick_t *b = second;

	return a->after_cycle - b->after_cycle;
}

void link_socket_update(asic_t *asic, int c) {
	if (c != EOF) {
		char _c = ' ';
		if (isprint((char)c)) {
			_c = (char)c;
		}
		// TODO: Log the TX better
		printf("Asked to send %02X (%c)\n", c, _c);
	}

	if (!asic->link->listenfd.fd) {
		return;
	}

	struct sockaddr_in clientaddr;
	int size = sizeof(clientaddr);
	poll(&asic->link->listenfd, 1, 0);
	int fd = -1;
	if (asic->link->accept++ == 4096) { // stupid hack
		fd = accept(asic->link->listenfd.fd, (struct sockaddr *)&clientaddr, &size);
		asic->link->accept = 0;
	}

	int i;
	for (i = 0; i < sizeof(asic->link->clients) / sizeof(int); ++i) {
		struct pollfd client = asic->link->clients[i];
		if (asic->link->clients[i].fd == 0) {
			if (fd != -1) {
				asic->link->clients[i].fd = fd;
				asic->link->clients[i].events = POLLIN;
				printf("Client accepted with fd %d\n", fd);
				fd = -1;
			} else {
				continue;
			}
		}
		if (asic->link->clients[i].fd != 0) {
			uint8_t val;
			poll(&asic->link->clients[i], 1, 0);
			if (asic->link->clients[i].revents & POLLIN) {
				if (link_recv_ready(asic)) {
					if (read(asic->link->clients[i].fd, &val, 1)) {
						link_recv_byte(asic, val);
					}
				}
			}
			if (c != EOF) {
				val = c;
				write(asic->link->clients[i].fd, &c, 1);
			}
		}
	}
}

void runloop_tick_cycles(runloop_state_t *state, int cycles) {
	int total_cycles = 0;
	int cycles_until_next_tick = cycles;
	int current_tick = 0;
	int i;
	for (i = 0; i < state->asic->timers->max_timers; i++) {
		z80_hardware_timer_t *timer = &state->asic->timers->timers[i];

		if (!(timer->flags & TIMER_IN_USE)) {
			continue;
		}

		int tot_cycles = cycles;
		if (timer->cycles_until_tick < tot_cycles) {
			retry:
			state->ticks[current_tick].index = i;
			state->ticks[current_tick].after_cycle = timer->cycles_until_tick + (cycles - tot_cycles);
			tot_cycles -= timer->cycles_until_tick;
			timer->cycles_until_tick = state->asic->clock_rate / timer->frequency;
			current_tick++;

			if (current_tick == state->max_tick_count) {
				state->max_tick_count += 10;
				state->ticks = realloc(state->ticks, sizeof(timer_tick_t) * state->max_tick_count);
			}

			if (timer->cycles_until_tick <= tot_cycles) {
				goto retry;
			}
		} else {
			timer->cycles_until_tick -= tot_cycles;
		}
	}

	qsort(state->ticks, current_tick, sizeof(timer_tick_t), runloop_compare);

	if (current_tick > 0) {
		cycles_until_next_tick = state->ticks[0].after_cycle;
	}

	int tick_i = 0;
	while (cycles > 0) {
		int ran = cycles_until_next_tick - cpu_execute(state->asic->cpu, cycles_until_next_tick);
		int c = link_read_tx_buffer(state->asic);
		link_socket_update(state->asic, c);

		total_cycles += ran;
		cycles -= ran;

		if (total_cycles >= state->ticks[tick_i].after_cycle) {
			tick_i++;
			if (tick_i <= current_tick) {
				int index = state->ticks[tick_i - 1].index;
				z80_hardware_timer_t *timer = &state->asic->timers->timers[index];
				timer->on_tick(state->asic, timer->data);
				cycles_until_next_tick = state->ticks[tick_i].after_cycle - total_cycles;
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
