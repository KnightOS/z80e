int test_RST() {
	uint8_t i;
	const uint8_t opcodes[] = { 0xC7, 0xCF, 0xD7, 0xDF, 0xE7, 0xEF, 0xF7, 0xFF };
	const uint8_t results[] = { 0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38 };
	for (i = 0; i < 8; i++) {
		asic_t *device = asic_init(TI83p, NULL);
		uint8_t test[] = { opcodes[i] };
		flash(device, test, sizeof(test));
		int cycles = cpu_execute(device->cpu, 11);
		if (device->cpu->registers.PC != results[i] ||
				cycles != 0) {
			asic_free(device);
			return 1;
		}
		asic_free(device);
	}
	return 0;
}

int test_DJNZ() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0x10, 0xFE }; // DJNZ $
	flash(device, test, sizeof(test));
	device->cpu->registers.B = 10;
	int cycles = cpu_execute(device->cpu, 13);
	if (device->cpu->registers.PC != 0 ||
			device->cpu->registers.B != 9 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	while (device->cpu->registers.B != 0) {
		cycles = cpu_execute(device->cpu, 1);
	}
	if (device->cpu->registers.PC != 2 || cycles != -7) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_JR() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0x18, 0x0E }; // JR 0x10
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 12);
	if (device->cpu->registers.PC != 0x10 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_JR_cc() {
	uint8_t test[] = { 0x28, 0x0E }; // JR Z, 0x10
	uint8_t test_nz[] = { 0x20, 0x0E }; // JR NZ, 0x10
	asic_t *device = asic_init(TI83p, NULL);
	device->cpu->registers.flags.Z = 0;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 7);
	if (device->cpu->registers.PC != 2 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	device = asic_init(TI83p, NULL);
	device->cpu->registers.flags.Z = 0;
	flash(device, test_nz, sizeof(test_nz));
	cycles = cpu_execute(device->cpu, 12);
	if (device->cpu->registers.PC != 0x10 || cycles != 0) {
		asic_free(device);
		return 2;
	}
	asic_free(device);
	return 0;
}

int test_RET_cc() {
	uint8_t test[] = { 0xC8 }; // RET Z
	uint8_t test_nz[] = { 0xC0 }; // RET NZ
	asic_t *device = asic_init(TI83p, NULL);
	device->cpu->registers.flags.Z = 0;
	device->cpu->registers.SP = 0x3FFE;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 5);
	if (device->cpu->registers.PC != 1 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	device = asic_init(TI83p, NULL);
	device->cpu->registers.flags.Z = 0;
	device->cpu->registers.SP = 0x3FFE;
	flash(device, test_nz, sizeof(test_nz));
	cycles = cpu_execute(device->cpu, 11);
	if (device->cpu->registers.PC != 0xFFFF || cycles != 0) {
		asic_free(device);
		return 2;
	}
	asic_free(device);
	return 0;
}

int test_RET() {
	uint8_t test[] = { 0xC9 }; // RET
	asic_t *device = asic_init(TI83p, NULL);
	device->cpu->registers.SP = 0x3FFE;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 10);
	if (device->cpu->registers.PC != 0xFFFF ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_JP_HL() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xE9 }; // JP (HL)
	device->cpu->registers.HL = 0x1234;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->registers.PC != 0x1234 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_LD_SP_HL() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xF9 }; // LD SP, HL
	device->cpu->registers.HL = 0x1234;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 6);
	if (device->cpu->registers.SP != 0x1234 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_JP_cc_nn() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xCA, 0x34, 0x12 }; // JP Z, 0x1234
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 10);
	if (device->cpu->registers.PC != 3 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	device = asic_init(TI83p, NULL);
	flash(device, test, sizeof(test));
	device->cpu->registers.flags.Z = 1;
	cycles = cpu_execute(device->cpu, 10);
	if (device->cpu->registers.PC != 0x1234 ||
			cycles != 0) {
		asic_free(device);
		return 2;
	}
	asic_free(device);
	return 0;
}

int test_JP_nn() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xC3, 0x34, 0x12 }; // JP 0x1234
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 10);
	if (device->cpu->registers.PC != 0x1234 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_CALL_cc_nn() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xCC, 0x34, 0x12 }; // CALL z, 0x1234
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 10);
	if (device->cpu->registers.PC != 3 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	device = asic_init(TI83p, NULL);
	device->cpu->registers.flags.Z = 1;
	flash(device, test, sizeof(test));
	cycles = cpu_execute(device->cpu, 17);
	if (device->cpu->registers.PC != 0x1234 ||
			device->cpu->registers.SP != 0xFFFE ||
			cycles != 0) {
		asic_free(device);
		return 2;
	}
	asic_free(device);
	return 0;
}

int test_CALL_nn() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xCD, 0x34, 0x12 }; // CALL 0x1234
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 17);
	if (device->cpu->registers.PC != 0x1234 ||
			device->cpu->registers.SP != 0xFFFE ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_IM_set() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xED, 0x5E }; // IM 2
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 8);
	if (device->cpu->int_mode != 2 ||
			device->cpu->registers.PC != 2 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_EI_DI() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xFB }; // EI
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->IFF1 != 1 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	device = asic_init(TI83p, NULL);
	uint8_t test2[] = { 0xF3 }; // DI
	device->cpu->IFF1 = 1;
	flash(device, test2, sizeof(test2));
	cycles = cpu_execute(device->cpu, 4);
	if (device->cpu->IFF1 != 0 ||
			cycles != 0) {
		asic_free(device);
		return 2;
	}
	asic_free(device);
	return 0;
}
