#include <z80e/ti/asic.h>
#include <z80e/debugger/debugger.h>
#include <z80e/disassembler/disassemble.h>
#include <z80e/runloop/runloop.h>
#include "tui.h"
#include <z80e/debugger/commands.h>
#include <z80e/log/log.h>
#include <z80e/ti/hardware/t6a04.h>
#include <z80e/ti/hardware/keyboard.h>
#include <z80e/ti/hardware/interrupts.h>

#include <SDL/SDL.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <time.h>
#include <limits.h>

typedef struct {
	char *key;
	loglevel_t level;
} loglevel_options_t;

const loglevel_options_t log_options[] = {
	{ "DEBUG", L_DEBUG },
	{ "WARN", L_WARN },
	{ "ERROR", L_ERROR },
	{ "INFO", L_INFO }
};

typedef struct {
	ti_device_type device;
	asic_t *device_asic;
	char *rom_file;
	int cycles;
	int print_state;
	int no_rom_check;
	loglevel_t log_level;
	int braille;
	int scale;
	int offset_l;
	int offset_t;
	SDL_Surface *screen;
} appContext_t;

#define COLOR_ON    0x00, 0x00, 0x00
#define COLOR_OFF   0x99, 0xb1, 0x99
#define COLOR_CLEAR 0xc6, 0xe6, 0xc6

unsigned int pixel_color_on;
unsigned int pixel_color_off;
unsigned int pixel_color_clear;

appContext_t context;

appContext_t create_context(void) {
	appContext_t context;
	context.device = TI83p;
	context.rom_file = NULL;
	context.cycles = -1;
	context.print_state = 0;
	context.no_rom_check = 0;
	context.log_level = L_WARN;
	context.braille = 0;
	context.scale = 5;
	context.offset_l = 0;
	context.offset_t = 0;
	return context;
}

int lcd_changed = 0;
void lcd_changed_hook(void *data, ti_bw_lcd_t *lcd) {
	lcd_changed = 1;
}

void tui_unicode_to_utf8(char *b, uint32_t c) {
	if (c<0x80) *b++=c;
	else if (c<0x800) *b++=192+c/64, *b++=128+c%64;
	else if (c-0xd800u<0x800) return;
	else if (c<0x10000) *b++=224+c/4096, *b++=128+c/64%64, *b++=128+c%64;
	else if (c<0x110000) *b++=240+c/262144, *b++=128+c/4096%64, *b++=128+c/64%64, *b++=128+c%64;
}

void print_lcd(void *data, ti_bw_lcd_t *lcd) {
	int cY;
	int cX;

	if (context.braille) {
		for (cX = 0; cX < 64; cX += 4) {
			for (cY = 0; cY < 120; cY += 2) {
				int a = bw_lcd_read_screen(lcd, cY + 0, cX + 0);
				int b = bw_lcd_read_screen(lcd, cY + 0, cX + 1);
				int c = bw_lcd_read_screen(lcd, cY + 0, cX + 2);
				int d = bw_lcd_read_screen(lcd, cY + 1, cX + 0);
				int e = bw_lcd_read_screen(lcd, cY + 1, cX + 1);
				int f = bw_lcd_read_screen(lcd, cY + 1, cX + 2);
				int g = bw_lcd_read_screen(lcd, cY + 0, cX + 3);
				int h = bw_lcd_read_screen(lcd, cY + 1, cX + 3);
				uint32_t byte_value = 0x2800;
				byte_value += (
						(a << 0) |
						(b << 1) |
						(c << 2) |
						(d << 3) |
						(e << 4) |
						(f << 5) |
						(g << 6) |
						(h << 7));
				char buff[5] = {0};
				tui_unicode_to_utf8(buff, byte_value);
				printf("%s", buff);
			}
			printf("\n");
		}
	}

	/* Update SDL display */

	if (SDL_MUSTLOCK(context.screen)) {
		SDL_LockSurface(context.screen);
	}

	for (cX = 0; cX < 64; cX += 1) {
		for (cY = 0; cY < 96; cY += 1) {
			int on = bw_lcd_read_screen(lcd, cY, cX);
			int i, j;
			for (i = 0; i < context.scale; ++i) {
				for (j = 0; j < context.scale; ++j) {
					uint8_t *p = (uint8_t *)context.screen->pixels + (context.offset_t + cX * context.scale + i) * context.screen->pitch + (context.offset_l + cY * context.scale + j) * context.screen->format->BytesPerPixel;
					*(uint32_t *)p = on ? pixel_color_on : pixel_color_off;
				}
			}
		}
	}

	if (SDL_MUSTLOCK(context.screen)) {
		SDL_UnlockSurface(context.screen);
	}
	SDL_UpdateRect(context.screen, context.offset_l, context.offset_t, 96 * context.scale, 64 * context.scale);

	/* Push pixels to display */

	if (context.log_level >= L_INFO) {
		printf("C: 0x%02X X: 0x%02X Y: 0x%02X Z: 0x%02X\n", lcd->contrast, lcd->X, lcd->Y, lcd->Z);
		printf("   %c%c%c%c  O1: 0x%01X 02: 0x%01X\n", lcd->up ? 'V' : '^', lcd->counter ? '-' : '|',
				lcd->word_length ? '8' : '6', lcd->display_on ? 'O' : ' ', lcd->op_amp1, lcd->op_amp2);
	}
}

