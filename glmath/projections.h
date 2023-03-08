//
// Created by icarr on 24/10/2022.
//

#pragma once

#include <cmath>
#include <array>

#include <glmath/matrices.h>

namespace glmath {
    /// @see gluPerspective https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluPerspective.xml
    static mat4x4 perspective(float fov_y, float aspect, float z_near, float z_far) {
        float const tan_half_fov_y = tan(fov_y / static_cast<float >(2));

        mat4x4 result(0);
        result[0].x = static_cast<float>(1) / (aspect * tan_half_fov_y);
        result[1].y = static_cast<float>(1) / (tan_half_fov_y);
        result[2].z = -(z_far + z_near) / (z_far - z_near);
        result[2].w = -static_cast<float>(1);
        result[3].w = -(static_cast<float >(2) * z_far * z_near) / (z_far - z_near);

        return result;
    }

    /// equivalent of gluOOrtho2D
    /// @see https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluOrtho2D.xml
    /// equivalent of ortho with near -1 and far 1
    static mat4x4 ortho(float left, float right, float bottom, float top) {
        mat4x4 result(1);
        result[0].x= static_cast<float>(2) / (right - left);
        result[1].y = static_cast<float>(2) / (top - bottom);
        result[2].z = -static_cast<float>(1);
        result[3].x = -(right + left) / (right - left);
        result[3].y = -(top + bottom) / (top - bottom);
        return result;
    }

    /// equivalent of glOrtho
    /// @see https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glOrtho.xml
    static mat4x4 ortho(float left, float right, float bottom, float top, float z_near, float z_far) {
        mat4x4 result(1);
        result[0].x = static_cast<float>(2) / (right - left);
        result[1].y = static_cast<float>(2) / (top - bottom);
        result[2].z = -static_cast<float>(2) / (z_far - z_near);
        result[3].x = -(right + left) / (right - left);
        result[3].y = -(top + bottom) / (top - bottom);
        result[3].z = -(z_far + z_near) / (z_far - z_near);
        return result;
    }
}