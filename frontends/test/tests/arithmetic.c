int test_ADD_HL_rp() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0x09 }; // ADD HL, BC
	device->cpu->registers.HL = 0x1000;
	device->cpu->registers.BC = 0x0234;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 11);
	if (device->cpu->registers.HL != 0x1234 ||
			device->cpu->registers.flags.Z != 0 ||
			device->cpu->registers.flags.C != 0 ||
			device->cpu->registers.flags.H != 0 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);

	device = asic_init(TI83p, NULL);
	uint8_t test2[] = { 0xED, 0x4A }; // ADC HL, BC
	device->cpu->registers.HL = 0xFF00;
	device->cpu->registers.BC = 0x1000;
	device->cpu->registers.flags.C = 0;
	flash(device, test2, sizeof(test2));
	cycles = cpu_execute(device->cpu, 15);
	if (device->cpu->registers.HL != 0x0F00 ||
			device->cpu->registers.flags.Z != 0 ||
			device->cpu->registers.flags.H != 0 ||
			device->cpu->registers.flags.PV != 0 ||
			device->cpu->registers.flags.C != 1 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_INC_rp() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0x23 }; // INC HL
	device->cpu->registers.HL = 0xFFFF;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 6);
	if (device->cpu->registers.HL != 0 ||
			device->cpu->registers.flags.Z != 0 ||
			device->cpu->registers.flags.C != 0 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_INC_r() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0x3C }; // INC A
	device->cpu->registers.A = 0xFF;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->registers.HL != 0 ||
			device->cpu->registers.flags.Z != 1 ||
			device->cpu->registers.flags.C != 0 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_DEC_rp() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0x2B }; // DEC HL
	device->cpu->registers.HL = 1;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 6);
	if (device->cpu->registers.HL != 0 ||
			device->cpu->registers.flags.Z != 0 ||
			device->cpu->registers.flags.C != 0 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_DEC_r() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0x3D }; // INC A
	device->cpu->registers.A = 1;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->registers.HL != 0 ||
			device->cpu->registers.flags.Z != 1 ||
			device->cpu->registers.flags.C != 0 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_CPL() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0x2F }; // CPL
	device->cpu->registers.A = 0x80;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->registers.A != 0x7F ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_DAA() { // TODO: This could be more comprehensive
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0x80, 0x27 }; // ADD A, B \ DAA
	device->cpu->registers.A = 0x15;
	device->cpu->registers.B = 0x27;
	flash(device, test, sizeof(test));
	cpu_execute(device->cpu, 1);
	int cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->registers.A != 0x42 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_BIT() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xCB, 0x78 }; // BIT 7, B
	device->cpu->registers.B = 0x80;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 8);
	if (device->cpu->registers.flags.Z == 1 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_RES() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xCB, 0xB8 }; // RES 7, B
	device->cpu->registers.B = 0xFF;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 8);
	if (device->cpu->registers.B != 0x7F ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_SET() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xCB, 0xF8 }; // SET 7, B
	device->cpu->registers.B = 0;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 8);
	if (device->cpu->registers.B != 0x80 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_NEG() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xED, 0x44 }; // NEG
	device->cpu->registers.A = 2;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 8);
	if (device->cpu->registers.A != 0xFE ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_SBC_HL_rp() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xED, 0x42 }; // SBC HL, BC
	device->cpu->registers.HL = 0x4000;
	device->cpu->registers.BC = 0x100;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 15);
	if (device->cpu->registers.HL != 0x3F00 ||
			device->cpu->registers.BC != 0x100 ||
			device->cpu->registers.flags.Z != 0 ||
			device->cpu->registers.flags.C != 0 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	device = asic_init(TI83p, NULL);
	device->cpu->registers.HL = 0x1000;
	device->cpu->registers.BC = 0x2000;
	device->cpu->registers.flags.C = 1;
	flash(device, test, sizeof(test));
	cycles = cpu_execute(device->cpu, 15);
	if (device->cpu->registers.HL != 0xEFFF ||
			device->cpu->registers.BC != 0x2000 ||
			device->cpu->registers.flags.Z != 0 ||
			device->cpu->registers.flags.C != 1 ||
			cycles != 0) {
		asic_free(device);
		return 2;
	}
	asic_free(device);
	return 0;
}

int test_ADC_HL_rp() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xED, 0x4A }; // ADC HL, BC
	device->cpu->registers.HL = 0x4000;
	device->cpu->registers.BC = 0x100;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 15);
	if (device->cpu->registers.HL != 0x4100 ||
			device->cpu->registers.BC != 0x100 ||
			device->cpu->registers.flags.Z != 0 ||
			device->cpu->registers.flags.C != 0 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	device = asic_init(TI83p, NULL);
	device->cpu->registers.HL = 0xF000;
	device->cpu->registers.BC = 0x2000;
	device->cpu->registers.flags.C = 1;
	flash(device, test, sizeof(test));
	cycles = cpu_execute(device->cpu, 15);
	if (device->cpu->registers.HL != 0x1001 ||
			device->cpu->registers.BC != 0x2000 ||
			device->cpu->registers.flags.Z != 0 ||
			device->cpu->registers.flags.C != 1 ||
			cycles != 0) {
		asic_free(device);
		return 2;
	}
	asic_free(device);
	return 0;
}
