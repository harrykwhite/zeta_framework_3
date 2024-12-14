#pragma once

#include <glad/glad.h>
#include <zf3c.h>

namespace zf3 {

using GLID = GLuint;

template<int BITS>
struct Bitset {
    Byte bytes[conv_bits_to_bytes(BITS)];
};

int get_first_inactive_bit_index(const Byte* const bytes, const int bitCnt);
bool are_all_bits_active(const Byte* const bytes, const int bitCnt);

inline bool is_bit_active(const Byte* const bytes, const int bitIndex) {
    return bytes[bitIndex / 8] & (1 << (bitIndex % 8));
}

inline void activate_bit(Byte* const bytes, const int bitIndex) {
    bytes[bitIndex / 8] |= 1 << (bitIndex % 8);
}

inline void deactivate_bit(Byte* const bytes, const int bitIndex) {
    bytes[bitIndex / 8] &= ~(1 << (bitIndex % 8));
}

inline void clear_bits(Byte* const bytes, const int bitCnt) {
    memset(bytes, 0, conv_bits_to_bytes(bitCnt));
}

}
