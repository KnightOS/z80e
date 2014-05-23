int test_keyboard() {
    z80iodevice_t keyboard = init_keyboard();
    depress_key(keyboard, 0);
    keyboard.write_out(keyboard.device, 0xFE);
    uint8_t value = keyboard.read_in(keyboard.device);
    printf("%02X\n", value);
    return 0;
}
