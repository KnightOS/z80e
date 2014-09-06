#include "ti/asic.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "ti/hardware/t6a04.h"
#include "ti/hardware/speed.h"
#include "ti/memory.h"
#include "ti/hardware/memorymapping.h"
#include "core/cpu.h"
#include "ti/hardware/keyboard.h"
#include "ti/hardware/status.h"
#include "ti/hardware/speed.h"

void plug_devices(asic_t *asic) {
	/* Link port unimplemented */
	asic->cpu->devices[0x01] = init_keyboard();
	asic->cpu->devices[0x02] = init_status(asic);
	asic->cpu->devices[0x03] = init_interrupts(asic, &asic->interrupts);
	setup_lcd_display(asic, asic->hook);

	if (asic->device != TI73 && asic->device != TI83p) {
		asic->cpu->devices[0x20] = init_speed(asic);
	}

	init_mapping_ports(asic);
}

void asic_null_write(void *ignored, uint8_t value) {

}

void asic_mirror_ports(asic_t *asic) {
	int i;
	switch (asic->device) {
	case TI83p:
		for (i = 0x08; i < 0x10; i++) {
			asic->cpu->devices[i] = asic->cpu->devices[i & 0x07];
			asic->cpu->devices[i].write_out = asic_null_write;
		}
		asic->cpu->devices[0x12] = asic->cpu->devices[0x10];
		asic->cpu->devices[0x13] = asic->cpu->devices[0x11];

		for (i = 0x14; i < 0x100; i++) {
			asic->cpu->devices[i] = asic->cpu->devices[i & 0x07];
			asic->cpu->devices[i].write_out = asic_null_write;
		}
		break;
	default:
		for (i = 0x60; i < 0x80; i++) {
			asic->cpu->devices[i] = asic->cpu->devices[i - 0x20];
		}
		break;
	}
}

void free_devices(asic_t *asic) {
	/* Link port unimplemented */
	free_keyboard(asic->cpu->devices[0x01].device);
	free_status(asic->cpu->devices[0x02]);
	free_mapping_ports(asic);
}

asic_t *asic_init(ti_device_type type, log_t *log) {
	asic_t* device = malloc(sizeof(asic_t));
	device->log = log;
	device->cpu = cpu_init(log);
	device->mmu = ti_mmu_init(type);
	device->cpu->memory = (void*)device->mmu;
	device->cpu->read_byte = ti_read_byte;
	device->cpu->write_byte = ti_write_byte;
	device->battery = BATTERIES_GOOD;
	device->device = type;
	device->clock_rate = 6000000;

	device->timers = calloc(sizeof(z80_hardware_timers_t), 1);
	device->timers->max_timers = 20;
	device->timers->timers = calloc(sizeof(z80_hardware_timer_t), 20);

	device->stopped = 0;
	device->debugger = 0;
	device->runloop = runloop_init(device);
	device->hook = create_hook_set(device);

	plug_devices(device);
	asic_mirror_ports(device);
	return device;
}

void asic_free(asic_t* device) {
	cpu_free(device->cpu);
	ti_mmu_free(device->mmu);
	free_devices(device);
	free(device);
}

int asic_add_timer(asic_t *asic, int flags, double frequency, timer_tick tick, void *data) {
	z80_hardware_timer_t *timer = 0;
	int i;
	for (i = 0; i < asic->timers->max_timers; i++) {
		if (!(asic->timers->timers[i].flags & TIMER_IN_USE)) {
			timer = &asic->timers->timers[i];
			break;
		}

		if (i == asic->timers->max_timers - 1) {
			asic->timers->max_timers += 10;
			asic->timers->timers = realloc(asic->timers->timers, sizeof(z80_hardware_timer_t) * asic->timers->max_timers);
			z80_hardware_timer_t *ne = &asic->timers->timers[asic->timers->max_timers - 10];
			memset(ne, 0, sizeof(z80_hardware_timer_t) * 10);
		}
	}

	timer->cycles_until_tick = asic->clock_rate / frequency;
	timer->flags = flags | TIMER_IN_USE;
	timer->frequency = frequency;
	timer->on_tick = tick;
	timer->data = data;
	return i;
}

void asic_remove_timer(asic_t *asic, int index) {
	asic->timers->timers[index].flags &= ~TIMER_IN_USE;
}

int asic_set_clock_rate(asic_t *asic, int new_rate) {
	int old_rate = asic->clock_rate;

	int i;
	for (i = 0; i < asic->timers->max_timers; i++) {
		z80_hardware_timer_t *timer = &asic->timers->timers[i];
		if (timer->flags & TIMER_IN_USE) {
			timer->cycles_until_tick =
				new_rate / (timer->cycles_until_tick * timer->frequency);
		}
	}

	asic->clock_rate = new_rate;
	return old_rate;
}
