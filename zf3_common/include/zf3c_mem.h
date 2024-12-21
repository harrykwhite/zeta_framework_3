#pragma once

#include <assert.h>
#include <stdlib.h>
#include <string.h>

namespace zf3 {
    using Byte = unsigned char;

    struct MemArena {
        void* buf;
        int size;
        int offs;
    };

    struct Bitset {
        Byte* bytes;
        int bitCnt;
    };

    constexpr int bits_to_bytes(int bits);

    template<int BIT_CNT>
    struct StaticBitset {
        Byte bytes[bits_to_bytes(BIT_CNT)];
    };

    bool init_mem_arena(MemArena& arena, const int size);
    void clean_mem_arena(MemArena& arena);
    void* push_to_mem_arena(MemArena& arena, const int size, const int alignment);
    void reset_mem_arena(MemArena& arena);

    int get_first_inactive_bit_index(const Byte* const bytes, const int bitCnt);
    bool are_all_bits_active(const Byte* const bytes, const int bitCnt);

    constexpr bool is_power_of_two(const int n) {
        assert(n >= 0);
        return n && !(n & (n - 1));
    }

    constexpr int kilobytes_to_bytes(int kb) {
        return kb << 10;
    }

    constexpr int megabytes_to_bytes(int mb) {
        return mb << 20;
    }

    constexpr int gigabytes_to_bytes(int gb) {
        return gb << 30;
    }

    constexpr int bits_to_bytes(int bits) {
        return ((bits + 7) & ~7) / 8;
    }

    constexpr int align_forward(const int n, const int alignment) {
        assert(n >= 0);
        assert(is_power_of_two(alignment));
        return (n + alignment - 1) & ~(alignment - 1);
    }

    template<typename T>
    T* alloc(const int cnt = 1) {
        return static_cast<T*>(malloc(sizeof(T) * cnt));
    }

    template<typename T>
    T* alloc_zeroed(const int cnt = 1) {
        return static_cast<T*>(calloc(cnt, sizeof(T)));
    }

    template<typename T>
    void zero_out(T& data) {
        memset(&data, 0, sizeof(data));
    }

    template<typename T>
    bool is_zero(const T& data) {
        const auto dataBytes = reinterpret_cast<const Byte*>(&data);

        for (int i = 0; i < sizeof(T); ++i) {
            if (dataBytes[i]) {
                return false;
            }
        }

        return true;
    }

    template<typename T>
    T* push_to_mem_arena(MemArena& arena, const int cnt) {
        assert(arena.buf);

        const int pushSize = sizeof(T) * cnt;
        const int offsAligned = align_forward(arena.offs, alignof(T));
        const int offsNext = offsAligned + pushSize;

        if (offsNext > arena.size) {
            return nullptr;
        }

        arena.offs = offsNext;

        return reinterpret_cast<T*>(static_cast<Byte*>(arena.buf) + offsAligned);
    }

    inline void activate_bit(Byte* const bytes, const int bitIndex) {
        bytes[bitIndex / 8] |= 1 << (bitIndex % 8);
    }

    inline void activate_bit(Bitset& activity, const int bitIndex) {
        activate_bit(activity.bytes, bitIndex);
    }

    template<int BIT_CNT>
    inline void activate_bit(StaticBitset<BIT_CNT>& activity, const int bitIndex) {
        activate_bit(activity.bytes, bitIndex);
    }

    inline void deactivate_bit(Byte* const bytes, const int bitIndex) {
        bytes[bitIndex / 8] &= ~(1 << (bitIndex % 8));
    }

    inline void deactivate_bit(Bitset& activity, const int bitIndex) {
        deactivate_bit(activity.bytes, bitIndex);
    }

    template<int BIT_CNT>
    inline void deactivate_bit(StaticBitset<BIT_CNT>& activity, const int bitIndex) {
        deactivate_bit(activity.bytes, bitIndex);
    }

    inline void clear_bits(Byte* const bytes, const int bitCnt) {
        memset(bytes, 0, bits_to_bytes(bitCnt));
    }

    inline void clear_bits(Bitset& activity) {
        clear_bits(activity.bytes, activity.bitCnt);
    }

    template<int BIT_CNT>
    inline void clear_bits(StaticBitset<BIT_CNT>& activity) {
        clear_bits(activity.bytes, BIT_CNT);
    }

    inline bool is_bit_active(const Byte* const bytes, const int bitIndex) {
        return bytes[bitIndex / 8] & (1 << (bitIndex % 8));
    }

    inline bool is_bit_active(const Bitset& activity, const int bitIndex) {
        return is_bit_active(activity.bytes, bitIndex);
    }

    template<int BIT_CNT>
    inline bool is_bit_active(const StaticBitset<BIT_CNT>& activity, const int bitIndex) {
        return is_bit_active(activity.bytes, bitIndex);
    }

    inline int get_first_inactive_bit_index(const Bitset& activity) {
        return get_first_inactive_bit_index(activity.bytes, activity.bitCnt);
    }

    template<int BIT_CNT>
    inline int get_first_inactive_bit_index(const StaticBitset<BIT_CNT>& activity) {
        return get_first_inactive_bit_index(activity.bytes, BIT_CNT);
    }

    inline bool are_all_bits_active(const Bitset& activity) {
        return are_all_bits_active(activity.bytes, activity.bitCnt);
    }

    template<int BIT_CNT>
    inline bool are_all_bits_active(const StaticBitset<BIT_CNT>& activity) {
        return are_all_bits_active(activity.bytes, BIT_CNT);
    }
}
