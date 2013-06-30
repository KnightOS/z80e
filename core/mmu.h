#ifndef MMU_H
#define MMU_H
#include <stdint.h>

/**
 *  This is an example of a robust way to implment object oriented
 *  programming in C.  From the client's (the user's) perspective,
 *  objects are opaque types that are accessed and manipulated by calling
 *  a defined set of functions or API.  It's like the f* collection of
 *  functions (fopen, fread, ...) for file manipulation except more
 *  powerful.  That's because this implementation supports polymorphism.
 *
 *  Every object is really a structure that is allocated on the heap.
 *  That structure always begins with an _mmu_base_t, which contains a 
 *  pointer to an __mmu_class_t.  Each provider of a concrete MMU
 *  implementation creates an instance of __mmu_class_t (usually a static
 *  variable as only one is needed) and populates it with pointers
 *  to their implementations of the functions defined to be polymorphic or
 *  common to all MMU implementations.
 *
 *  Shims are defined here for each polymorphic function that simply
 *  call through to the concrete implementations in the object's class.  
 *  You'll notice there are no 'create' function here because MMU is an 
 *  abstract type. Creating an MMU is up to the concrete implementation 
 *  providers and they must define a 'create' function specific to their 
 *  implementation.
 */

/* FOR MMU CLIENTS */
/* The following functions are called by clients of an MMU. */

/* Opaque type reference to an MMU.  The type is void* so that any other
 * pointer can be assigned to a variable of this type w/o casting. */
typedef void* mmu_ref;

void
mmu_destroy(mmu_ref mmu);

uint8_t
mmu_read_byte(mmu_ref mmu, uint16_t address);

void
mmu_write_byte(mmu_ref mmu, uint16_t address, uint8_t value);




/* FOR MMU IMPLEMENTORS */
/* The following structurs are used by providers of concrete MMU implementations. */

/**
 *  Type that defines an implementation (class) of an MMU. 
 */
typedef struct __mmu_class_s {
    /// Can be useful for debugging
    const char *class_name;
    
    /* Polymorphic functions */
    /* Each concrete implementation must provide an implementation for
     * each of these functions. 
     */
    void (*destroy)(mmu_ref mmu);
    uint8_t (*read_byte)(mmu_ref mmu, uint16_t address);
    void (*write_byte)(mmu_ref mmu, uint16_t address, uint8_t value);
    
} __mmu_class_t;


/**
 *  The 'header' or 'prefix' data of every mmu object.  Used to identify
 *  the object and dispatch polymorphic calls to the object's class.
 */
typedef struct _mmu_base_s {
    // The class of this mmu object.  Concrete implementations must
    // set this to point to their class when an object is created.
    __mmu_class_t *isa;
} _mmu_base_t;

#endif
