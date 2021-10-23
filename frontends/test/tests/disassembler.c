char dis_buffer[40];
char *dis_pointer = dis_buffer;
uint8_t *dis_input;

int dis_write(struct disassemble_memory *memp, const char *data, ...) {
	va_list list;
	va_start(list, data);

	int count = vsnprintf((char *)dis_pointer, 40 - (dis_pointer - dis_buffer), data, list);
	dis_pointer += count;
	return count;
}

uint8_t dis_read_byte(struct disassemble_memory *memp, uint16_t pointer) {
	return dis_input[pointer];
}

int test_disassembler() {
	disassembler_init();
	uint8_t tests[] =
			{0x3a, 0x34, 0x12,  // LD A, (0x1234)
			 0xdd, 0xcb, 0x12, 0x06, // RLC (IX + 0x12)
			 0xfd, 0x36, 0x10, 0x20, // LD (IY + 0x10), 0x20
			 };

	struct disassemble_memory mem = { dis_read_byte, 0 };
	dis_input = tests;

	parse_instruction(&mem, dis_write, false);
	dis_pointer = dis_buffer;
	if (strcmp(dis_buffer, "LD A, (0x1234)") != 0) {
		return 1;
	}

	parse_instruction(&mem, dis_write, false);
	dis_pointer = dis_buffer;
	if (strcmp(dis_buffer, "RLC (IX + 0x12)") != 0) {
		return 2;
	}

	parse_instruction(&mem, dis_write, false);
	dis_pointer = dis_buffer;
	if (strcmp(dis_buffer, "LD (IY + 0x10), 0x20") != 0) {
		return 3;
	}

	return 0;
}
