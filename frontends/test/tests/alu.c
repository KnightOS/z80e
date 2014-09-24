int test_ADD_A_r() {
	uint8_t test[] = { 0x80 }; // ADD A, B
	uint8_t test_hl[] = { 0x86 }; // ADD A, (HL)

	asic_t *device = asic_init(TI83p, NULL);
	device->cpu->registers.A = 0x10;
	device->cpu->registers.B = 0x20;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->registers.A != 0x30 ||
			device->cpu->registers.B != 0x20 ||
			device->cpu->registers.flags.Z != 0 ||
			device->cpu->registers.flags.C != 0 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	device = asic_init(TI83p, NULL);
	device->cpu->registers.A = 0xF0;
	device->cpu->registers.B = 0x20;
	flash(device, test, sizeof(test));
	cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->registers.A != 0x10 ||
			device->cpu->registers.flags.Z != 0 ||
			device->cpu->registers.flags.C != 1 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	device = asic_init(TI83p, NULL);
	device->cpu->registers.A = 0xF0;
	device->cpu->registers.B = 0x10;
	flash(device, test, sizeof(test));
	cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->registers.A != 0 ||
			device->cpu->registers.flags.Z != 1 ||
			device->cpu->registers.flags.C != 1 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	device = asic_init(TI83p, NULL);
	device->cpu->registers.A = 0x10;
	device->cpu->registers.HL = 0x1000;
	mmu_force_write(device->mmu, 0x1000, 0x20);
	flash(device, test_hl, sizeof(test_hl));
	cycles = cpu_execute(device->cpu, 7);
	if (device->cpu->registers.A != 0x30 || cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_ADC_A_r() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0x88 }; // ADC A, B
	device->cpu->registers.A = 0x10;
	device->cpu->registers.B = 0x20;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->registers.A != 0x30 ||
			device->cpu->registers.B != 0x20 ||
			device->cpu->registers.flags.Z != 0 ||
			device->cpu->registers.flags.C != 0 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	device = asic_init(TI83p, NULL);
	device->cpu->registers.A = 0x10;
	device->cpu->registers.B = 0x20;
	device->cpu->registers.flags.C = 1;
	flash(device, test, sizeof(test));
	cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->registers.A != 0x31 ||
			device->cpu->registers.B != 0x20 ||
			device->cpu->registers.flags.Z != 0 ||
			device->cpu->registers.flags.C != 0 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_SUB_A_r() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0x90 }; // SUB A, B
	device->cpu->registers.A = 0x20;
	device->cpu->registers.B = 0x10;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->registers.A != 0x10 ||
			device->cpu->registers.B != 0x10 ||
			device->cpu->registers.flags.Z != 0 ||
			device->cpu->registers.flags.C != 0 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	device = asic_init(TI83p, NULL);
	device->cpu->registers.A = 0x10;
	device->cpu->registers.B = 0x20;
	flash(device, test, sizeof(test));
	cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->registers.A != 0xF0 ||
			device->cpu->registers.B != 0x20 ||
			device->cpu->registers.flags.Z != 0 ||
			device->cpu->registers.flags.C != 1 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_SBC_A_r() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0x98 }; // SBC A, B
	device->cpu->registers.A = 0x20;
	device->cpu->registers.B = 0x10;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->registers.A != 0x10 ||
			device->cpu->registers.B != 0x10 ||
			device->cpu->registers.flags.Z != 0 ||
			device->cpu->registers.flags.C != 0 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	device = asic_init(TI83p, NULL);
	device->cpu->registers.A = 0x10;
	device->cpu->registers.B = 0x20;
	device->cpu->registers.flags.C = 1;
	flash(device, test, sizeof(test));
	cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->registers.A != 0xEF ||
			device->cpu->registers.B != 0x20 ||
			device->cpu->registers.flags.Z != 0 ||
			device->cpu->registers.flags.C != 1 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_AND_A_r() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xA0 }; // AND A, B
	device->cpu->registers.A = 0xFF;
	device->cpu->registers.B = 0x0F;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->registers.A != 0x0F ||
			device->cpu->registers.flags.Z != 0 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_XOR_A_r() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xA8 }; // XOR A, B
	device->cpu->registers.A = 0xFF;
	device->cpu->registers.B = 0x0F;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->registers.A != 0xF0 ||
			device->cpu->registers.flags.Z != 0 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_OR_A_r() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xB0 }; // OR A, B
	device->cpu->registers.A = 0x00;
	device->cpu->registers.B = 0x0F;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->registers.A != 0x0F ||
			device->cpu->registers.flags.Z != 0 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_CP_r() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xB8 }; // CP B
	device->cpu->registers.A = 0x00;
	device->cpu->registers.B = 0x10;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->registers.A != 0 ||
			device->cpu->registers.flags.S != 1 ||
			device->cpu->registers.flags.C != 1 ||
			device->cpu->registers.flags.PV != 0 ||
			device->cpu->registers.flags.N != 1 ||
			device->cpu->registers.flags.Z != 0 ||
			cycles != 0) {
		print_state(&device->cpu->registers);
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_alu_n() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xC6, 0x20 }; // ADD A, 0x20
	device->cpu->registers.A = 0x10;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->registers.A != 0x30 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}
