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

    struct Pt2D {
        int x;
        int y;
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

        RectFloat() = default;
        constexpr RectFloat(const float x, const float y, const float width, const float height) : x(x), y(y), width(width), height(height) {}
        constexpr RectFloat(const float width, const float height) : x(0.0f), y(0.0f), width(width), height(height) {}
        constexpr RectFloat(const Vec2D pos, const Vec2D size) : x(pos.x), y(pos.y), width(size.x), height(size.y) {}
    };

    struct Rect {
        int x;
        int y;
        int width;
        int height;

        Rect() = default;
        constexpr Rect(const int x, const int y, const int width, const int height) : x(x), y(y), width(width), height(height) {}
        constexpr Rect(const int width, const int height) : x(0), y(0), width(width), height(height) {}
        constexpr Rect(const Pt2D pos, const Pt2D size) : x(pos.x), y(pos.y), width(size.x), height(size.y) {}

        operator RectFloat() const {
            return {static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height)};
        }
    };

    struct Range {
        int begin;
        int end;
    };

    Matrix4x4 create_identity_matrix_4x4();
    Matrix4x4 create_ortho_matrix_4x4(const float left, const float right, const float bottom, const float top, const float near, const float far);

    constexpr Vec2D to_vec_2d(const Pt2D pt) {
        return {static_cast<float>(pt.x), static_cast<float>(pt.y)};
    }

    constexpr Pt2D to_pt_2d(const Vec2D vec) {
        return {static_cast<int>(vec.x), static_cast<int>(vec.y)};
    }

    template<typename T>
    constexpr T min(const T& a, const T& b) {
        return (a < b) ? a : b;
    }

    template<typename T>
    constexpr T max(const T& a, const T& b) {
        return (a > b) ? a : b;
    }

    template<typename T>
    constexpr T clamp(const T& value, const T& min, const T& max) {
        if (value < min) {
            return min;
        }

        if (value > max) {
            return max;
        }

        return value;
    }

    template<typename T>
    constexpr T lerp(const T& a, const T& b, const float t) {
        return a + ((b - a) * t);
    }

    inline float calc_mag(const Vec2D vec) {
        return sqrtf((vec.x * vec.x) + (vec.y * vec.y));
    }

    inline float calc_dist(const Vec2D a, const Vec2D b) {
        return calc_mag(b - a);
    }

    inline float calc_dir(const Vec2D a, const Vec2D b) {
        return atan2f(-(b.y - a.y), b.x - a.x);
    }

    inline Vec2D calc_len_dir(const float len, const float dir) {
        return {len * cosf(dir), len * -sinf(dir)};
    }

    inline Vec2D calc_normal(const Vec2D vec) {
        const float mag = calc_mag(vec);
        return mag ? vec / mag : Vec2D {};
    }

    constexpr Pt2D get_rect_pos(const Rect& rect) {
        return {rect.x, rect.y};
    }

    constexpr Vec2D get_rect_pos(const RectFloat& rect) {
        return {rect.x, rect.y};
    }

    constexpr Pt2D get_rect_size(const Rect& rect) {
        return {rect.width, rect.height};
    }

    constexpr Vec2D get_rect_size(const RectFloat& rect) {
        return {rect.width, rect.height};
    }

    constexpr int get_rect_right(const Rect& rect) {
        return rect.x + rect.width;
    }

    constexpr float get_rect_right(const RectFloat& rect) {
        return rect.x + rect.width;
    }

    constexpr int get_rect_bottom(const Rect& rect) {
        return rect.y + rect.height;
    }

    constexpr float get_rect_bottom(const RectFloat& rect) {
        return rect.y + rect.height;
    }

    constexpr Pt2D get_rect_center(const Rect& rect) {
        return {rect.x + (rect.width / 2), rect.y + (rect.height / 2)};
    }

    constexpr Vec2D get_rect_center(const RectFloat& rect) {
        return {rect.x + (rect.width / 2), rect.y + (rect.height / 2)};
    }

    constexpr bool do_rects_intersect(const RectFloat& a, const RectFloat& b) {
        return a.x < b.x + b.width && a.x + a.width > b.x && a.y < b.y + b.height && a.y + a.height > b.y;
    }
}
