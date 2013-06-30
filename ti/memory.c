#include "memory.h"
#include "cpu.h"
#include <stdint.h>
#include <stdlib.h>

/* Polymorphic function prototypes */
void __ti_mmu_destroy(mmu_ref mmu);
uint8_t __ti_mmu_read_byte(mmu_ref mmu, uint16_t address);
void __ti_mmu_write_byte(mmu_ref mu, uint16_t address, uint8_t value);

/* Implementation specific class. */
static __mmu_class_t __ti_mmu_class = {
    "ti_mmu",
    __ti_mmu_destroy,       // destroy
    __ti_mmu_read_byte,     // read_byte
    __ti_mmu_write_byte,     // write_byte
};


typedef struct {
    uint16_t ram_pages;
    uint16_t flash_pages;
} ti_mmu_settings_t;

struct __ti_mmu_s {
    // _mmu_base_t must be at the beginning so that objects
    // of this type can be casted to an _mmu_base_t safely.
    _mmu_base_t base;
    
    /* Implementation specific 'instance' variables. */
    
    ti_mmu_settings_t settings;
    uint8_t* memory_banks;
    uint8_t* ram;
    uint8_t* flash;
};

/* Implementation specific functions */

ti_mmu_ref
ti_mmu_create(/* TI MMU specific parameters */)
{
    ti_mmu_ref mmu = malloc(sizeof(struct __ti_mmu_s));
    mmu->base.isa = &__ti_mmu_class; // Do not forget this!
    
    mmu->ram = malloc(1234);
    mmu->ram[0] = 10;
    mmu->ram[1] = 42;
    mmu->ram[2] = 63;
    
    return mmu;
}

// You can define other non-polymorphic functions that may be specific to
// this implementation.  Such functions should take a ti_mmu_ref as their
// first parameter so that type safety is enforced.

/* Polymorphic function implementations */

void
__ti_mmu_destroy(mmu_ref mmu)
{
    /* Down cast to this implementation's type. */
    ti_mmu_ref ti_mmu = (ti_mmu_ref)mmu;
    
    free(ti_mmu->ram);
    free(ti_mmu);
}

uint8_t
__ti_mmu_read_byte(mmu_ref mmu, uint16_t address)
{
    /* Down cast to this implementation's type. */
    ti_mmu_ref ti_mmu = (ti_mmu_ref)mmu;
    
    return ti_mmu->ram[address];
}

void
__ti_mmu_write_byte(mmu_ref mmu, uint16_t address, uint8_t value)
{
    /* Down cast to this implementation's type. */
    ti_mmu_ref ti_mmu = (ti_mmu_ref)mmu;
    
    // TODO
}
