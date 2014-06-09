int debugger_initialized = 0;

int debugger_alwaysokc = 0;
int debugger_alwaysfailc = 0;

int debugger_alwaysok(debugger_state_t *state, int argc, char **argv) {
	debugger_alwaysokc++;
	return 0;
}

debugger_command_t *debugger_alwaysok_struct = 0;

int debugger_alwaysfail(debugger_state_t *state, int argc, char **argv) {
	debugger_alwaysfailc++;
	return 0;
}

debugger_command_t *debugger_alwaysfail_struct = 0;

void debugger_init() {
	debugger_alwaysok_struct = malloc(sizeof(debugger_command_t));
	debugger_alwaysok_struct->name = "alwaysok";
	debugger_alwaysok_struct->function = debugger_alwaysok;

	debugger_alwaysfail_struct = malloc(sizeof(debugger_command_t));
	debugger_alwaysfail_struct->name = "alwaysfail";
	debugger_alwaysfail_struct->function = debugger_alwaysfail;

	register_command(debugger_alwaysok_struct);
	register_command(debugger_alwaysfail_struct);
	debugger_initialized = 1;
}

int test_debugger_find_command() {
	if(!debugger_initialized)
		debugger_init();

	debugger_command_t *command;
	int result = find_best_command("always", &command);
	if (result != -1)
		return 1;

	result = find_best_command("alwaysok", &command);
	if (!result || command != debugger_alwaysok_struct)
		return 2;

	result = find_best_command("alwaysfail", &command);
	if (!result || command != debugger_alwaysfail_struct)
		return 3;

	result = find_best_command("dontexist", &command);
	if (result != 0)
		return 4;

	return 0;
}
