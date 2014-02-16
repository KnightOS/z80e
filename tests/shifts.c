int test_RLCA() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0x07 }; // RLCA
    device->cpu->registers.A = 0x80;
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 1);
    if (device->cpu->registers.A != 1 ||
        device->cpu->registers.flags.C != 1 ||
        cycles != -3) {
        asic_free(device);
        return 1;
    }
    return 0;
}

int test_RRCA() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0x0F }; // RRCA
    device->cpu->registers.A = 1;
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 1);
    if (device->cpu->registers.A != 0x80 ||
        device->cpu->registers.flags.C != 1 ||
        cycles != -3) {
        asic_free(device);
        return 1;
    }
    return 0;
}

int test_RLA() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0x17 }; // RLA
    device->cpu->registers.A = 0x80;
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 1);
    if (device->cpu->registers.A != 0 ||
        device->cpu->registers.flags.C != 1 ||
        cycles != -3) {
        asic_free(device);
        return 1;
    }
    return 0;
}

int test_RRA() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0x1F }; // RRA
    device->cpu->registers.A = 1;
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 1);
    if (device->cpu->registers.A != 0 ||
        device->cpu->registers.flags.C != 1 ||
        cycles != -3) {
        asic_free(device);
        return 1;
    }
    return 0;
}
