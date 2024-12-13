#pragma once

#include <math.h>
#include <string.h>

namespace zf3 {

constexpr float gk_pi = 3.14159265358979323846f;

template<typename T>
constexpr T min(T x, T y) {
    return (x < y) ? x : y;
}

template<typename T>
constexpr T max(T x, T y) {
    return (x > y) ? x : y;
}

struct Vec2D {
    float x;
    float y;
};

struct Vec2DInt {
    int x;
    int y;
};

struct Matrix4x4 {
    float elems[4][4];
};

struct Rect {
    int x;
    int y;
    int width;
    int height;
};

struct Range {
    int begin;
    int end;
};

void init_identity_matrix_4x4(Matrix4x4* const mat);
void init_ortho_matrix_4x4(Matrix4x4* const mat, const float left, const float right, const float bottom, const float top, const float near, const float far);

}
