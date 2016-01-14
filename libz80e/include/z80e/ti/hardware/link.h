#ifndef LINK_H
#define LINK_H

#include <z80e/ti/asic.h>

typedef struct {
	asic_t *asic;
	struct {
		uint8_t tip	: 1;
		uint8_t ring	: 1;
	} them;
	struct {
		uint8_t tip	: 1;
		uint8_t ring	: 1;
	} us;
} link_state_t;

void init_link_ports(asic_t *asic);
void free_link_ports(asic_t *asic);

#endif
