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
} link_state_t;

void init_link_ports(asic_t *asic);
void free_link_ports(asic_t *asic);

#endif
