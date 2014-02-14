int test_RST() {
    uint8_t i;
    const uint8_t opcodes[] = { 0xC7, 0xCF, 0xD7, 0xDF, 0xE7, 0xEF, 0xF7, 0xFF };
    const uint8_t results[] = { 0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38 };
    for (i = 0; i < 8; i++) {
        asic_t *device = asic_init(TI83p);
        uint8_t test[] = { opcodes[i] };
        flash(device, test);
        int cycles = cpu_execute(device->cpu, 1);
        if (device->cpu->registers.PC != results[i] ||
            cycles != -10) {
            asic_free(device);
            return 1;
        }
        asic_free(device);
    }
    return 0;
}
