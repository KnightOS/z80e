#include "asic.h"
#include "tui.h"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <signal.h>

#include "hooks.h"

typedef struct {
    ti_device_type device;
    char *rom_file;
    int cycles;
    int print_state;
    int stop;
    int debugger;
    int no_rom_check;
} appContext_t;

appContext_t context;

appContext_t create_context(void) {
    appContext_t context;
    context.device = TI83p;
    context.rom_file = NULL;
    context.cycles = -1;
    context.print_state = 0;
    context.stop = 0;
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

void handleLongFlag(appContext_t *context, char *flag, int *i, char **argv) {
    if (strcasecmp(flag, "device") == 0) {
        char *next = argv[*i++];
        setDevice(context, next);
    } else if (strcasecmp(flag, "print-state") == 0) {
        context->print_state = 1;
    } else if (strcasecmp(flag, "no-rom-check") == 0) {
        context->no_rom_check = 1;
    } else if (strcasecmp(flag, "debug") == 0) {
        context->debugger = 1;
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

    if (context.debugger == 1) {
        printf(" Caught interrupt, entering debugger\n");
        context.stop = 2;
    } else if (context.debugger == 2) {
        printf(" Caught interrupt, stopping emulator\n");
        exit(0);
    } else {
        printf(" Caught interrupt, stopping emulation\n");
        context.stop = 1;
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
    if (context.rom_file == NULL) {
        printf("Warning: No ROM file specified\n");
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
            printf("Error: This file does not match the required ROM size of %d bytes (use --no-rom-check to override).\n", length);
            fclose(file);
            asic_free(device);
            return 1;
        }
        length = fread(device->mmu->flash, 0x4000, device->mmu->settings->flash_pages, file);
        fclose(file);
    }

    init_hooks();

    if (context.debugger) {
        tui_tick(device);
    }

    if (context.cycles == -1) { // Run indefinitely
        while (1) {
            // TODO: Timings
            cpu_execute(device->cpu, 1);

        backInLoop:
            if (context.stop) {
                break;
            }
        }
        if (context.stop == 2) {
            context.stop = 0;
            context.debugger = 2;
            tui_tick(device);
            context.debugger = 1;
            goto backInLoop;
        }
    } else {
        cpu_execute(device->cpu, context.cycles);
    }

    if (context.print_state) {
        z80registers_t r = device->cpu->registers;
        printf("   AF: 0x%04X   BC: 0x%04X   DE: 0x%04X  HL: 0x%04X\n", r.AF, r.BC, r.DE, r.HL);
        printf("  'AF: 0x%04X  'BC: 0x%04X  'DE: 0x%04X 'HL: 0x%04X\n", r._AF, r._BC, r._DE, r._HL);
        printf("   PC: 0x%04X   SP: 0x%04X   IX: 0x%04X  IY: 0x%04X\n", r.PC, r.SP, r.IX, r.IY);
        printf("Flags: ");
        if (r.flags.S) printf("S ");
        if (r.flags.Z) printf("Z ");
        if (r.flags.H) printf("H ");
        if (r.flags.PV) printf("P/V ");
        if (r.flags.N) printf("N ");
        if (r.flags.C) printf("C ");
        if (r.F == 0) printf("None set");
        printf("\n");
    }
    asic_free(device);
    return 0;
}
