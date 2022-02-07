void* heap_alloc(size_t size, size_t alignment);
void* heap_realloc(void* block, size_t size, size_t alignment);
void heap_free(void* block);
