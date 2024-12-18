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

    bool MemArena::init(const int size) {
        zero_out(*this);

        bytes = alloc<Byte>(size);

        if (!bytes) {
            return false;
        }

        this->size = size;

        return true;
    }

    void MemArena::clean() {
        if (bytes) {
            free(bytes);
        }

        zero_out(*this);
    }

    void* MemArena::push_size(const int size, const int alignment) {
        assert(size > 0);
        assert(is_power_of_two(alignment));

        const int offsAligned = align_forward(offs, alignment);
        const int offsNext = offsAligned + size;

        if (offsNext > this->size) {
            return nullptr;
        }

        offs = offsNext;

        return bytes + offsAligned;
    }

    void MemArena::reset() {
        memset(bytes, 0, offs);
        offs = 0;
    }
}
