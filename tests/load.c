int test_LD_rp_nn() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0x21, 0x34, 0x12 }; // LD HL, 0x1234
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 10);
    if (device->cpu->registers.HL != 0x1234 ||
        cycles != 0) {
        asic_free(device);
        return 1;
    }
    asic_free(device);
    return 0;
}

int test_LD_BCptr_A() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0x02 }; // LD (BC), A
    device->cpu->registers.BC = 0xC000;
    device->cpu->registers.A = 0x2F;
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 7);
    if (cpu_read_byte(device->cpu, 0xC000) != 0x2F ||
        cycles != 0) {
        asic_free(device);
        return 1;
    }
    asic_free(device);
    return 0;
}

int test_LD_nnptr_HL() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0x22, 0x00, 0xC0 }; // LD (0xC000), HL
    device->cpu->registers.HL = 0x1234;
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 16);
    if (cpu_read_word(device->cpu, 0xC000) != 0x1234 ||
        cycles != 0) {
        asic_free(device);
        return 1;
    }
    asic_free(device);
    return 0;
}

int test_LD_nnptr_A() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0x32, 0x00, 0xC0 }; // LD (0xC000), A
    device->cpu->registers.A = 0x4F;
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 13);
    if (cpu_read_word(device->cpu, 0xC000) != 0x4F ||
        cycles != 0) {
        asic_free(device);
        return 1;
    }
    asic_free(device);
    return 0;
}

int test_LD_A_BCptr() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0x0A }; // LD (BC), A
    device->cpu->registers.BC = 0xC000;
    cpu_write_byte(device->cpu, 0xC000, 0x2F);
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 7);
    if (device->cpu->registers.A != 0x2F ||
        cycles != 0) {
        asic_free(device);
        return 1;
    }
    asic_free(device);
    return 0;
}

int test_LD_HL_nnptr() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0x2A, 0x00, 0xC0 }; // LD HL, (0xC000)
    cpu_write_word(device->cpu, 0xC000, 0x1234);
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 16);
    if (device->cpu->registers.HL != 0x1234 ||
        cycles != 0) {
        asic_free(device);
        return 1;
    }
    asic_free(device);
    return 0;
}

int test_LD_A_nnptr() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0x3A, 0x00, 0xC0 }; // LD A, (0xC000)
    cpu_write_byte(device->cpu, 0xC000, 0x4F);
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 13);
    if (device->cpu->registers.A != 0x4F ||
        cycles != 0) {
        asic_free(device);
        return 1;
    }
    asic_free(device);
    return 0;
}

int test_LD_r_n() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0x06, 0x2F }; // LD B, 0x2F
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 7);
    if (device->cpu->registers.B != 0x2F ||
        cycles != 0) {
        asic_free(device);
        return 1;
    }
    asic_free(device);
    return 0;
}

int test_LD_r_r() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0x78 }; // LD A, B
    device->cpu->registers.B = 0x4F;
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 4);
    if (device->cpu->registers.A != 0x4F ||
        cycles != 0) {
        asic_free(device);
        return 1;
    }
    asic_free(device);
    return 0;
}

int test_POP_rp2() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0xC1 }; // POP BC
    device->cpu->registers.SP = 0xC000;
    cpu_write_byte(device->cpu, 0xC000, 0x34);
    cpu_write_byte(device->cpu, 0xC001, 0x12);
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 10);
    if (device->cpu->registers.BC != 0x1234 ||
        device->cpu->registers.SP != 0xC002 ||
        cycles != 0) {
        asic_free(device);
        return 1;
    }
    asic_free(device);
    return 0;
}

int test_EXX() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0xD9 }; 
    device->cpu->registers.BC = 0x1111;
    device->cpu->registers._BC = 0x2222;
    device->cpu->registers.DE = 0x3333;
    device->cpu->registers._DE = 0x4444;
    device->cpu->registers.HL = 0x5555;
    device->cpu->registers._HL = 0x6666;
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 4);
    if (device->cpu->registers.BC != 0x2222 ||
        device->cpu->registers._BC != 0x1111 ||
        device->cpu->registers.DE != 0x4444 ||
        device->cpu->registers._DE != 0x3333 ||
        device->cpu->registers.HL != 0x6666 ||
        device->cpu->registers._HL != 0x5555 ||
        cycles != 0) {
        asic_free(device);
        return 1;
    }
    asic_free(device);
    return 0;
}

int test_EX_SP_HL() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0xE3 }; // EX (SP), HL
    device->cpu->registers.HL = 0xDEAD;
    device->cpu->registers.SP = 0xC000;
    cpu_write_word(device->cpu, 0xC000, 0xBEEF);
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 19);
    if (device->cpu->registers.HL != 0xBEEF ||
        cpu_read_word(device->cpu, 0xC000) != 0xDEAD ||
        cycles != 0) {
        asic_free(device);
        return 1;
    }
    asic_free(device);
    return 0;
}

int test_EX_DE_HL() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0xEB }; // EX DE, HL
    device->cpu->registers.HL = 0xDEAD;
    device->cpu->registers.DE = 0xBEEF;
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 4);
    if (device->cpu->registers.HL != 0xBEEF ||
        device->cpu->registers.DE != 0xDEAD ||
        cycles != 0) {
        asic_free(device);
        return 1;
    }
    asic_free(device);
    return 0;
}

int test_PUSH_rp2() {
    asic_t *device = asic_init(TI83p);
    uint8_t test[] = { 0xD5 }; // PUSH DE
    device->cpu->registers.DE = 0x1234;
    flash(device, test);
    int cycles = cpu_execute(device->cpu, 11);
    if (device->cpu->registers.SP != 0xFFFE ||
        cpu_read_word(device->cpu, 0xFFFE) != 0x1234 ||
        cycles != 0) {
        asic_free(device);
        return 1;
    }
    asic_free(device);
    return 0;
}
