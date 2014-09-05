uint8_t test_read(void* device) {
	return *(uint8_t*)device;
}

void test_write(void* device, uint8_t value) {
	*(uint8_t*)device = value;
}

int test_OUT_n_A() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xD3, 0x12 }; // OUT (0x12), A
	device->cpu->registers.A = 0x3C;
	flash(device, test, sizeof(test));
	uint8_t value = 0;
	z80iodevice_t test_device = { &value, test_read, test_write };
	device->cpu->devices[0x12] = test_device;
	int cycles = cpu_execute(device->cpu, 11);
	if (value != 0x3C ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_IN_A_n() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xDB, 0x12 }; // IN A, (0x12)
	flash(device, test, sizeof(test));
	uint8_t value = 0x3C;
	z80iodevice_t test_device = { &value, test_read, test_write };
	device->cpu->devices[0x12] = test_device;
	int cycles = cpu_execute(device->cpu, 11);
	if (device->cpu->registers.A != 0x3C ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_IN_C() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xED, 0x70 }; // IN (C)
	flash(device, test, sizeof(test));
	uint8_t value = 0;
	z80iodevice_t test_device = { &value, test_read, test_write };
	device->cpu->devices[0x12] = test_device;
	device->cpu->registers.C = 0x12;
	int cycles = cpu_execute(device->cpu, 12);
	if (device->cpu->registers.flags.Z != 1 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_IN_r_C() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xED, 0x40 }; // IN B, (C)
	flash(device, test, sizeof(test));
	uint8_t value = 0x5B;
	z80iodevice_t test_device = { &value, test_read, test_write };
	device->cpu->devices[0x12] = test_device;
	device->cpu->registers.C = 0x12;
	int cycles = cpu_execute(device->cpu, 12);
	if (device->cpu->registers.B != 0x5B ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_OUT_C_0() {
	// Note: Despite the mnomic of this instruction, the
	// CMOS z80 variant (which TI calculators use) output
	// 0xFF instead.
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xED, 0x71 }; // OUT (C), 0
	uint8_t value;
	z80iodevice_t test_device = { &value, test_read, test_write };
	device->cpu->devices[0x12] = test_device;
	device->cpu->registers.C = 0x12;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 12);
	if (value != 0xFF || cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_OUT_C_r() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xED, 0x41 }; // OUT (C), B
	uint8_t value;
	z80iodevice_t test_device = { &value, test_read, test_write };
	device->cpu->devices[0x12] = test_device;
	device->cpu->registers.B = 0xB2;
	device->cpu->registers.C = 0x12;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 12);
	if (value != 0xB2 || cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_INI() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xED, 0xA2 }; // INI
	uint8_t value = 0x3E;
	z80iodevice_t test_device = { &value, test_read, test_write };
	device->cpu->devices[0x12] = test_device;
	device->cpu->registers.B = 1;
	device->cpu->registers.HL = 0xC000;
	device->cpu->registers.C = 0x12;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 16);
	if (cpu_read_byte(device->cpu, 0xC000) != 0x3E ||
			device->cpu->registers.B != 0 ||
			device->cpu->registers.HL != 0xC001 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_IND() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xED, 0xAA }; // IND
	uint8_t value = 0x3E;
	z80iodevice_t test_device = { &value, test_read, test_write };
	device->cpu->devices[0x12] = test_device;
	device->cpu->registers.B = 1;
	device->cpu->registers.HL = 0xC000;
	device->cpu->registers.C = 0x12;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 16);
	if (cpu_read_byte(device->cpu, 0xC000) != 0x3E ||
			device->cpu->registers.B != 0 ||
			device->cpu->registers.HL != 0xBFFF ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_INIR() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xED, 0xB2 }; // INIR
	uint8_t value = 0x3E;
	z80iodevice_t test_device = { &value, test_read, test_write };
	device->cpu->devices[0x12] = test_device;
	device->cpu->registers.B = 5;
	device->cpu->registers.HL = 0xC000;
	device->cpu->registers.C = 0x12;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 100);
	if (cpu_read_byte(device->cpu, 0xC000) != 0x3E ||
			cpu_read_byte(device->cpu, 0xC001) != 0x3E ||
			cpu_read_byte(device->cpu, 0xC002) != 0x3E ||
			cpu_read_byte(device->cpu, 0xC003) != 0x3E ||
			cpu_read_byte(device->cpu, 0xC004) != 0x3E ||
			device->cpu->registers.B != 0 ||
			device->cpu->registers.HL != 0xC005 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_INDR() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xED, 0xBA }; // INDR
	uint8_t value = 0x3E;
	z80iodevice_t test_device = { &value, test_read, test_write };
	device->cpu->devices[0x12] = test_device;
	device->cpu->registers.B = 5;
	device->cpu->registers.HL = 0xC004;
	device->cpu->registers.C = 0x12;
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 100);
	if (cpu_read_byte(device->cpu, 0xC000) != 0x3E ||
			cpu_read_byte(device->cpu, 0xC001) != 0x3E ||
			cpu_read_byte(device->cpu, 0xC002) != 0x3E ||
			cpu_read_byte(device->cpu, 0xC003) != 0x3E ||
			cpu_read_byte(device->cpu, 0xC004) != 0x3E ||
			device->cpu->registers.B != 0 ||
			device->cpu->registers.HL != 0xBFFF ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_OUTI() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xED, 0xA3 };
	uint8_t value = 0x00;
	z80iodevice_t test_device = { &value, test_read, test_write };
	device->cpu->devices[0x12] = test_device;
	device->cpu->registers.C = 0x12;
	device->cpu->registers.B = 5;
	device->cpu->registers.HL = 0xC000;
	cpu_write_byte(device->cpu, 0xC000, 0x11);
	cpu_write_byte(device->cpu, 0xC001, 0x22);
	cpu_write_byte(device->cpu, 0xC002, 0x33);
	cpu_write_byte(device->cpu, 0xC003, 0x44);
	cpu_write_byte(device->cpu, 0xC004, 0x55);
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 16);
	if (value != 0x11 ||
			device->cpu->registers.B != 4 ||
			device->cpu->registers.HL != 0xC001 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_OUTD() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xED, 0xAB };
	uint8_t value = 0x00;
	z80iodevice_t test_device = { &value, test_read, test_write };
	device->cpu->devices[0x12] = test_device;
	device->cpu->registers.C = 0x12;
	device->cpu->registers.B = 5;
	device->cpu->registers.HL = 0xC004;
	cpu_write_byte(device->cpu, 0xC000, 0x11);
	cpu_write_byte(device->cpu, 0xC001, 0x22);
	cpu_write_byte(device->cpu, 0xC002, 0x33);
	cpu_write_byte(device->cpu, 0xC003, 0x44);
	cpu_write_byte(device->cpu, 0xC004, 0x55);
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 16);
	if (value != 0x55 ||
			device->cpu->registers.B != 4 ||
			device->cpu->registers.HL != 0xC003 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_OTIR() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xED, 0xB3 };
	uint8_t value = 0x00;
	z80iodevice_t test_device = { &value, test_read, test_write };
	device->cpu->devices[0x12] = test_device;
	device->cpu->registers.C = 0x12;
	device->cpu->registers.B = 5;
	device->cpu->registers.HL = 0xC000;
	cpu_write_byte(device->cpu, 0xC000, 0x11);
	cpu_write_byte(device->cpu, 0xC001, 0x22);
	cpu_write_byte(device->cpu, 0xC002, 0x33);
	cpu_write_byte(device->cpu, 0xC003, 0x44);
	cpu_write_byte(device->cpu, 0xC004, 0x55);
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 100);
	if (value != 0x55 ||
			device->cpu->registers.B != 0 ||
			device->cpu->registers.HL != 0xC005 ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}

int test_OTDR() {
	asic_t *device = asic_init(TI83p, NULL);
	uint8_t test[] = { 0xED, 0xBB };
	uint8_t value = 0x00;
	z80iodevice_t test_device = { &value, test_read, test_write };
	device->cpu->devices[0x12] = test_device;
	device->cpu->registers.C = 0x12;
	device->cpu->registers.B = 5;
	device->cpu->registers.HL = 0xC004;
	cpu_write_byte(device->cpu, 0xC000, 0x11);
	cpu_write_byte(device->cpu, 0xC001, 0x22);
	cpu_write_byte(device->cpu, 0xC002, 0x33);
	cpu_write_byte(device->cpu, 0xC003, 0x44);
	cpu_write_byte(device->cpu, 0xC004, 0x55);
	flash(device, test, sizeof(test));
	int cycles = cpu_execute(device->cpu, 100);
	if (value != 0x11 ||
			device->cpu->registers.B != 0 ||
			device->cpu->registers.HL != 0xBFFF ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	asic_free(device);
	return 0;
}
