#ifndef LINK_H
#define LINK_H

#include <z80e/ti/asic.h>
#include <stdbool.h>

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
			uint8_t rx			: 1;
			uint8_t tx			: 1;
			uint8_t error		: 1;
			uint8_t 			: 4;
			uint8_t disabled	: 1;
		};
	} interrupts;
	struct {
		uint8_t rx_buffer;
		uint8_t tx_buffer;
		union {
			struct {
				uint8_t int_rx_ready	: 1;
				uint8_t int_tx_ready	: 1;
				uint8_t int_error		: 1;
				uint8_t rx_active		: 1;
				uint8_t rx_ready		: 1;
				uint8_t tx_ready		: 1;
				uint8_t error			: 1;
				uint8_t tx_active		: 1;
			};
			uint8_t u8;
		} status;
	} assist;
	bool la_ready;
} link_state_t;

void init_link_ports(asic_t *asic);
void free_link_ports(asic_t *asic);
/**
 * Receives a byte via link assist.
 */
bool link_recv_byte(asic_t *asic, uint8_t val);
/**
 * Reads a byte from the tx buffer and sets the link assist state to ready to send another byte.
 */
int link_read_tx_buffer(asic_t *asic);

bool link_recv_ready(asic_t *asic);

#endif
