#include "mmu.h"
#include <stdlib.h>

void
mmu_destroy(mmu_ref mmu)
{
    /* Call through to the actual implementation. */
    ((_mmu_base_t*)mmu)->isa->destroy(mmu);
}

uint8_t
mmu_read_byte(mmu_ref mmu, uint16_t address)
{
    /* Call through to the actual implementation. */
    return ((_mmu_base_t*)mmu)->isa->read_byte(mmu, address);
}

void
mmu_write_byte(mmu_ref mmu, uint16_t address, uint8_t value)
{
    /* Call through to the actual implementation. */
    ((_mmu_base_t*)mmu)->isa->write_byte(mmu, address, value);
}
