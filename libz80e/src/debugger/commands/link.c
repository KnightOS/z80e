#include "debugger/commands.h"
#include "debugger/debugger.h"
#include "ti/hardware/link.h"
#include "debugger/hooks.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <wordexp.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

FILE *link_output = NULL;
FILE *link_input = NULL;
int hook;

void send_byte_from_input(struct debugger_state *state) {
	if (!link_input) {
		return;
	}
	int c = getc(link_input);
	if (c == EOF) {
		fclose(link_input);
		hook_remove_port_in(state->asic->hook, hook);
		return;
	}
	if (!link_recv_byte(state->asic, (uint8_t)c)) {
		state->print(state, "Warning: link not ready\n");
		ungetc(c, link_input);
	}
}

uint8_t on_link_rx_buffer_read(void *state, uint8_t port, uint8_t value) {
	struct debugger_state *s = state;
	s->print(s, "Link rx buffer read from\n");
	send_byte_from_input(state);
	return value;
}

int handle_send(struct debugger_state *state, int argc, char **argv) {
	char *path = strdup(argv[2]);
	wordexp_t p;
	if (wordexp(path, &p, 0) == 0) {
		free(path);
		path = strdup(p.we_wordv[0]);
	}

	link_input = fopen(path, "r");
	if (link_input) {
		state->print(state, "Sending file: %s\n", path);
		hook = hook_add_port_in(state->asic->hook, 0x0A, 0x0A, state, on_link_rx_buffer_read);
		send_byte_from_input(state);
		free(path);
		return 0;
	}
	free(path);

	int strl = 0;
	int i;
	for (i = 1; i < argc - 1; i++) {
		strl += strlen(argv[i + 1]) + 1;
	}

	char *data = malloc(strl);
	char *dpointer = data;
	for (i = 1; i < argc - 1; i++) {
		strcpy(dpointer, argv[i + 1]);
		dpointer += strlen(argv[i + 1]);
		*(dpointer++) = ' ';
	}
	*(dpointer - 1) = 0;

	uint8_t expr = parse_expression_z80e(state, data);

	free(data);

	if (!link_recv_byte(state->asic, expr)) {
		state->print(state, "Calculator is not ready to receive another byte.\n");
	} else {
		state->print(state, "Sent %02X to calculator's link assist.\n", expr);
	}
	return 0;
}

int handle_recv(struct debugger_state *state, int argc, char **argv) {
	return 0;
}

int handle_socket(struct debugger_state *state, int argc, char **argv) {
	state->asic->link->listenfd.fd =
		socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
	fcntl(state->asic->link->listenfd.fd, F_SETFL, O_NONBLOCK);
	state->asic->link->listenfd.events = POLLIN;
	if (state->asic->link->listenfd.fd == -1) {
		state->print(state, "Unable to create socket\n");
		return 1;
	}

	struct sockaddr_un *link_sockaddr = malloc(sizeof(struct sockaddr_un));
	if (link_sockaddr == NULL) {
		state->print(state, "Can't allocate socket address\n");
		return 1;
	}

	link_sockaddr->sun_family = AF_UNIX;
	strncpy(link_sockaddr->sun_path, argv[2], sizeof(link_sockaddr->sun_path));
	link_sockaddr->sun_path[sizeof(link_sockaddr->sun_path) - 1] = 0;

	unlink(link_sockaddr->sun_path);
	if (bind(state->asic->link->listenfd.fd,
				(struct sockaddr *)link_sockaddr,
				sizeof(*link_sockaddr)) == -1) {
		state->print(state, "Unable to bind socket\n");
		return 1;
	}

	if (listen(state->asic->link->listenfd.fd, 3) == -1) {
		state->print(state, "Unable to listen on socket\n");
		return 1;
	}

	state->print(state, "Bound to socket at %s\n", argv[2]);

	return 0;
}

int command_link(struct debugger_state *state, int argc, char **argv) {
	if (argc < 3) {
		state->print(state, "%s [send|recv|socket] [value|behavior|path]\n"
				"send a value will send a value to the link port. If you pass a file, it will be sent instead.\n"
				"recv [behavior] defines z80e's behavior when receiving link port data.\n"
				"Use 'print' to print each value, or a file name to write values to that file.\n", argv[0]);
		return 0;
	}

	if (strcasecmp(argv[1], "send") == 0) {
		return handle_send(state, argc, argv);
	} else if (strcasecmp(argv[1], "recv") == 0) {
		return handle_recv(state, argc, argv);
	} else if (strcasecmp(argv[1], "socket") == 0) {
		return handle_socket(state, argc, argv);
	} else {
		state->print(state, "Invalid operation %s", argv[1]);
	}

	return 0;
}

void init_link(struct debugger_state *state) {
}