void lcd_timer_tick(asic_t *asic, void *data) {
	ti_bw_lcd_t *lcd = data;
	if (lcd_changed) {
		print_lcd(asic, lcd);
		lcd_changed = 0;
	}
}

void setDevice(appContext_t *context, char *target) {
	if (strcasecmp(target, "TI73") == 0) {
		context->device = TI73;
	} else if (strcasecmp(target, "TI83p") == 0) {
		context->device = TI83p;
	} else if (strcasecmp(target, "TI83pSE") == 0) {
		context->device = TI83pSE;
	} else if (strcasecmp(target, "TI84p") == 0) {
		context->device = TI84p;
	} else if (strcasecmp(target, "TI84pSE") == 0) {
		context->device = TI84pSE;
	} else if (strcasecmp(target, "TI84pCSE") == 0) {
		context->device = TI84pCSE;
	} else {
		printf("Incorrect usage. See z80e --help.\n");
		exit(1);
	}
}

void print_help(void) {
	printf("z80e - Emulate z80 calculators\n"
			"Usage: z80e [flags] [rom]\n\n"
			"Flags:\n"
			"\t-d <device>: Selects a device to emulate. Available devices:\n"
			"\t\tTI73, TI83p, TI83pSE, TI84p, TI84pSE, TI84pCSE\n"
			"\t-c <cycles>: Emulate this number of cycles, then exit. If omitted, the machine will be emulated indefinitely.\n"
			"\t-s <scale>: Scaleoffset_l, offset_t, 96 * scale, 64 * scale window by <scale> times.\n"
			"\t--braille: Enables braille display in console.\n"
			"\t--print-state: Prints the state of the machine on exit.\n"
			"\t--no-rom-check: Skips the check that ensure the provided ROM file is the correct size.\n"
			"\t--debug: Enable the debugger, which is enabled by interrupting the emulator.\n");
}

void handleFlag(appContext_t *context, char flag, int *i, char **argv) {
	char *next;
	switch (flag) {
		case 'd':
			next = argv[++*i];
			setDevice(context, next);
			break;
		case 'c':
			next = argv[++*i];
			context->cycles = atoi(next);
			break;
		case 's':
			next = argv[++*i];
			context->scale = atoi(next);
			break;
		default:
			printf("Incorrect usage. See z80e --help.\n");
			exit(1);
			break;
	}
}

int enable_debug = 0;

void handleLongFlag(appContext_t *context, char *flag, int *i, char **argv) {
	if (strcasecmp(flag, "device") == 0) {
		char *next = argv[++*i];
		setDevice(context, next);
	} else if (strcasecmp(flag, "print-state") == 0) {
		context->print_state = 1;
	} else if (strcasecmp(flag, "no-rom-check") == 0) {
		context->no_rom_check = 1;
	} else if (strcasecmp(flag, "braille") == 0) {
		context->braille = 1;
	} else if (strcasecmp(flag, "log") == 0) {
		char *level = argv[++*i];
		int j;
		for (j = 0; j < sizeof(log_options) / sizeof(loglevel_options_t); ++j) {
			if (strcasecmp(level, log_options[j].key) == 0) {
				context->log_level = log_options[j].level;
				return;
			}
		}
		fprintf(stderr, "%s is not a valid logging level.\n", level);
		exit(1);
	} else if (strcasecmp(flag, "debug") == 0) {
		enable_debug = 1;
	} else if (strcasecmp(flag, "help") == 0) {
		print_help();
		exit(0);
	} else {
		printf("Incorrect usage. See z80e --help.\n");
		exit(1);
	}
}

