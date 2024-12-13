#include <zf3c.h>

bool zf3_init_mem_arena(ZF3MemArena* const memArena, const int size) {
    memset(memArena, 0, sizeof(*memArena));

    memArena->buf = malloc(size);

    if (!memArena->buf) {
        return false;
    }

    memset(memArena->buf, 0, size);

    memArena->size = size;

    return true;
}

void zf3_clean_mem_arena(ZF3MemArena* const memArena) {
    free(memArena->buf);
    memset(memArena, 0, sizeof(*memArena));
}

void* zf3_push_to_mem_arena(ZF3MemArena* const memArena, const int size) {
    if (memArena->used + size > memArena->size) {
        return NULL;
    }

    void* const ptr = memArena->buf + memArena->used;
    memArena->used += size;
    return ptr;
}

void zf3_reset_mem_arena(ZF3MemArena* const memArena) {
    memset(memArena->buf, 0, memArena->used);
    memArena->used = 0;
}

bool zf3_is_zero(const void* const mem, const int size) {
    const ZF3Byte* const memBytes = mem;

    for (int i = 0; i < size; ++i) {
        if (memBytes[i]) {
            return false;
        }
    }

    return true;
}
