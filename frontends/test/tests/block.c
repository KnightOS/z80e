int test_LDI() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xED, 0xA0 }; // LDI
	device->cpu->registers.HL = 0xC000;
	device->cpu->registers.DE = 0xD000;
	device->cpu->registers.BC = 5;
	cpu_write_byte(device->cpu, 0xC000, 0x11);
	cpu_write_byte(device->cpu, 0xC001, 0x22);
	cpu_write_byte(device->cpu, 0xC002, 0x33);
	cpu_write_byte(device->cpu, 0xC003, 0x44);
	cpu_write_byte(device->cpu, 0xC004, 0x55);
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 16);
	if (cpu_read_byte(device->cpu, 0xD000) != 0x11 ||
			device->cpu->registers.HL != 0xC001 ||
			device->cpu->registers.DE != 0xD001 ||
			device->cpu->registers.BC != 4 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_LDIR() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xED, 0xB0 }; // LDIR
	device->cpu->registers.HL = 0xC000;
	device->cpu->registers.DE = 0xD000;
	device->cpu->registers.BC = 5;
	cpu_write_byte(device->cpu, 0xC000, 0x11);
	cpu_write_byte(device->cpu, 0xC001, 0x22);
	cpu_write_byte(device->cpu, 0xC002, 0x33);
	cpu_write_byte(device->cpu, 0xC003, 0x44);
	cpu_write_byte(device->cpu, 0xC004, 0x55);
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 100);
	if (cpu_read_byte(device->cpu, 0xD000) != 0x11 ||
			cpu_read_byte(device->cpu, 0xD001) != 0x22 ||
			cpu_read_byte(device->cpu, 0xD002) != 0x33 ||
			cpu_read_byte(device->cpu, 0xD003) != 0x44 ||
			cpu_read_byte(device->cpu, 0xD004) != 0x55 ||
			device->cpu->registers.HL != 0xC005 ||
			device->cpu->registers.DE != 0xD005 ||
			device->cpu->registers.BC != 0 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_LDD() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xED, 0xA8 }; // LDD
	device->cpu->registers.HL = 0xC004;
	device->cpu->registers.DE = 0xD004;
	device->cpu->registers.BC = 5;
	cpu_write_byte(device->cpu, 0xC000, 0x11);
	cpu_write_byte(device->cpu, 0xC001, 0x22);
	cpu_write_byte(device->cpu, 0xC002, 0x33);
	cpu_write_byte(device->cpu, 0xC003, 0x44);
	cpu_write_byte(device->cpu, 0xC004, 0x55);
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 16);
	if (cpu_read_byte(device->cpu, 0xD004) != 0x55 ||
			device->cpu->registers.HL != 0xC003 ||
			device->cpu->registers.DE != 0xD003 ||
			device->cpu->registers.BC != 4 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_LDDR() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xED, 0xB8 }; // LDDR
	device->cpu->registers.HL = 0xC004;
	device->cpu->registers.DE = 0xD004;
	device->cpu->registers.BC = 5;
	cpu_write_byte(device->cpu, 0xC000, 0x11);
	cpu_write_byte(device->cpu, 0xC001, 0x22);
	cpu_write_byte(device->cpu, 0xC002, 0x33);
	cpu_write_byte(device->cpu, 0xC003, 0x44);
	cpu_write_byte(device->cpu, 0xC004, 0x55);
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 100);
	if (cpu_read_byte(device->cpu, 0xD000) != 0x11 ||
			cpu_read_byte(device->cpu, 0xD001) != 0x22 ||
			cpu_read_byte(device->cpu, 0xD002) != 0x33 ||
			cpu_read_byte(device->cpu, 0xD003) != 0x44 ||
			cpu_read_byte(device->cpu, 0xD004) != 0x55 ||
			device->cpu->registers.HL != 0xBFFF ||
			device->cpu->registers.DE != 0xCFFF ||
			device->cpu->registers.BC != 0 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_CPI() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xED, 0xA1 }; // CPI
	device->cpu->registers.HL = 0xC000;
	device->cpu->registers.BC = 5;
	device->cpu->registers.A = 0x33;
	cpu_write_byte(device->cpu, 0xC000, 0x11);
	cpu_write_byte(device->cpu, 0xC001, 0x22);
	cpu_write_byte(device->cpu, 0xC002, 0x33);
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 16);
	if (device->cpu->registers.flags.Z != 0 ||
			device->cpu->registers.HL != 0xC001 ||
			device->cpu->registers.BC != 4 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	device->cpu->registers.PC = 0;
	cpu_execute(device->cpu, 16);
	device->cpu->registers.PC = 0;
	cpu_execute(device->cpu, 16);
	if (device->cpu->registers.flags.Z != 1 ||
			device->cpu->registers.HL != 0xC003 ||
			device->cpu->registers.BC != 2) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_CPD() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xED, 0xA9 }; // CPD
	device->cpu->registers.HL = 0xC002;
	device->cpu->registers.BC = 5;
	device->cpu->registers.A = 0x11;
	cpu_write_byte(device->cpu, 0xC000, 0x11);
	cpu_write_byte(device->cpu, 0xC001, 0x22);
	cpu_write_byte(device->cpu, 0xC002, 0x33);
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 16);
	if (device->cpu->registers.flags.Z != 0 ||
			device->cpu->registers.HL != 0xC001 ||
			device->cpu->registers.BC != 4 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	device->cpu->registers.PC = 0;
	cpu_execute(device->cpu, 16);
	device->cpu->registers.PC = 0;
	cpu_execute(device->cpu, 16);
	if (device->cpu->registers.flags.Z != 1 ||
			device->cpu->registers.HL != 0xBFFF ||
			device->cpu->registers.BC != 2) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_CPIR() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xED, 0xB1 }; // CPIR
	device->cpu->registers.HL = 0xC000;
	device->cpu->registers.BC = 5;
	device->cpu->registers.A = 0x33;
	cpu_write_byte(device->cpu, 0xC000, 0x11);
	cpu_write_byte(device->cpu, 0xC001, 0x22);
	cpu_write_byte(device->cpu, 0xC002, 0x33);
	cpu_write_byte(device->cpu, 0xC003, 0x44);
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 58);
	if (device->cpu->registers.HL != 0xC003 ||
			device->cpu->registers.BC != 2 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_CPDR() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xED, 0xB9 }; // CPDR
	device->cpu->registers.HL = 0xC004;
	device->cpu->registers.BC = 5;
	device->cpu->registers.A = 0x33;
	cpu_write_byte(device->cpu, 0xC001, 0x22);
	cpu_write_byte(device->cpu, 0xC002, 0x33);
	cpu_write_byte(device->cpu, 0xC003, 0x44);
	cpu_write_byte(device->cpu, 0xC004, 0x55);
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 58);
	if (device->cpu->registers.HL != 0xC001 ||
			device->cpu->registers.BC != 2 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

// Note: INI, INIR, IND, INDR, OUTI, OUTD, OTIR, and OTDR are tested in io.c
