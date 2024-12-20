#pragma once

#include <assert.h>
#include <glad/glad.h>
#include <AL/al.h>
#include <zf3c.h>

namespace zf3 {
    using GLID = GLuint;
    using ALID = ALuint;

    template<typename T>
    struct List {
        T* elems;
        int len;
        int cap;
    };

    template<int BIT_CNT>
    struct Bitset {
        Byte bytes[bits_to_bytes(BIT_CNT)];
    };

    template<typename T, int LEN>
    struct ActivityList {
        T elems[LEN];
        Bitset<LEN> activity;
    };

    struct MemArena {
        void* buf;
        int size;
        int offs;
    };

    int get_first_inactive_bit_index(const Byte* const bytes, const int bitCnt);
    bool are_all_bits_active(const Byte* const bytes, const int bitCnt);

    bool init_mem_arena(MemArena& arena, const int size);
    void clean_mem_arena(MemArena& arena);
    void* push_to_mem_arena(MemArena& arena, const int size, const int alignment);
    void reset_mem_arena(MemArena& arena);

    constexpr bool is_power_of_two(const int n) {
        assert(n >= 0);
        return n && !(n & (n - 1));
    }

    constexpr int align_forward(const int n, const int alignment) {
        assert(n >= 0);
        assert(is_power_of_two(alignment));
        return (n + alignment - 1) & ~(alignment - 1);
    }

    template<typename T>
    void append(List<T>& list, const T& elem) {
        assert(list.len < list.cap);

        list.elems[list.len] = elem;
        ++list.len;
    }

    template<typename T>
    void insert(List<T>& list, const int index, const T& elem) {
        assert(index >= 0 && index <= list.len);
        assert(list.len < list.cap);

        for (int i = list.len; i > index; --i) {
            list.elems[i] = list.elems[i - 1];
        }

        list.elems[index] = elem;
        ++list.len;
    }

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
}
