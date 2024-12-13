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

}
