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

#ifdef CURSES
#include <curses.h>
#include <locale.h>
#endif

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

void sigint_handler(int sig) {
    signal(SIGINT, sigint_handler);

    log_message(L_ERROR, "sigint", "Caught interrupt, stopping emulation");
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

#ifdef CURSES
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    nonl();
    timeout(0);
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    start_color();
    use_default_colors();

    int width, height;
    getmaxyx(stdscr, height, width);

    int lcd_width = 60;
    int lcd_height = 18;
    int lcd_x = width / 2 - lcd_width / 2;
    int lcd_y = 1;

    WINDOW *lcd_window_border = newwin(lcd_height + 2, lcd_width + 2, lcd_y - 1, lcd_x - 1);
    WINDOW *lcd_window = newwin(lcd_height, lcd_width, lcd_y, lcd_x);
    box(lcd_window_border, 0, 0);
    wrefresh(lcd_window_border);

    bw_lcd_set_window(device->cpu->devices[0x10].device, lcd_window);

    WINDOW *log_window_border = newwin(height - 2 - (lcd_height + 2), width / 2 - 2, 1 + lcd_height + 2, 1);
    log_window = newwin(height - 4 - (lcd_height + 2), width / 2 - 4, 2 + (lcd_height + 2), 2);
    box(log_window_border, 0, 0);
    wrefresh(log_window_border);
    scrollok(log_window, TRUE);

    WINDOW *debugger_border = newwin(height - 2 - (lcd_height + 2), width / 2 - 1, 1 + lcd_height + 2, width / 2 - 1);
    WINDOW *debugger_window = newwin(height - 4 - (lcd_height + 2), width / 2 - 3, lcd_height + 4, width / 2);
    box(debugger_border, 0, 0);
    wrefresh(debugger_border);
    scrollok(debugger_window, TRUE);
#endif

    context.device_asic = device;
    device->state->debugger = tmp_debug;
    if (context.rom_file == NULL) {
        log_message(L_WARN, "main", "No ROM file specified, starting debugger");
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
            #ifdef CURSES
                endwin();
            #endif
            printf("Error: This file does not match the required ROM size of %d bytes, but it is %d bytes (use --no-rom-check to override).\n",
		device->mmu->settings->flash_pages * 0x4000, length);
            fclose(file);
            asic_free(device);
            return 1;
        }
        length = fread(device->mmu->flash, 0x4000, device->mmu->settings->flash_pages, file);
        fclose(file);
    }

    debugger_t *debugger = init_debugger(device);

    if (device->state->debugger) {
        #ifdef CURSES
        tui_state_t state = { debugger, debugger_window };
        #else
        tui_state_t state = { debugger };
        #endif
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
    #ifdef CURSES
        endwin();
    #endif
    return 0;
}
