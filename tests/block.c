int test_LDI() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0xED, 0xA0 }; // LDI
    device->cpu->registers.HL = 0xC000;
    device->cpu->registers.DE = 0xD000;
    device->cpu->registers.BC = 5;
    cpu_write_byte(device->cpu, 0xC000, 0x11);
    cpu_write_byte(device->cpu, 0xC001, 0x22);
    cpu_write_byte(device->cpu, 0xC002, 0x33);
    cpu_write_byte(device->cpu, 0xC003, 0x44);
    cpu_write_byte(device->cpu, 0xC004, 0x55);
    flash(device, test);
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
