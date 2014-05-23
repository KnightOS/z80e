int test_keyboard() {
    z80iodevice_t keyboard = init_keyboard();
    depress_key(keyboard, 0);
    keyboard.write_out(keyboard.device, 0xFE);
    uint8_t value = keyboard.read_in(keyboard.device);
    if (value != 0x01) {
        return 1;
    }
    depress_key(keyboard, 1);
    value = keyboard.read_in(keyboard.device);
    if (value != 0x03) {
        return 2;
    }
    depress_key(keyboard, 0x14);
    value = keyboard.read_in(keyboard.device);
    if (value != 0x03) {
        return 3;
    }
    keyboard.write_out(keyboard.device, 0xFC);
    value = keyboard.read_in(keyboard.device);
    if (value != 0x13) {
        return 4;
    }
    release_key(keyboard, 0x14);
    value = keyboard.read_in(keyboard.device);
    if (value != 0x03) {
        return 4;
    }
    return 0;
}
