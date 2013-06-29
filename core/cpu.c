#include "cpu.h"
#include <string.h> /* memset */
#include <stdlib.h> /* malloc, calloc */

z80cpu_t* z80cpu_init() {
    // Allocate memory for the CPU struct
    z80cpu_t* cpu = malloc(sizeof(z80cpu_t));
    
    // Allocate memory for the registers.
    // calloc automatically zero-initialises. 
    cpu->registers = calloc(1, sizeof(z80registers_t));
    cpu->shadow_registers = calloc(1, sizeof(z80registers_t));

    return cpu;
}
