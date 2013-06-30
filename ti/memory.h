#ifndef MEMORY_H
#define MEMORY_H
#include "mmu.h"
#include <stdint.h>

/* A concrete implementation of an MMU. */

/* Opaque type reference for this MMU type.  The actual declaration of
 * this structure is inside the source file.  Clients should never see (or touch)
 * the implementation directly.  This is not a void* so that type safety can be
 * enforced. */
typedef struct __ti_mmu_s* ti_mmu_ref;

ti_mmu_ref
ti_mmu_create(/* TI MMU specific parameters */);

#endif
