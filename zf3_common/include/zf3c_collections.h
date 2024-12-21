#pragma once

#include <zf3c_mem.h>

namespace zf3 {
    using Byte = unsigned char;

    template<typename T>
    struct Array {
        T* elems;
        int len;

        T& operator[](const int index) {
            assert(elems);
            assert(index >= 0 && index < len);
            return elems[index];
        }

        const T& operator[](const int index) const {
            assert(elems);
            assert(index >= 0 && index < len);
            return elems[index];
        }
    };

    template<typename T, int LEN>
    struct StaticArray {
        T elems[LEN];

        T& operator[](const int index) {
            assert(index >= 0 && index < LEN);
            return elems[index];
        }

        const T& operator[](const int index) const {
            assert(index >= 0 && index < LEN);
            return elems[index];
        }
    };

    template<typename T>
    struct ActivityArray {
        T* elems;
        Byte* activity;
        int len;

        T& operator[](const int index) {
            assert(elems);
            assert(activity);
            assert(index >= 0 && index < len);
            assert(is_bit_active(activity, index));
            return elems[index];
        }

        const T& operator[](const int index) const {
            assert(elems);
            assert(activity);
            assert(index >= 0 && index < len);
            assert(is_bit_active(activity, index));
            return elems[index];
        }
    };

    template<typename T, int LEN>
    struct StaticActivityArray {
        T elems[LEN];
        Byte activity[bits_to_bytes(LEN)];

        T& operator[](const int index) {
            assert(index >= 0 && index < LEN);
            assert(is_bit_active(activity, index));
            return elems[index];
        }

        const T& operator[](const int index) const {
            assert(index >= 0 && index < LEN);
            assert(is_bit_active(activity, index));
            return elems[index];
        }
    };

    template<typename T>
    struct List {
        T* elems;
        int cap;
        int len;

        T& operator[](const int index) {
            assert(elems);
            assert(index >= 0 && index < cap);
            return elems[index];
        }

        const T& operator[](const int index) const {
            assert(elems);
            assert(index >= 0 && index < cap);
            return elems[index];
        }
    };

    template<typename T, int CAP>
    struct StaticList {
        T elems[CAP];
        int len;

        T& operator[](const int index) {
            assert(index >= 0 && index < len);
            return elems[index];
        }

        const T& operator[](const int index) const {
            assert(index >= 0 && index < len);
            return elems[index];
        }
    };

    template<typename T>
    Array<T> push_array(MemArena& memArena, const int len) {
        assert(len > 0);

        return {
            .elems = push_to_mem_arena<T>(memArena, len),
            .len = len
        };
    }

    template<typename T>
    ActivityArray<T> push_activity_array(MemArena& memArena, const int len) {
        assert(len > 0);

        return {
            .elems = push_to_mem_arena<T>(memArena, len),
            .activity = push_to_mem_arena<Byte>(memArena, bits_to_bytes(len)),
            .len = len
        };
    }

    template<typename T>
    List<T> push_list(MemArena& memArena, const int cap) {
        assert(cap > 0);

        return {
            .elems = push_to_mem_arena<T>(memArena, cap),
            .cap = cap
        };
    }

    template<typename T>
    void append_to_list(List<T>& list, const T& elem) {
        assert(list.len < list.cap);

        list.elems[list.len] = elem;
        ++list.len;
    }

    template<typename T>
    void insert_into_list(List<T>& list, const int index, const T& elem) {
        assert(index >= 0 && index <= list.len);
        assert(list.len < list.cap);

        for (int i = list.len; i > index; --i) {
            list.elems[i] = list.elems[i - 1];
        }

        list.elems[index] = elem;
        ++list.len;
    }
}