void frontend_log(void *data, loglevel_t level, const char *part, const char *message, va_list args) {
	vprintf(message, args);
	printf("\n");
}

void sigint_handler(int sig) {
	signal(SIGINT, sigint_handler);

	log_message(context.device_asic->log, L_ERROR, "sigint", "Caught interrupt, stopping emulation");
	context.device_asic->stopped = 1;

	if (!context.device_asic->debugger || context.device_asic->debugger->state == DEBUGGER_ENABLED) {
#ifdef CURSES
		endwin();
#endif
		exit(0);
	}

	fflush(stdout);
}

void key_tap(asic_t *asic, int scancode, int down) {
	if (down) {
		depress_key(asic->cpu->devices[0x01].device, scancode);
	} else {
		release_key(asic->cpu->devices[0x01].device, scancode);
	}
}

void setup_display(int w, int h) {
	context.screen = SDL_SetVideoMode(w, h, 32, SDL_SWSURFACE | SDL_RESIZABLE);

	pixel_color_on    = SDL_MapRGB(context.screen->format, COLOR_ON);
	pixel_color_off   = SDL_MapRGB(context.screen->format, COLOR_OFF);
	pixel_color_clear = SDL_MapRGB(context.screen->format, COLOR_CLEAR);

	int best_w_scale = w / 96;
	int best_h_scale = h / 64;

	int y, x;

	if (best_w_scale < best_h_scale) {
		context.scale = best_w_scale;
	} else {
		context.scale = best_h_scale;
	}

	context.offset_l = (w - context.scale * 96) / 2;
	context.offset_t = (h - context.scale * 64) / 2;

	if (SDL_MUSTLOCK(context.screen)) {
		SDL_LockSurface(context.screen);
	}

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			uint8_t *p = (uint8_t *)context.screen->pixels + y * context.screen->pitch + x * context.screen->format->BytesPerPixel;
			*(uint32_t *)p = pixel_color_clear;
		}
	}

	if (SDL_MUSTLOCK(context.screen)) {
		SDL_UnlockSurface(context.screen);
	}

	SDL_UpdateRect(context.screen, 0, 0, w, h);

	lcd_changed = 1;
}

void sdl_events_hook(asic_t *device, void * unused) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				switch (event.key.keysym.sym) {
					case SDLK_F1: /* F1 */
						key_tap(device, 0x64, event.type == SDL_KEYDOWN);
						break;
					case SDLK_F2: /* F2 */
						key_tap(device, 0x63, event.type == SDL_KEYDOWN);
						break;
					case SDLK_F3: /* F3 */
						key_tap(device, 0x62, event.type == SDL_KEYDOWN);
						break;
					case SDLK_F4: /* F4 */
						key_tap(device, 0x61, event.type == SDL_KEYDOWN);
						break;
					case SDLK_F5: /* F5 */
						key_tap(device, 0x60, event.type == SDL_KEYDOWN);
						break;
					case SDLK_UP: /* Up */
						key_tap(device, 0x03, event.type == SDL_KEYDOWN);
						break;
					case SDLK_LEFT: /* Left */
						key_tap(device, 0x01, event.type == SDL_KEYDOWN);
						break;
					case SDLK_RIGHT: /* Right */
						key_tap(device, 0x02, event.type == SDL_KEYDOWN);
						break;
					case SDLK_DOWN: /* Down */
						key_tap(device, 0x00, event.type == SDL_KEYDOWN);
						break;
					case SDLK_LSHIFT: /* Shift = 2nd */
					case SDLK_RSHIFT:
						key_tap(device, 0x65, event.type == SDL_KEYDOWN);
						break;
					case SDLK_RETURN: /* Enter */
						key_tap(device, 0x10, event.type == SDL_KEYDOWN);
						break;
					case SDLK_ESCAPE:
						key_tap(device, 0x66, event.type == SDL_KEYDOWN);
						break;
					case SDLK_0:
						key_tap(device, 0x40, event.type == SDL_KEYDOWN);
						break;
					case SDLK_1:
						key_tap(device, 0x41, event.type == SDL_KEYDOWN);
						break;
					case SDLK_2:
						key_tap(device, 0x31, event.type == SDL_KEYDOWN);
						break;
					case SDLK_3:
						key_tap(device, 0x21, event.type == SDL_KEYDOWN);
						break;
					case SDLK_4:
						key_tap(device, 0x42, event.type == SDL_KEYDOWN);
						break;
					case SDLK_5:
						key_tap(device, 0x32, event.type == SDL_KEYDOWN);
						break;
					case SDLK_6:
						key_tap(device, 0x22, event.type == SDL_KEYDOWN);
						break;
					case SDLK_7:
						key_tap(device, 0x43, event.type == SDL_KEYDOWN);
						break;
					case SDLK_8:
						key_tap(device, 0x33, event.type == SDL_KEYDOWN);
						break;
					case SDLK_9:
						key_tap(device, 0x23, event.type == SDL_KEYDOWN);
						break;

					case SDLK_F12:
						ti_interrupts_interrupt(device->interrupts, INTERRUPT_ON_KEY);
						break;
					default:
						break;
				}

				break;
			case SDL_VIDEORESIZE:
				setup_display(event.resize.w, event.resize.h);
				break;
			case SDL_QUIT:
				exit(0);
				break;
			default:
				break;
		}
	}
}

