/*
 * Implements the 83+SE/84+((C)SE) crystal timers
 */
#include "ti/asic.h"
#include "log/log.h"
#include "ti/memory.h"
#include "ti/hardware/timers.h"
#include "ti/hardware/interrupts.h"

#include <stdlib.h>
#include <string.h>

uint8_t read_timer_freq_port(void *device) {
	struct crystal_timer *timer = device;
	return timer->frequency;
}

uint8_t read_timer_loop_port(void *device) {
	struct crystal_timer *timer = device;
	return timer->loop;
}

uint8_t read_timer_counter_port(void *device) {
	struct crystal_timer *timer = device;
	return timer->counter;
}

void write_timer_freq_port(void *device, uint8_t val) {
	struct crystal_timer *timer = device;
	timer->frequency = val;
}

void write_timer_loop_port(void *device, uint8_t val) {
	struct crystal_timer *timer = device;
	timer->loop = val;
}

void write_timer_counter_port(void *device, uint8_t val) {
	struct crystal_timer *timer = device;
	timer->counter = val;
}

void init_crystal_timers(asic_t *asic) {
	struct crystal_timer *timer_1 = calloc(1, sizeof(struct crystal_timer));
	struct crystal_timer *timer_2 = calloc(1, sizeof(struct crystal_timer));
	struct crystal_timer *timer_3 = calloc(1, sizeof(struct crystal_timer));
	timer_1->asic = asic;
	timer_2->asic = asic;
	timer_3->asic = asic;

	z80iodevice_t timer1_freq_port = { timer_1, read_timer_freq_port, write_timer_freq_port };
	z80iodevice_t timer1_loop_port = { timer_1, read_timer_loop_port, write_timer_loop_port };
	z80iodevice_t timer1_count_port = { timer_1, read_timer_counter_port, write_timer_counter_port };

	z80iodevice_t timer2_freq_port = { timer_2, read_timer_freq_port, write_timer_freq_port };
	z80iodevice_t timer2_loop_port = { timer_2, read_timer_loop_port, write_timer_loop_port };
	z80iodevice_t timer2_count_port = { timer_2, read_timer_counter_port, write_timer_counter_port };

	z80iodevice_t timer3_freq_port = { timer_3, read_timer_freq_port, write_timer_freq_port };
	z80iodevice_t timer3_loop_port = { timer_3, read_timer_loop_port, write_timer_loop_port };
	z80iodevice_t timer3_count_port = { timer_3, read_timer_counter_port, write_timer_counter_port };

	asic->cpu->devices[0x30] = timer1_freq_port;
	asic->cpu->devices[0x31] = timer1_loop_port;
	asic->cpu->devices[0x32] = timer1_count_port;

	asic->cpu->devices[0x33] = timer2_freq_port;
	asic->cpu->devices[0x34] = timer2_loop_port;
	asic->cpu->devices[0x35] = timer2_count_port;

	asic->cpu->devices[0x36] = timer3_freq_port;
	asic->cpu->devices[0x37] = timer3_loop_port;
	asic->cpu->devices[0x38] = timer3_count_port;
}

void free_crystal_timers(asic_t *asic) {
	free(asic->cpu->devices[0x30].device);
	free(asic->cpu->devices[0x33].device);
	free(asic->cpu->devices[0x38].device);
}
