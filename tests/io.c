uint8_t test_read(void* device) {
    return *(uint8_t*)device;
}

void test_write(void* device, uint8_t value) {
    *(uint8_t*)device = value;
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
