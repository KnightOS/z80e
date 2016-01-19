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

uint8_t read_link_assist_buffer_port(void *device) {
	link_state_t *state = device;
	switch (state->asic->device) {
	case TI73:
	case TI83p:
		return 0;
	default:
		if (state->la.read_index == state->la.recv_index) {
			return 0;
		}
		return state->la.buffer[state->la.recv_index++];
	}
}

void write_link_assist_buffer_port(void *device, uint8_t val) {
	// Not emualted by z80e
}

void init_link_ports(asic_t *asic) {
	link_state_t *state = malloc(sizeof(link_state_t));

	memset(state, 0, sizeof(link_state_t));
	state->asic = asic;

	z80iodevice_t link_port = { state, read_link_port, write_link_port };
	z80iodevice_t link_assist_enable = { state, read_link_assist_enable_port, write_link_assist_enable_port };
	z80iodevice_t link_assist_buffer_read = { state, read_link_assist_buffer_port, write_link_assist_buffer_port };

	asic->cpu->devices[0x00] = link_port;
	asic->cpu->devices[0x08] = link_assist_enable;
	asic->cpu->devices[0x0A] = link_assist_buffer_read;
}

void free_link_ports(asic_t *asic) {
	free(asic->cpu->devices[0x00].device);
}

/**
 * Receives a byte via link assist.
 */
void link_recv_byte(asic_t *asic, uint8_t val) {
	// TODO: better buffering here
	link_state_t *state = asic->cpu->devices[0x00].device;
	state->la.buffer[state->la.read_index++] = val;
	if (state->la.read_index == 0xFF) {
		state->la.read_index = 0;
	}
	if (!state->interrupts.disabled) {
		// TODO
		// asic->cpu->interrupt = 1;
	}
}

/**
 * Sets the handler for when the emulated calculator attempts to send a byte.
 */
void link_send_byte(asic_t *asic, void (*handler)(uint8_t val, void *data)) {
	link_state_t *state = asic->cpu->devices[0x00].device;
	state->send = handler;
}
