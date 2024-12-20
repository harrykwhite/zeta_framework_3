#include <zf3_misc.h>

namespace zf3 {
    int get_first_inactive_bit_index(const Byte* const bytes, const int bitCnt) {
        for (int i = 0; i < bitCnt; ++i) {
            if (!is_bit_active(bytes, i)) {
                return i;
            }
        }

        return -1;
    }

    bool are_all_bits_active(const Byte* const bytes, const int bitCnt) {
        for (int i = 0; i < bitCnt; ++i) {
            if (!is_bit_active(bytes, i)) {
                return false;
            }
        }

        return true;
    }

    bool init_mem_arena(MemArena& arena, const int size) {
        zero_out(arena);

        arena.buf = alloc_zeroed<Byte>(size);

        if (!arena.buf) {
            return false;
        }

        arena.size = size;

        return true;
    }

    void clean_mem_arena(MemArena& arena) {
        if (arena.buf) {
            free(arena.buf);
        }

        zero_out(arena);
    }

    void* push_to_mem_arena(MemArena& arena, const int size, const int alignment) {
        assert(arena.buf);
        assert(size > 0);
        assert(is_power_of_two(alignment));

        const int offsAligned = align_forward(arena.offs, alignment);
        const int offsNext = offsAligned + size;

        if (offsNext > arena.size) {
            return nullptr;
        }

        arena.offs = offsNext;

        return static_cast<Byte*>(arena.buf) + offsAligned;
    }

    void reset_mem_arena(MemArena& arena) {
        memset(arena.buf, 0, arena.offs);
        arena.offs = 0;
    }
}
