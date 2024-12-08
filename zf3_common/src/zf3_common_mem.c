#include <zf3_common.h>

bool zf3_mem_arena_init(ZF3MemArena* const memArena, const int size) {
    assert(size > 0);

    memArena->buf = malloc(size);

    if (!memArena->buf) {
        return false;
    }

    memset(memArena->buf, 0, size);

    memArena->size = size;
    memArena->used = 0;

    return true;
}

void* zf3_mem_arena_push(ZF3MemArena* const memArena, const int size) {
    assert(size > 0);

    if (memArena->used + size > memArena->size) {
        return NULL;
    }

    void* const ptr = memArena->buf + memArena->used;
    memArena->used += size;
    return ptr;
}

void zf3_mem_arena_reset(ZF3MemArena* const memArena) {
    memset(memArena->buf, 0, memArena->used);
    memArena->used = 0;
}

void zf3_mem_arena_cleanup(ZF3MemArena* const memArena) {
    free(memArena->buf);

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
