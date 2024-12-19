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

    union Vec3D {
        struct {
            float x;
            float y;
            float z;
        };

        struct {
            float r;
            float g;
            float b;
        };

        Vec3D operator+(const Vec3D& other) const {
            return {x + other.x, y + other.y, z + other.z};
        }

        Vec3D operator-(const Vec3D& other) const {
            return {x - other.x, y - other.y, z - other.z};
        }

        Vec3D operator*(const float scalar) const {
            return {x * scalar, y * scalar, z * scalar};
        }

        Vec3D operator/(const float scalar) const {
            return {x / scalar, y / scalar, z / scalar};
        }

        Vec3D& operator+=(const Vec3D& other) {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        Vec3D& operator-=(const Vec3D& other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        Vec3D& operator*=(const float scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

        Vec3D& operator/=(const float scalar) {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            return *this;
        }

        bool operator==(const Vec3D& other) const {
            return x == other.x && y == other.y && z == other.z;
        }

        bool operator!=(const Vec3D& other) const {
            return !(*this == other);
        }
    };

    union Vec4D {
        struct {
            float x;
            float y;
            float z;
            float w;
        };

        struct {
            float r;
            float g;
            float b;
            float a;
        };

        Vec4D operator+(const Vec4D& other) const {
            return {x + other.x, y + other.y, z + other.z, w + other.w};
        }

        Vec4D operator-(const Vec4D& other) const {
            return {x - other.x, y - other.y, z - other.z, w - other.w};
        }

        Vec4D operator*(const float scalar) const {
            return {x * scalar, y * scalar, z * scalar, w * scalar};
        }

        Vec4D operator/(const float scalar) const {
            return {x / scalar, y / scalar, z / scalar, w / scalar};
        }

        Vec4D& operator+=(const Vec4D& other) {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;
            return *this;
        }

        Vec4D& operator-=(const Vec4D& other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;
            return *this;
        }

        Vec4D& operator*=(const float scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;
            return *this;
        }

        Vec4D& operator/=(const float scalar) {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            w /= scalar;
            return *this;
        }

        bool operator==(const Vec4D& other) const {
            return x == other.x && y == other.y && z == other.z && w == other.w;
        }

        bool operator!=(const Vec4D& other) const {
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

        float get_right() const {
            return x + width;
        }

        float get_bottom() const {
            return y + height;
        }

        Vec2D get_pos() const {
            return {x, y};
        }

        Vec2D get_size() const {
            return {width, height};
        }

        Vec2D get_center() const {
            return {x + (width / 2), y + (height / 2)};
        }
    };

    struct Rect {
        int x;
        int y;
        int width;
        int height;

        operator RectFloat() const {
            return {static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height)};
        }

        int get_right() const {
            return x + width;
        }

        int get_bottom() const {
            return y + height;
        }

        Pt2D get_pos() const {
            return {x, y};
        }

        Pt2D get_size() const {
            return {width, height};
        }

        Pt2D get_center() const {
            return {x + (width / 2), y + (height / 2)};
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

    inline Vec2D normalized(const Vec2D vec) {
        const float mag = calc_mag(vec);
        return mag ? vec / mag : Vec2D {};
    }

    inline int rect_right(const Rect& rect) {
        return rect.x + rect.width;
    }

    inline float rect_right(const RectFloat& rect) {
        return rect.x + rect.width;
    }

    inline int rect_bottom(const Rect& rect) {
        return rect.y + rect.height;
    }

    inline float rect_bottom(const RectFloat& rect) {
        return rect.y + rect.height;
    }

    inline bool do_rects_intersect(const RectFloat& a, const RectFloat& b) {
        return a.x < b.x + b.width && a.x + a.width > b.x && a.y < b.y + b.height && a.y + a.height > b.y;
    }
}
