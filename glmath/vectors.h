//
// Created by icarr on 10/07/2022.
//

#pragma once

#define _USE_MATH_DEFINES

#include <type_traits>
#include <cassert>
#include <math.h>
#include <array>
#include <vector>

#include <cmath>

namespace glmath {
    struct vec2f {
        float x;
        float y;

        constexpr vec2f() : x(0), y(0) {}
        constexpr vec2f(float x, float y, float z = 0) : x(x), y(y) {}
        constexpr vec2f(float scalar) : x(scalar), y(scalar) {}

        constexpr vec2f operator*(const vec2f &other) const {
            return vec2f(x * other.x, y * other.y);
        }

        constexpr vec2f operator*(const float scalar) const {
            return vec2f(x * scalar, y * scalar);
        }

        constexpr vec2f operator/(float scalar) const {
            return vec2f(x / scalar, y / scalar);
        }

        constexpr vec2f operator+(const vec2f &other) const {
            return vec2f(x + other.x, y + other.y);
        }

        constexpr vec2f operator-(const vec2f &other) const {
            return vec2f(x - other.x, y - other.y);
        }

        constexpr float dot(const vec2f &other) const {
            return x * other.x + y * other.y;
        }

        // cross product is only valid in 3d!

        constexpr vec2f normalize() const {
            return *this / std::sqrt(dot(*this));
        }

        constexpr float magnitude2() const {
            return x * x + y * y;
        }

        inline float magnitude() const {
            return sqrt(magnitude2());
        }

        bool operator==(const vec2f &rhs) const {
            return std::tie(x, y) == std::tie(rhs.x, rhs.y);
        }

        bool operator!=(const vec2f &rhs) const {
            return !(rhs == *this);
        }

        bool operator<(const vec2f &rhs) const {
            return std::tie(x, y) < std::tie(rhs.x, rhs.y);
        }

        bool operator>(const vec2f &rhs) const {
            return rhs < *this;
        }

        bool operator<=(const vec2f &rhs) const {
            return !(rhs < *this);
        }

        bool operator>=(const vec2f &rhs) const {
            return !(*this < rhs);
        }
    };
    // verify should be trivially copyable!
    static_assert(std::is_trivially_copyable<vec2f>::value);

    struct vec3f {
        float x{};
        float y{};
        float z{};

        constexpr vec3f() : x(0), y(0), z(0) {}
        constexpr vec3f(float x, float y, float z = 0) : x(x), y(y), z(z) {}
        constexpr vec3f(float scalar) : x(scalar), y(scalar), z(scalar) {}
        vec3f(const vec2f &other) : x(other.x), y(other.y), z(0) {}

        constexpr vec3f operator*(const vec3f &other) const {
            return vec3f(x * other.x, y * other.y, z * other.z);
        }

        constexpr vec3f operator*(const float scalar) const {
            return vec3f(x * scalar, y * scalar, z * scalar);
        }

        constexpr vec3f operator/(float scalar) const {
            return vec3f(x / scalar, y / scalar, z / scalar);
        }

        constexpr vec3f operator+(const vec3f &other) const {
            return vec3f(x + other.x, y + other.y, z + other.z);
        }

        constexpr vec3f operator-(const vec3f &other) const {
            return vec3f(x - other.x, y - other.y, z - other.z);
        }

        constexpr float dot(const vec3f &other) const {
            return x * other.x + y * other.y + z * other.z;
        }

        constexpr vec3f cross(const vec3f &c) {
            return vec3f(y * c.z - c.y * z, z * c.x - c.z * x, x * c.y - c.x * y);
        }

        constexpr vec3f normalize() const {
            return *this / std::sqrt(dot(*this));
        }

        constexpr float magnitude2() const {
            return x * x + y * y + z * z;
        }

        inline float magnitude() const {
            return sqrt(magnitude2());
        }

        bool operator==(const vec3f &rhs) const {
            return std::tie(x, y, z) == std::tie(rhs.x, rhs.y, rhs.z);
        }

