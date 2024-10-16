#pragma once

#include <stddef.h>

#include "cxlalloc.h"

class MemoryAllocator {
public:
    static void* allocate(size_t size) { return cxlalloc_malloc(size); }

    static void* aligned_allocate(size_t size) { return cxlalloc_malloc(size); }

    static void deallocate(void* ptr) { return cxlalloc_free(ptr); }
};
