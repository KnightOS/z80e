#include "ti/asic.h"
#include "log/log.h"
#include "ti/memory.h"
#include "ti/hardware/link.h"

#include <stdlib.h>
#include <string.h>

uint8_t read_link_port(void *device) {
	link_state_t *state = device;
	switch (state->asic->device) {
	case TI73:
	case TI83p:
		// TODO: Link assist for 83+ models
		return (
			(state->them.tip | state->us.tip) |
			((state->them.ring | state->us.ring) << 1) |
			(state->us.tip << 4) |
			(state->us.ring << 5)
		);
	case TI83pSE:
	case TI84p:
	case TI84pSE:
	case TI84pCSE:
		return (
			(state->them.tip | state->us.tip) |
			((state->them.ring | state->us.ring) << 1) |
			(state->us.tip << 4) |
			(state->us.ring << 5)
		);
	default:
		return 0;
	}
}

void write_link_port(void *device, uint8_t val) {
	link_state_t *state = device;
	uint8_t tip = val & 1;
	uint8_t ring = val & 2;
	state->us.tip = tip;
	state->us.ring = ring;
}

uint8_t read_link_assist_enable_port(void *device) {
	link_state_t *state = device;
	switch (state->asic->device) {
	case TI73:
	case TI83p:
		return 0;
	default:
		return state->interrupts.mask;
	}
}

void write_link_assist_enable_port(void *device, uint8_t val) {
	link_state_t *state = device;
	switch (state->asic->device) {
	case TI73:
	case TI83p:
		break;
	default:
		state->interrupts.mask = val;
		break;
	}
}

void init_link_ports(asic_t *asic) {
	link_state_t *state = malloc(sizeof(link_state_t));

	memset(state, 0, sizeof(link_state_t));
	state->asic = asic;

	z80iodevice_t link_port = { state, read_link_port, write_link_port };
	z80iodevice_t link_assist_enable = { state, read_link_assist_enable_port, write_link_assist_enable_port };
	// TODO: link assist ports

	asic->cpu->devices[0x00] = link_port;
	asic->cpu->devices[0x08] = link_assist_enable;
}

void free_link_ports(asic_t *asic) {
	free(asic->cpu->devices[0x00].device);
}
