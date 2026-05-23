#include <tracy/Tracy.hpp>

#ifdef IR_USE_TRACY

void* operator new(size_t size)
{
    void* ptr = malloc(size);
    TracyAlloc(ptr, size);
    return ptr;
}

void operator delete(void* ptr) noexcept
{
    if (!ptr) {
        return;
    }

    TracySecureFree(ptr);
    free(ptr);
}

#endif
