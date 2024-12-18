#pragma once

#include <assert.h>
#include <glad/glad.h>
#include <AL/al.h>
#include <zf3c.h>

namespace zf3 {
    using GLID = GLuint;
    using ALID = ALuint;

    template<int BIT_CNT>
    struct Bitset {
        Byte bytes[bits_to_bytes(BIT_CNT)];
    };

    template<typename T, int CNT>
    struct ActivityBuf {
        T buf[CNT];
        Bitset<CNT> activity;

        T& operator[](const int index);
        const T& operator[](const int index) const;

        T& get_even_if_inactive(const int index) {
            return buf[index];
        }

        const T& get_even_if_inactive(const int index) const {
            return buf[index];
        }
    };

    struct MemArena {
        Byte* bytes;
        int size;
        int offs;

        bool init(const int size);
        void clean();
        template<typename T> T* push(const int cnt = 1);
        void* push_size(const int size, const int alignment);
        void reset();
    };

    constexpr bool is_power_of_two(const int n) {
        assert(n >= 0);
        return n && !(n & (n - 1));
    }

    constexpr int align_forward(const int n, const int alignment) {
        assert(n >= 0);
        assert(is_power_of_two(alignment));
        return (n + alignment - 1) & ~(alignment - 1);
    }

    int get_first_inactive_bit_index(const Byte* const bytes, const int bitCnt);
    bool are_all_bits_active(const Byte* const bytes, const int bitCnt);

    inline void activate_bit(Byte* const bytes, const int bitIndex) {
        bytes[bitIndex / 8] |= 1 << (bitIndex % 8);
    }

    template<int BIT_CNT>
    inline void activate_bit(Bitset<BIT_CNT>& activity, const int bitIndex) {
        activate_bit(activity.bytes, bitIndex);
    }

    inline void deactivate_bit(Byte* const bytes, const int bitIndex) {
        bytes[bitIndex / 8] &= ~(1 << (bitIndex % 8));
    }

    template<int BIT_CNT>
    inline void deactivate_bit(Bitset<BIT_CNT>& activity, const int bitIndex) {
        deactivate_bit(activity.bytes, bitIndex);
    }

    inline void clear_bits(Byte* const bytes, const int bitCnt) {
        memset(bytes, 0, bits_to_bytes(bitCnt));
    }

    template<int BIT_CNT>
    inline void clear_bits(Bitset<BIT_CNT>& activity) {
        clear_bits(activity.bytes, BIT_CNT);
    }

    inline bool is_bit_active(const Byte* const bytes, const int bitIndex) {
        return bytes[bitIndex / 8] & (1 << (bitIndex % 8));
    }

    template<int BIT_CNT>
    inline bool is_bit_active(const Bitset<BIT_CNT>& activity, const int bitIndex) {
        return is_bit_active(activity.bytes, bitIndex);
    }

    template<int BIT_CNT>
    inline int get_first_inactive_bit_index(const Bitset<BIT_CNT>& activity) {
        return get_first_inactive_bit_index(activity.bytes, BIT_CNT);
    }

    template<int BIT_CNT>
    inline bool are_all_bits_active(const Bitset<BIT_CNT>& activity) {
        return are_all_bits_active(activity.bytes, BIT_CNT);
    }

    template<typename T, int CNT>
    inline T& ActivityBuf<T, CNT>::operator[](const int index) {
        assert(is_bit_active(activity, index));
        return buf[index];
    }

    template<typename T, int CNT>
    inline const T& ActivityBuf<T, CNT>::operator[](const int index) const {
        assert(is_bit_active(activity, index));
        return buf[index];
    }

    template<typename T>
    T* MemArena::push(const int cnt) {
        const int pushSize = sizeof(T) * cnt;
        const int offsAligned = align_forward(offs, alignof(T));
        const int offsNext = offsAligned + pushSize;

        if (offsNext > size) {
            return nullptr;
        }

        offs = offsNext;

        return reinterpret_cast<T*>(bytes + offsAligned);
    }
}
