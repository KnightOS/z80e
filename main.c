#include "asic.h"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <signal.h>

typedef struct {
    ti_device_type device;
    char *rom_file;
    int cycles;
    int print_state;
    int stop;
} appContext_t;

appContext_t context;

appContext_t create_context(void) {
    appContext_t context;
    context.device = TI84pSE;
    context.rom_file = NULL;
    context.cycles = -1;
    context.print_state = 0;
    context.stop = 0;
    return context;
}

void setDevice(appContext_t *context, char* target) {
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
        char *next = argv[*i];
        *i += 1;
        setDevice(context, next);
    } else if (strcasecmp(flag, "print-state") == 0) {
        context->print_state = 1;
    } else {
        printf("Incorrect usage. See z80e --help.\n");
        exit(1);
    }
}

void sigint_handler(int sig) {
    signal(SIGINT, sigint_handler);
    context.stop = 1;
    printf(" Caught interrupt, stopping emulation\n");
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
        fclose(file);
    }
    if (context.cycles == -1) { // Run indefinitely
        while (1) {
            // TODO: Timings
            cpu_execute(device->cpu, 1);
            if (context.stop) {
                break;
            }
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
