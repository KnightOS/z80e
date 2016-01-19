#ifndef LINK_H
#define LINK_H

#include <z80e/ti/asic.h>

typedef struct {
	asic_t *asic;
	struct {
		uint8_t tip		: 1;
		uint8_t ring	: 1;
	} them;
	struct {
		uint8_t tip		: 1;
		uint8_t ring	: 1;
	} us;
	union {
		uint8_t mask;
		struct {
			uint8_t recv		: 1;
			uint8_t sent		: 1;
			uint8_t error		: 1;
			uint8_t 			: 4;
			uint8_t disabled	: 1;
		};
	} interrupts;
	void (*send)(uint8_t val, void *data);
	struct {
		uint8_t buffer[0x100];
		int recv_index;
		int read_index;
	} la;
} link_state_t;

void init_link_ports(asic_t *asic);
void free_link_ports(asic_t *asic);
/**
 * Receives a byte via link assist.
 */
void link_recv_byte(asic_t *asic, uint8_t val);
/**
 * Sets the handler for when the emulated calculator attempts to send a byte.
 */
void link_send_byte(asic_t *asic, void (*handler)(uint8_t val, void *data));

#endif
