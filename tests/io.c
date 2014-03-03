uint8_t test_read(void *device) {
    return *(uint8_t *)device;
}

void test_write(void *device, uint8_t value) {
    *(uint8_t *)device = value;
}

int test_OUT_n_A() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0xD3, 0x12 }; // OUT (0x12), A
    device->cpu->registers.A = 0x3C;
    flash(device, test);
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
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0xDB, 0x12 }; // IN A, (0x12)
    flash(device, test);
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
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0xED, 0x70 }; // IN (C)
    flash(device, test);
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
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0xED, 0x40 }; // IN B, (C)
    flash(device, test);
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
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0xED, 0x71 }; // OUT (C), 0
    uint8_t value;
    z80iodevice_t test_device = { &value, test_read, test_write };
    device->cpu->devices[0x12] = test_device;
    device->cpu->registers.C = 0x12;
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 12);
    if (value != 0xFF || cycles != 0) {
        asic_free(device);
        return 1;
    }
    asic_free(device);
    return 0;
}

int test_OUT_C_r() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0xED, 0x41 }; // OUT (C), B
    uint8_t value;
    z80iodevice_t test_device = { &value, test_read, test_write };
    device->cpu->devices[0x12] = test_device;
    device->cpu->registers.B = 0xB2;
    device->cpu->registers.C = 0x12;
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 12);
    if (value != 0xB2 || cycles != 0) {
        asic_free(device);
        return 1;
    }
    asic_free(device);
    return 0;
}
