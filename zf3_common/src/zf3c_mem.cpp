#include <zf3c.h>

namespace zf3 {

bool init_mem_arena(MemArena* const memArena, const int size) {
    memset(memArena, 0, sizeof(*memArena));

    memArena->buf = static_cast<zf3::Byte*>(malloc(size));

    if (!memArena->buf) {
        return false;
    }

    memset(memArena->buf, 0, size);

    memArena->size = size;

    return true;
}

void clean_mem_arena(MemArena* const memArena) {
    free(memArena->buf);
    memset(memArena, 0, sizeof(*memArena));
}

void reset_mem_arena(MemArena* const memArena) {
    memset(memArena->buf, 0, memArena->used);
    memArena->used = 0;
}

}
