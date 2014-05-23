int test_keyboard() {
    z80iodevice_t keyboard = init_keyboard();
    depress_key(keyboard, 0);
    keyboard.write_out(keyboard.device, 0xFE);
    uint8_t value = keyboard.read_in(keyboard.device);
    if (value != 0x01) {
        free_keyboard(keyboard);
        return 1;
    }
    depress_key(keyboard, 1);
    value = keyboard.read_in(keyboard.device);
    if (value != 0x03) {
        free_keyboard(keyboard);
        return 2;
    }
    depress_key(keyboard, 0x14);
    value = keyboard.read_in(keyboard.device);
    if (value != 0x03) {
        free_keyboard(keyboard);
        return 3;
    }
    keyboard.write_out(keyboard.device, 0xFC);
    value = keyboard.read_in(keyboard.device);
    if (value != 0x13) {
        free_keyboard(keyboard);
        return 4;
    }
    release_key(keyboard, 0x14);
    value = keyboard.read_in(keyboard.device);
    if (value != 0x03) {
        free_keyboard(keyboard);
        return 4;
    }
    return 0;
}

int test_status() {
    asic_t *asic = asic_init(TI83p);
    z80iodevice_t status = init_status(asic, TI83p);
    // Test battery status
    asic->battery = BATTERIES_GOOD;
    asic->battery_remove_check = 0;
    uint8_t value = status.read_in(status.device);
    if (!(value & 1)) {
        asic_free(asic);
        return 1;
    }
    // Test flash
    asic->mmu->flash_unlocked = 1;
    value = status.read_in(status.device);
    if (!(value & 4)) {
        asic_free(asic);
        return 1;
    }
    asic->mmu->flash_unlocked = 0;
    value = status.read_in(status.device);
    if (value & 4) {
        asic_free(asic);
        return 1;
    }
    asic_free(asic);
    return 0;
}
