//
// Created by icarr on 22/01/2023.
//

#pragma once

#include <glmath/vectors.h>

namespace gldraw {
    struct rect {
        glmath::vec2f pos{};
        glmath::vec2f size{};

        bool contains_point(const glmath::vec2f &point) const {
            glmath::vec2f delta = pos - point;
            return (-delta.x >= 0 && -delta.x <= size.x) && (-delta.y >= 0 && -delta.y <= size.y);
        }

        rect inflate(float x, float y) const {
            return {{pos.x - x,      pos.y - y},
                    {size.x + 2 * x, size.y + 2 * y}};
        }
    };
}