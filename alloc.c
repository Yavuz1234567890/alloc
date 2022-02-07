#include "alloc.h"

typedef char byte;
typedef char* bytep;
typedef void* voidp;

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif
#include <malloc.h>
#include <memory.h>

typedef void*(*alloc_fn)(size_t, size_t);
typedef void(free_fn)(voidp);

#define MAX(x, y) x > y ? y : x
#define MIN(x, y) x < y ? y : x

voidp os_heap_alloc(size_t size) {
#ifdef _WIN32
    return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
#else
    return malloc(size);
#endif
}

void os_heap_free(void* block) {
#ifdef _WIN32
    HeapFree(GetProcessHeap(), 0, block);
#else
    free(block);
#endif
}

voidp aligned_heap_alloc(size_t size, size_t alignment) {
    int align = MAX(alignment, 16) - 1;
    voidp heap_buffer = os_heap_alloc(size + align);
    voidp* buffer = NULL;
    if(heap_buffer) {
        buffer = (voidp*)(((size_t)(heap_buffer) + align) & ~(alignment - 1));
        buffer[-1] = heap_buffer;
    }
    return (voidp)buffer;
}

void aligned_heap_free(voidp block) {
    if(block) {
        voidp buffer = ((voidp*)block)[-1];
        os_heap_free(buffer);
    }
}

voidp realloc_buffer(alloc_fn fn, free_fn release, voidp block, size_t size, size_t alignment) {
    void* mem = fn(size, alignment);
    memcpy(mem, block, size);
    release(block);
    return mem;
}

void* heap_alloc(size_t size, size_t alignment) {
    return aligned_heap_alloc(size, alignment);
}

void* heap_realloc(void* block, size_t size, size_t alignment) {
    return realloc_buffer(heap_alloc, heap_free, block, size, alignment);
}

void heap_free(void* block) {
    aligned_heap_free(block);
}
