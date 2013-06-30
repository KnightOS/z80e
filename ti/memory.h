#ifndef MEMORY_H
#define MEMORY_H
#include "cpu.h"
#include <stdint.h>

typedef struct {
    uint8_t* memory_banks;
    uint16_t ram_length;
    uint16_t flash_length;
    uint8_t* ram;
    uint8_t* flash;
} ti_mmu_t;

ti_mmu_t* ti_mmu_init();
void ti_mmu_free(ti_mmu_t* mmu);
uint8_t ti_read_byte(void* memory, uint16_t address);
void ti_write_byte(void* memory, uint16_t address, uint8_t value);

#endif
