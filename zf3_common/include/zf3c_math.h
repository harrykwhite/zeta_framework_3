#pragma once

#include <math.h>
#include <string.h>

namespace zf3 {

constexpr float gk_pi = 3.14159265358979323846f;

struct Vec2D {
    float x;
    float y;

    Vec2D operator+(const Vec2D& other) const {
        return {x + other.x, y + other.y};
    }

    Vec2D operator-(const Vec2D& other) const {
        return {x - other.x, y - other.y};
    }

    Vec2D operator*(const float scalar) const {
        return {x * scalar, y * scalar};
    }

    Vec2D operator/(const float scalar) const {
        return {x / scalar, y / scalar};
    }

    Vec2D& operator+=(const Vec2D& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vec2D& operator-=(const Vec2D& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vec2D& operator*=(const float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vec2D& operator/=(const float scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    bool operator==(const Vec2D& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Vec2D& other) const {
        return !(*this == other);
    }
};

struct Vec2DInt {
    int x;
    int y;

    operator Vec2D() const {
        return {static_cast<float>(x), static_cast<float>(y)};
    }
};

struct Matrix4x4 {
    float elems[4][4];

    float* operator[](const int row) {
        return elems[row];
    }
};

struct RectFloat {
    float x;
    float y;
    float width;
    float height;
};

struct Rect {
    int x;
    int y;
    int width;
    int height;

    operator RectFloat() const {
        return {static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height)};
    }
};

struct Range {
    int begin;
    int end;
};

template<typename T>
constexpr T get_min(const T& a, const T& b) {
    return (a < b) ? a : b;
}

template<typename T>
constexpr T get_max(const T& a, const T& b) {
    return (a > b) ? a : b;
}

template<typename T>
constexpr float clamp(const T& val, const T& min, const T& max) {
    if (val < min) {
        return min;
    }

    if (val > max) {
        return max;
    }

    return val;
}

template<typename T>
constexpr T calc_lerp(const T& a, const T& b, const float t) {
    return a + ((b - a) * t);
}

Matrix4x4 create_identity_matrix_4x4();
Matrix4x4 create_ortho_matrix_4x4(const float left, const float right, const float bottom, const float top, const float near, const float far);

inline float calc_mag(const Vec2D vec) {
    return sqrtf((vec.x * vec.x) + (vec.y * vec.y));
}

inline float calc_dist(const Vec2D a, const Vec2D b) {
    return calc_mag(b - a);
}

inline Vec2D calc_normal(const Vec2D vec) {
    const float mag = calc_mag(vec);
    return mag ? vec / mag : Vec2D {};
}

inline bool do_rects_intersect(const RectFloat a, const RectFloat b) {
    return a.x < b.x + b.width && a.x + a.width > b.x && a.y < b.y + b.height && a.y + a.height > b.y;
}

}
