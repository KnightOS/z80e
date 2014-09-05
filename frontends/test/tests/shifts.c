int test_RLCA() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0x07 }; // RLCA
	device->cpu->registers.A = 0x80;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->registers.A != 1 ||
			device->cpu->registers.flags.C != 1 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_RRCA() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0x0F }; // RRCA
	device->cpu->registers.A = 1;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->registers.A != 0x80 ||
			device->cpu->registers.flags.C != 1 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_RLA() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0x17 }; // RLA
	device->cpu->registers.A = 0x80;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->registers.A != 0 ||
			device->cpu->registers.flags.C != 1 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_RRA() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0x1F }; // RRA
	device->cpu->registers.A = 1;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->registers.A != 0 ||
			device->cpu->registers.flags.C != 1 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_RLC() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xCB, 0x00 }; // RLC B
	device->cpu->registers.B = 0x80;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 8);
	if (device->cpu->registers.B != 1 ||
			device->cpu->registers.flags.C != 1 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_RRC() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xCB, 0x08 }; // RRC B
	device->cpu->registers.B = 1;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 8);
	if (device->cpu->registers.B != 0x80 ||
			device->cpu->registers.flags.C != 1 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_RL() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xCB, 0x10 }; // RL B
	device->cpu->registers.B = 0x80;
	device->cpu->registers.flags.C = 1;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 8);
	if (device->cpu->registers.B != 1 ||
			device->cpu->registers.flags.C != 1 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_RR() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xCB, 0x18 }; // RR B
	device->cpu->registers.B = 1;
	device->cpu->registers.flags.C = 0;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 8);
	if (device->cpu->registers.B != 0 ||
			device->cpu->registers.flags.C != 1 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_SLA() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xCB, 0x20 }; // SLA B
	device->cpu->registers.B = 0x80;
	device->cpu->registers.flags.C = 0;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 8);
	if (device->cpu->registers.B != 0 ||
			device->cpu->registers.flags.C != 1 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_SRA() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xCB, 0x28 }; // SRA B
	device->cpu->registers.B = 1;
	device->cpu->registers.flags.C = 0;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 8);
	if (device->cpu->registers.B != 0 ||
			device->cpu->registers.flags.C != 1 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_SLL() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xCB, 0x30 }; // SLL B
	device->cpu->registers.B = 1;
	device->cpu->registers.flags.C = 0;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 8);
	if (device->cpu->registers.B != 3 ||
			device->cpu->registers.flags.C != 0 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_SRL() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xCB, 0x38 }; // SRL B
	device->cpu->registers.B = 1;
	device->cpu->registers.flags.C = 0;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 8);
	if (device->cpu->registers.B != 0 ||
			device->cpu->registers.flags.C != 1 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}