        bool operator!=(const vec3f &rhs) const {
            return !(rhs == *this);
        }
        bool operator<(const vec3f &rhs) const {
            return std::tie(x, y, z) < std::tie(rhs.x, rhs.y, rhs.z);
        }
        bool operator>(const vec3f &rhs) const {
            return rhs < *this;
        }
        bool operator<=(const vec3f &rhs) const {
            return !(rhs < *this);
        }
        bool operator>=(const vec3f &rhs) const {
            return !(*this < rhs);
        }
    };
    // verify should be trivially copyable!
    static_assert(std::is_trivially_copyable<vec3f>::value);

    struct vec4f {
        float x;
        float y;
        float z;
        float w;

        constexpr vec4f() : x(0), y(0), z(0), w(0) {}
        // extend vec3f to vec4f defaults to homogeneous point,w=1, use w=0 for a vector
        vec4f(const vec2f &other, float w = 1.0f) : x(other.x), y(other.y), z(0), w(w) {}
        constexpr vec4f(const vec3f &other, float w = 1.0f) : x(other.x), y(other.y), z(other.z), w(w) {}
        constexpr vec4f(float x, float y, float z = 0, float w = 1) : x(x), y(y), z(z), w(w) {}

        constexpr vec4f(float scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {}

        constexpr vec4f operator*(const vec4f &other) const {
            return vec4f(x * other.x, y * other.y, z * other.z, w * other.w);
        }

        constexpr vec4f operator*(const float scalar) const {
            return vec4f(x * scalar, y * scalar, z * scalar, w * scalar);
        }

        constexpr vec4f operator/(float scalar) const {
            return vec4f(x / scalar, y / scalar, z / scalar, w / scalar);
        }

        constexpr vec4f operator+(const vec4f &other) const {
            return vec4f(x + other.x, y + other.y, z + other.z, w + other.w);
        }

        constexpr vec4f operator-(const vec4f &other) const {
            return vec4f(x - other.x, y - other.y, z - other.z, w - other.w);
        }

        constexpr float dot(const vec4f &other) const {
            return x * other.x + y * other.y + z * other.z + w * other.w;
        }

        // no cross product for vec4!

        constexpr vec4f normalize() const {
            return *this / std::sqrt(dot(*this));
        }

        constexpr float magnitude2() const {
            return x * x + y * y + z * z + w * w;
        }

        inline float magnitude() const {
            return sqrt(magnitude2());
        }

        // divsion by w homogeneous conversion
        vec3f to_vec3_hmgns() const {
            if (w != 0 && w != 1.0f) {
                return vec3f(x / w, y / w, z / w);
            }
            return xyz();
        }

        vec2f to_vec2_hmgns() const {
            if (w != 0 && w != 1.0f) {
                return vec2f(x / w, y / w);
            }
            return xy();
        }

        // no division by w
        vec3f xyz() const {
            return vec3f(x, y, z);
        }

        // no division by w
        vec2f xy() const {
            return vec2f(x, y);
        }

        bool operator==(const vec4f &rhs) const {
            return std::tie(x, y, z, w) == std::tie(rhs.x, rhs.y, rhs.z, rhs.w);
        }

        bool operator!=(const vec4f &rhs) const {
            return !(rhs == *this);
        }

        bool operator<(const vec4f &rhs) const {
            return std::tie(x, y, z, w) < std::tie(rhs.x, rhs.y, rhs.z, rhs.w);
        }

        bool operator>(const vec4f &rhs) const {
            return rhs < *this;
        }

        bool operator<=(const vec4f &rhs) const {
            return !(rhs < *this);
        }

        bool operator>=(const vec4f &rhs) const {
            return !(*this < rhs);
        }
    };
    // verify should be trivially copyable!
    static_assert(std::is_trivially_copyable<vec4f>::value);

    template<typename TVec>
    constexpr float dot(const TVec &v1, const TVec &v2) {
        return v1.dot(v2);
    }

    template<typename T>
    static T inverse_sqrt(const T &x) {
        return static_cast<T>(1) / sqrt(x);
    }

    template<typename TVec>
    TVec normalize(const TVec &v) {
        return v * inverse_sqrt(dot(v, v));
    }
}