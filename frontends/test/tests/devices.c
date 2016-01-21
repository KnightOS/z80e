int test_keyboard() {
	z80iodevice_t keyboard = init_keyboard();
	depress_key(keyboard.device, 0);
	keyboard.write_out(keyboard.device, 0xFE);
	uint8_t value = keyboard.read_in(keyboard.device);
	if (value != 0xFE) {
		free_keyboard(keyboard.device);
		return 1;
	}
	depress_key(keyboard.device, 1);
	value = keyboard.read_in(keyboard.device);
	if (value != 0xFC) {
		free_keyboard(keyboard.device);
		return 2;
	}
	depress_key(keyboard.device, 0x14);
	value = keyboard.read_in(keyboard.device);
	if (value != 0xFC) {
		free_keyboard(keyboard.device);
		return 3;
	}
	keyboard.write_out(keyboard.device, 0xFC);
	value = keyboard.read_in(keyboard.device);
	if (value != 0xEC) {
		free_keyboard(keyboard.device);
		return 4;
	}
	release_key(keyboard.device, 0x14);
	value = keyboard.read_in(keyboard.device);
	if (value != 0xFC) {
		free_keyboard(keyboard.device);
		return 4;
	}
	return 0;
}

int test_memorymapping_83p() {
	asic_t *asic = asic_init(TI83p, NULL);
	memory_mapping_state_t *state = asic->cpu->devices[0x04].device;

	state->bank_a_page = 0;
	state->bank_a_flash = 0;

	reload_mapping(state);
	ti_write_byte(asic->mmu, 0x4000, 0x12);

	if (asic->mmu->ram[0] != 0x12) {
		asic_free(asic);
		return 1;
	}

	if (ti_read_byte(asic->mmu, 0xC000) != 0x12) {
		asic_free(asic);
		return 2;
	}

	state->map_mode = 1;
	state->bank_a_page = 1;
	state->bank_a_flash = 0;
	state->bank_b_page = 0;
	state->bank_b_flash = 0;
	reload_mapping(state);

	if (ti_read_byte(asic->mmu, 0x4000) != 0x12) {
		asic_free(asic);
		return 3;
	}

	asic->mmu->ram[0x4000] = 0x34;
	if (ti_read_byte(asic->mmu, 0x8000) != 0x34) {
		asic_free(asic);
		return 4;
	}

	if (ti_read_byte(asic->mmu, 0xC000) != 0x12) {
		asic_free(asic);
		return 5;
	}
	return 0;
}

int test_memorymapping_others() {
	asic_t *asic = asic_init(TI84p, NULL);
	memory_mapping_state_t *state = asic->cpu->devices[0x04].device;


	state->ram_bank_page = 1;
	state->bank_a_page = 0;
	state->bank_a_flash = 0;

	reload_mapping(state);
	ti_write_byte(asic->mmu, 0x4000, 0x12);

	if (asic->mmu->ram[0] != 0x12) {
		asic_free(asic);
		return 1;
	}

	asic->mmu->ram[0x4000] = 0x34;
	if (ti_read_byte(asic->mmu, 0xC000) != 0x34) {
		asic_free(asic);
		return 2;
	}

	state->map_mode = 1;
	state->bank_a_page = 0;
	state->bank_a_flash = 0;
	state->bank_b_page = 0;
	state->bank_b_flash = 0;
	reload_mapping(state);

	if (ti_read_byte(asic->mmu, 0x4000) != 0x12) {
		asic_free(asic);
		return 3;
	}

	if (ti_read_byte(asic->mmu, 0x8000) != 0x34) {
		asic_free(asic);
		return 4;
	}

	if (ti_read_byte(asic->mmu, 0xC000) != 0x12) {
		asic_free(asic);
		return 5;
	}
	return 0;
}

int test_status() {
	asic_t *asic = asic_init(TI83p, NULL);
	z80iodevice_t status = init_status(asic);
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

int test_link_port() {
	asic_t *asic = asic_init(TI83p, NULL);
	z80iodevice_t link = asic->cpu->devices[0x00];
	link_state_t *state = link.device;
	uint8_t value = link.read_in(state);
	if (value != 0) {
		asic_free(asic);
		return 1;
	}
	link.write_out(state, 0x01);
	value = link.read_in(state);
	if (value != 0x11) {
		asic_free(asic);
		return 2;
	}
	return 0;
}

int test_link_assist_rx() {
	asic_t *asic = asic_init(TI83pSE, NULL);
	z80iodevice_t link_assist_rx_read = asic->cpu->devices[0x0A];
	z80iodevice_t link_assist_status = asic->cpu->devices[0x09];
	link_state_t *state = link_assist_rx_read.device;

	if (!link_recv_byte(asic, 0xBE)) {
		asic_free(asic);
		return 1;
	}
	if (link_recv_byte(asic, 0xEF)) {
		asic_free(asic);
		return 2;
	}

	uint8_t status = link_assist_status.read_in(state);
	if (status != state->assist.status.u8 ||
			!state->assist.status.rx_ready ||
			!state->assist.status.int_rx_ready) {
		return 3;
	}

	uint8_t val = link_assist_rx_read.read_in(state);
	if (val != 0xBE) {
		return 4;
	}

	status = link_assist_status.read_in(state);
	if (status != state->assist.status.u8 ||
			state->assist.status.rx_ready ||
			state->assist.status.int_rx_ready) {
		return 5;
	}
	return 0;
}

int test_link_assist_tx() {
	asic_t *asic = asic_init(TI83pSE, NULL);
	z80iodevice_t link_assist_tx_read = asic->cpu->devices[0x0D];
	z80iodevice_t link_assist_status = asic->cpu->devices[0x09];
	link_state_t *state = link_assist_tx_read.device;

	if (link_read_tx_buffer(asic) != EOF) {
		asic_free(asic);
		return 1;
	}

	uint8_t status = link_assist_status.read_in(state);
	if (status != state->assist.status.u8 ||
			!state->assist.status.tx_ready ||
			!state->assist.status.int_tx_ready) {
		return 2;
	}

	link_assist_tx_read.write_out(state, 0xDE);

	status = link_assist_status.read_in(state);
	if (status != state->assist.status.u8 ||
			state->assist.status.tx_ready ||
			state->assist.status.int_tx_ready) {
		return 3;
	}

	if (link_read_tx_buffer(asic) != 0xDE) {
		return 4;
	}

	status = link_assist_status.read_in(state);
	if (status != state->assist.status.u8 ||
			!state->assist.status.tx_ready ||
			!state->assist.status.int_tx_ready) {
		return 5;
	}
	return 0;
}
