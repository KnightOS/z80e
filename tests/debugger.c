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

int test_debugger_init() {
	asic_t uninit;
	debugger_t *debugger = init_debugger(&uninit);

	if (debugger->asic != &uninit) {
		free_debugger(debugger);
		return 1;
	}
	int orig_count = debugger->commands.count;

	register_command(debugger, "alwaysok", debugger_alwaysok, NULL, 0);
	register_command(debugger, "alwaysfail", debugger_alwaysfail, NULL, 0);

	if (debugger->commands.count - orig_count != 2) {
		free_debugger(debugger);
		return 2;
	}

	free_debugger(debugger);
	return 0;
}

int test_debugger_find_command() {
	asic_t uninit;
	debugger_t *debugger = init_debugger(&uninit);
	register_command(debugger, "alwaysok", debugger_alwaysok, NULL, 0);
	register_command(debugger, "alwaysfail", debugger_alwaysfail, NULL, 0);

	debugger_command_t *command;
	int result = find_best_command(debugger, "always", &command);
	if (result != -1) {
		return 1;
	}

	result = find_best_command(debugger, "alwaysok", &command);
	if (!result || command->function != debugger_alwaysok) {
		return 2;
	}

	result = find_best_command(debugger, "alwaysfail", &command);
	if (!result || command->function != debugger_alwaysfail) {
		return 3;
	}

	result = find_best_command(debugger, "alwaysdontexist", &command);
	if (result != 0) {
		return 4;
	}

	return 0;
}

int test_debugger_commandline() {
	const char *test_data = "5 + 3 \"5 + 3\""; // 5 + 3 "5 + 3"

	int argc = 0;
	char **result = debugger_parse_commandline(test_data, &argc);

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
