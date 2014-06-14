int debugger_initialized = 0;

int debugger_alwaysokc = 0;
int debugger_alwaysfailc = 0;

int debugger_alwaysok(debugger_state_t *state, int argc, char **argv) {
	debugger_alwaysokc++;
	return 0;
}

int debugger_alwaysfail(debugger_state_t *state, int argc, char **argv) {
	debugger_alwaysfailc++;
	return 0;
}

int test_debugger_register_command() {
	register_command("alwaysok", debugger_alwaysok, 0);
	register_command("alwaysfail", debugger_alwaysfail, 0);
	debugger_initialized = 1;

	return 0;
}

int test_debugger_find_command() {
	if (!debugger_initialized) {
		test_debugger_register_command();
	}

	debugger_command_t *command;
	int result = find_best_command("always", &command);
	if (result != -1) {
		return 1;
	}

	result = find_best_command("alwaysok", &command);
	if (!result || command->function != debugger_alwaysok) {
		return 2;
	}

	result = find_best_command("alwaysfail", &command);
	if (!result || command->function != debugger_alwaysfail) {
		return 3;
	}

	result = find_best_command("alwaysdontexist", &command);
	if (result != 0) {
		return 4;
	}

	return 0;
}

int hook_test_counter = 0;

int hook(ti_mmu_t *a, read_memory_struct_t *b) {
	hook_test_counter = 1;
	return 1;
}

int hook2(ti_mmu_t *a, read_memory_struct_t *b) {
	hook_test_counter = 50;
	return 0;
}

int test_debugger_hooks() {
	init_hooks();

	if (!read_memory_hooks) {
		deinit_hooks();
		return 1;
	}

	register_hook_read_memory(&hook);
	register_hook_read_memory(&hook2);

	if (read_memory_hooks->used != 2) {
		deinit_hooks();
		return 2;
	}

	read_memory_struct_t t = {0, 0};
	call_read_memory_hooks(0, &t);

	if (hook_test_counter != 1) {
		return 3;
	}

	deinit_hooks();
	return read_memory_hooks ? 4 : 0;
}

int test_debugger_tui_commandline() {
	const char *test_data = "5 + 3 \"5 + 3\""; // 5 + 3 "5 + 3"

	int argc = 0;
	char **result = tui_parse_commandline(test_data, &argc);

	if (argc != 4) {
		return 1;
	}

	if (strcmp(result[0], "5") != 0) {
		return 2;
	}

	if (strcmp(result[1], "+") != 0) {
		return 3;
	}

	if (strcmp(result[2], "3") != 0) {
		return 4;
	}

	if (strcmp(result[3], "5 + 3") != 0) {
		return 5;
	}

	return 0;
}
