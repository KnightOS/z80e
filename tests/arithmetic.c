int test_ADD_HL_rp() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0x09 }; // ADD HL, BC
    device->cpu->registers.HL = 0x1000;
    device->cpu->registers.BC = 0x0234;
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 1);
    if (device->cpu->registers.HL != 0x1234 ||
        cycles != -10) {
        asic_free(device);
        return 1;
    }
    asic_free(device);

    device = asic_init(TI83p);
    device->cpu->registers.HL = 0xF000;
    device->cpu->registers.BC = 0x1000;
    device->cpu->registers.flags.Z = 0;
    flash(device, test);
    cycles = cpu_execute(device->cpu, 1);
    if (device->cpu->registers.HL != 0 ||
        device->cpu->registers.flags.Z != 0 ||
        device->cpu->registers.flags.C != 1 ||
        cycles != -10) {
        asic_free(device);
        return 1;
    }
    asic_free(device);
    return 0;
}

int test_INC_rp() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0x23 }; // INC HL
    device->cpu->registers.HL = 0xFFFF;
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 1);
    if (device->cpu->registers.HL != 0 ||
        device->cpu->registers.flags.Z != 0 ||
        device->cpu->registers.flags.C != 0 ||
        cycles != -5) {
        asic_free(device);
        return 1;
    }
    return 0;
}

int test_INC_r() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0x3C }; // INC A
    device->cpu->registers.A = 0xFF;
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 1);
    if (device->cpu->registers.HL != 0 ||
        device->cpu->registers.flags.Z != 1 ||
        device->cpu->registers.flags.C != 0 ||
        cycles != -3) {
        asic_free(device);
        return 1;
    }
    return 0;
}

int test_DEC_rp() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0x2B }; // DEC HL
    device->cpu->registers.HL = 1;
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 1);
    if (device->cpu->registers.HL != 0 ||
        device->cpu->registers.flags.Z != 0 ||
        device->cpu->registers.flags.C != 0 ||
        cycles != -5) {
        asic_free(device);
        return 1;
    }
    return 0;
}

int test_DEC_r() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0x3D }; // INC A
    device->cpu->registers.A = 1;
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 1);
    if (device->cpu->registers.HL != 0 ||
        device->cpu->registers.flags.Z != 1 ||
        device->cpu->registers.flags.C != 0 ||
        cycles != -3) {
        asic_free(device);
        return 1;
    }
    return 0;
}
