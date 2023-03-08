//
// Created by icarr on 15/01/2023.
//

#pragma once

#include <glad/gl.h>

#include <gldraw/colour.h>
#include <glmath/vectors.h>
#include <glmath/matrices.h>

namespace gldraw {
    struct coloured_vertex {
        // location
        glmath::vec3f position{};
        // UV
        glmath::vec2f uv{};
        // colours
        gldraw::colour fore_colour{};

        coloured_vertex() = default;
        explicit coloured_vertex(glmath::vec3f position,
                                 glmath::vec2f uv = {0.0f, 0.0f},
                                 gldraw::colour fore_colour = {255, 255, 255, 255}) :
                position(position), fore_colour(fore_colour), uv(uv) {}

        coloured_vertex &apply_transform(const glmath::mat4x4 &transform) {
            position = (transform * position).to_vec3_hmgns();
            return *this;
        };

        static void map_vertex_attributes() {
            // these attributes match a shader layout like this:
            //        #version 330 core
            //        layout (location = 0) in vec3 aPos;
            //        layout (location = 1) in vec2 aTexCoord;
            //        layout (location = 2) in vec4 aColor;

            // position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(coloured_vertex), (void *) offsetof(coloured_vertex, position));
            glEnableVertexAttribArray(0);

            // texture coord attribute
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(coloured_vertex), (void *) offsetof(coloured_vertex, uv));
            glEnableVertexAttribArray(1);

            // fore_color attribute
            glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(coloured_vertex), (void *) offsetof(coloured_vertex, fore_colour));
            glEnableVertexAttribArray(2);
        }
    };

    GLuint get_coloured_vertex_shader();
}