#include <z80e/ti/asic.h>
#include <z80e/debugger/debugger.h>
#include <z80e/disassembler/disassemble.h>
#include <z80e/runloop/runloop.h>
#include "tui.h"
#include <z80e/debugger/commands.h>
#include <z80e/log/log.h>
#include <z80e/ti/hardware/t6a04.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <time.h>
#include <limits.h>

typedef struct {
    ti_device_type device;
    asic_t *device_asic;
    char *rom_file;
    int cycles;
    int print_state;
    int no_rom_check;
} appContext_t;

appContext_t context;

appContext_t create_context(void) {
    appContext_t context;
    context.device = TI83p;
    context.rom_file = NULL;
    context.cycles = -1;
    context.print_state = 0;
    context.no_rom_check = 0;
    return context;
}

int lcd_changed = 0;
void lcd_changed_hook(void *data, ti_bw_lcd_t *lcd) {
	lcd_changed = 1;
}

void print_lcd(void *data, ti_bw_lcd_t *lcd) {
	int cY;
	int cX;

	#define LCD_BRAILLE
	#ifndef LCD_BRAILLE
		for (cX = 0; cX < 64; cX++) {
			for (cY = 0; cY < 120; cY++) {
				printf("%c", bw_lcd_read_screen(lcd, cY, cX) ? 'O' : ' ');
			}
			printf("\n");
		}
	#else
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
				unicode_to_utf8(buff, byte_value);
				printf("%s", buff);
			}
			printf("\n");
		}
	#endif
	printf("C: 0x%02X X: 0x%02X Y: 0x%02X Z: 0x%02X\n", lcd->contrast, lcd->X, lcd->Y, lcd->Z);
	printf("   %c%c%c%c  O1: 0x%01X 02: 0x%01X\n", lcd->up ? 'V' : '^', lcd->counter ? '-' : '|',
		lcd->word_length ? '8' : '6', lcd->display_on ? 'O' : ' ', lcd->op_amp1, lcd->op_amp2);
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
        default:
            printf("Incorrect usage. See z80e --help.\n");
            exit(1);
            break;
    }
}

debugger_state tmp_debug = 0;

void handleLongFlag(appContext_t *context, char *flag, int *i, char **argv) {
    if (strcasecmp(flag, "device") == 0) {
        char *next = argv[*i++];
        setDevice(context, next);
    } else if (strcasecmp(flag, "print-state") == 0) {
        context->print_state = 1;
    } else if (strcasecmp(flag, "no-rom-check") == 0) {
        context->no_rom_check = 1;
    } else if (strcasecmp(flag, "debug") == 0) {
        tmp_debug = DEBUGGER_ENABLED;
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
    context.device_asic->state->stopped = 1;

    if (context.device_asic->state->debugger == DEBUGGER_ENABLED) {
        #ifdef CURSES
            endwin();
        #endif
        exit(0);
    }

    fflush(stdout);
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

    asic_t *device = asic_init(context.device);
    device->log = init_log(frontend_log, 0, INT_MAX);
    context.device_asic = device;
    device->state->debugger = tmp_debug;
    if (context.rom_file == NULL) {
        log_message(device->log, L_WARN, "main", "No ROM file specified, starting debugger");
        device->state->debugger = DEBUGGER_ENABLED;
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
        if (!context.no_rom_check && length != device->mmu->settings->flash_pages * 0x4000) {
            printf("Error: This file does not match the required ROM size of %d bytes, but it is %d bytes (use --no-rom-check to override).\n",
		device->mmu->settings->flash_pages * 0x4000, length);
            fclose(file);
            asic_free(device);
            return 1;
        }
        length = fread(device->mmu->flash, 0x4000, device->mmu->settings->flash_pages, file);
        fclose(file);
    }

    hook_add_lcd_update(device->hook, NULL, lcd_changed_hook);
    asic_add_timer(device, 0, 60, lcd_timer_tick, device->cpu->devices[0x10].device);

    debugger_t *debugger = init_debugger(device);

    if (device->state->debugger) {
        tui_state_t state = { debugger };
	tui_init(&state);
        tui_tick(&state);
    } else {
        if (context.cycles == -1) { // Run indefinitely
            while (1) {
                runloop_tick(device->state->runloop);
                if (device->state->stopped) {
                    break;
                }
                nanosleep((struct timespec[]){{0, (1.f / 60.f) * 1000000000}}, NULL);
            }
        } else {
            runloop_tick_cycles(device->state->runloop, context.cycles);
        }
    }

    if (context.print_state) {
        print_state(&device->cpu->registers);
    }
    asic_free(device);
    return 0;
}
