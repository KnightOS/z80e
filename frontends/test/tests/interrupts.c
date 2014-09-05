int test_IM_1() {
	uint8_t test[] = { 0xED, 0x56, 0xFB, 0x18, 0xFE }; // IM 1 \ EI \ JR $
	asic_t *device = asic_init(TI83p, NULL);
	flash(device, test, sizeof(test));
	cpu_execute(device->cpu, 100);
	if (device->cpu->registers.PC != 3) {
		asic_free(device);
		return 1;
	}
	device->cpu->interrupt = 1;
	int cycles = cpu_execute(device->cpu, 13);
	if (device->cpu->registers.PC != 0x38 ||
			device->cpu->registers.SP != 0xFFFE ||
			cycles != 0) {
		asic_free(device);
		return 1;
	}
	return 0;
}

int test_IM_2() {
	uint8_t test[] = { 0xED, 0x5E, 0x3E, 0x80, 0xED, 0x47, 0xFB, 0x18, 0xFE }; // IM 2 \ LD A, 0x80 \ LD I, A \ EI \ JR $
	asic_t *device = asic_init(TI83p, NULL);
	flash(device, test, sizeof(test));
	cpu_execute(device->cpu, 100);
	if (device->cpu->registers.PC != 7 ||
			device->cpu->registers.I != 0x80) {
		asic_free(device);
		return 1;
	}
	device->cpu->bus = 0x90;
	device->cpu->interrupt = 1;
	int cycles = cpu_execute(device->cpu, 19);
	if (device->cpu->registers.PC != 0x8090 ||
			device->cpu->registers.SP != 0xFFFE ||
			cycles != 0) {
		asic_free(device);
		return 2;
	}
	return 0;
}
