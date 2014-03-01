int test_JP_IX__JP_IY() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0xDD, 0xE9 }; // JP (IX)
    device->cpu->registers.IX = 0x1234;
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 8);
    if (device->cpu->registers.PC != 0x1234 ||
        cycles != 0) {
        asic_free(device);
        return 1;
    }
    asic_free(device);

    device = asic_init(TI83p);
    uint8_t test_2[] = { 0xFD, 0xE9 }; // JP (IY)
    device->cpu->registers.IY = 0x1234;
    flash(device, test_2);
    cycles = cpu_execute(device->cpu, 8);
    if (device->cpu->registers.PC != 0x1234 ||
        cycles != 0) {
        asic_free(device);
        return 2;
    }
    asic_free(device);
    return 0;
}

int test_ADD_IX_rp() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0xDD, 0x09 }; // ADD IX, BC
    device->cpu->registers.IX = 0x1000;
    device->cpu->registers.BC = 0x0234;
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 15);
    if (device->cpu->registers.IX != 0x1234 ||
        cycles != 0) {
        asic_free(device);
        return 1;
    }
    asic_free(device);

    device = asic_init(TI83p);
    device->cpu->registers.IX = 0xF000;
    device->cpu->registers.BC = 0x1000;
    device->cpu->registers.flags.Z = 0;
    flash(device, test);
    cycles = cpu_execute(device->cpu, 15);
    if (device->cpu->registers.IX != 0 ||
        device->cpu->registers.flags.Z != 0 ||
        device->cpu->registers.flags.C != 1 ||
        cycles != 0) {
        asic_free(device);
        return 1;
    }
    asic_free(device);
    return 0;
}

int test_prefix_reset() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0xDD, 0x09, 0x09 }; // ADD IX, BC \ ADD HL, BC
    device->cpu->registers.IX = 0x1000;
    device->cpu->registers.HL = 0x2000;
    device->cpu->registers.BC = 0x0234;
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 26);
    if (device->cpu->registers.IX != 0x1234 ||
        device->cpu->registers.HL != 0x2234 ||
        cycles != 0) {
        asic_free(device);
        return 1;
    }
    asic_free(device);
    return 0;
}