int main(int argc, char **argv) {
	context = create_context();
	signal(SIGINT, sigint_handler);

	int i;
	for (i = 1; i < argc; i++) {
		char *a = argv[i];
		if (*a == '-') {
			a++;
			if (*a == '-') {
				handleLongFlag(&context, a + 1, &i, argv);
			} else {
				while (*a) {
					handleFlag(&context, *a++, &i, argv);
				}
			}
		} else {
			if (context.rom_file == NULL) {
				context.rom_file = a;
			} else {
				printf("Incorrect usage. See z80e --help.\n");
				return 1;
			}
		}
	}

	log_t *log = init_log(frontend_log, 0, context.log_level);
	asic_t *device = asic_init(context.device, log);
	context.device_asic = device;

	if (enable_debug) {
		device->debugger = init_debugger(device);
		device->debugger->state = DEBUGGER_ENABLED;
	}

	if (context.rom_file == NULL && !enable_debug) {
		log_message(device->log, L_WARN, "main", "No ROM file specified, starting debugger");
		device->debugger = init_debugger(device);
		device->debugger->state = DEBUGGER_ENABLED;
	} else {
		FILE *file = fopen(context.rom_file, "r");
		if (!file) {
			printf("Error opening '%s'.\n", context.rom_file);
			asic_free(device);
			return 1;
		}
		int length;
		fseek(file, 0L, SEEK_END);
		length = ftell(file);
		fseek(file, 0L, SEEK_SET);
		if (!context.no_rom_check && length != device->mmu->settings.flash_pages * 0x4000) {
			printf("Error: This file does not match the required ROM size of %d bytes, but it is %d bytes (use --no-rom-check to override).\n",
					device->mmu->settings.flash_pages * 0x4000, length);
			fclose(file);
			asic_free(device);
			return 1;
		}
		length = fread(device->mmu->flash, 0x4000, device->mmu->settings.flash_pages, file);
		fclose(file);
	}

	SDL_Init(SDL_INIT_EVERYTHING);
	setup_display(context.scale * 96, context.scale * 64);
	SDL_WM_SetCaption("z80e", NULL);

	hook_add_lcd_update(device->hook, NULL, lcd_changed_hook);
	asic_add_timer(device, 0, 60, lcd_timer_tick, device->cpu->devices[0x10].device);
	asic_add_timer(device, 0, 100, sdl_events_hook, device->cpu->devices[0x10].device);

	if (device->debugger) {
		tui_state_t state = { device->debugger };
		tui_init(&state);
		tui_tick(&state);
	} else {
		if (context.cycles == -1) { // Run indefinitely
			while (1) {
				runloop_tick(device->runloop);
				if (device->stopped) {
					break;
				}
				nanosleep((struct timespec[]){{0, (1.f / 60.f) * 1000000000}}, NULL);
			}
		} else {
			runloop_tick_cycles(device->runloop, context.cycles);
		}
	}

finish:

	if (context.print_state) {
		print_state(&device->cpu->registers);
	}
	asic_free(device);
	return 0;
}
