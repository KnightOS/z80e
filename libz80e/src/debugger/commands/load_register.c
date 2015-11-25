#include "debugger/commands.h"
#include "debugger/debugger.h"
#include <ctype.h>
#include <string.h>

int command_load_register(debugger_state_t *state, int argc, char **argv) {
	if (argc != 3) {
		state->print(state, "%s - Load a register with a specified value.\n", argv[0]);
		return 0;
	}
#define REGISTER(num, len, print) \
	if (strncasecmp(argv[1], print, len) == 0) {\
	char *result;\
	if(isxdigit(argv[2])) {\
		state->asic->cpu->registers. num = argv[2];\
	} else {\
	state->asic->cpu->registers. num = strtol(argv[2], &result, 16); \
	}\
}
		REGISTER(IXH, 3, "IXH");
		REGISTER(IXL, 3, "IXL");
		REGISTER(IYH, 3, "IYH");
		REGISTER(IYL, 3, "IYL");
		REGISTER(_BC, 3, "BC'");
		REGISTER(_DE, 3, "DE'");
		REGISTER(_HL, 3, "HL'");
		REGISTER(_AF, 3, "AF'");
		REGISTER(IX, 2, "IX");
		REGISTER(IY, 2, "IY");
		REGISTER(AF, 2, "AF");
		REGISTER(BC, 2, "BC");
		REGISTER(DE, 2, "DE");
		REGISTER(HL, 2, "HL");
		REGISTER(PC, 2, "PC");
		REGISTER(SP, 2, "SP");
		REGISTER(A, 1, "A");
		REGISTER(B, 1, "B");
		REGISTER(C, 1, "C");
		REGISTER(D, 1, "D");
		REGISTER(E, 1, "E");
		REGISTER(F, 1, "F");
		REGISTER(H, 1, "H");
		REGISTER(L, 1, "L");
		REGISTER(I, 1, "I");
		REGISTER(R, 1, "R");


	return 0;
}
