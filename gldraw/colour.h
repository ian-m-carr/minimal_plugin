//
// Created by icarr on 25/01/2023.
//

#pragma once

#include <cstdint>

namespace gldraw {
    struct colour {
        uint8_t r{};
        uint8_t g{};
        uint8_t b{};
        uint8_t a{};

        colour() = default;
        colour(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}
    };

    static colour colour_from_floats(float r, float g, float b, float a) {
        return gldraw::colour(r * 255, g * 255, b * 255, a * 255);
    }

    const gldraw::colour COL_WHITE = {255, 255, 255, 255};
    const gldraw::colour COL_BLACK = {0, 0, 0, 255};

    const gldraw::colour COL_RED = {255, 0, 0, 255};
    const gldraw::colour COL_GREEN = {0, 255, 0, 255};
    const gldraw::colour COL_BLUE = {0, 0, 255, 255};

    const gldraw::colour COL_YELLOW = {255, 255, 0, 255};

    const gldraw::colour COL_TRANSP = {0, 0, 0, 0};
}