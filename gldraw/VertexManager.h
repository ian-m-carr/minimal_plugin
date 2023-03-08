//
// Created by icarr on 19/10/2022.
//

#pragma once

#include <functional>
#include <vector>
#include <memory>

#include <glad/gl.h>

#include <gldraw/geom.h>
#include <gldraw/colour.h>
#include <glmath/vectors.h>
#include <glmath/matrices.h>

namespace gldraw {
    template<typename TVertex>
    class VertexManager {
    public:
        using vertex_type = TVertex;
    public:
        VertexManager(bool static_buffers = false) : _static_buffers(static_buffers) {
            glGenVertexArrays(1, &_VAO);
            glGenBuffers(1, &_VBO);
            glGenBuffers(1, &_EBO);
        }

        ~VertexManager() {
            glDeleteVertexArrays(1, &_VAO);
            glDeleteBuffers(1, &_VBO);
            glDeleteBuffers(1, &_EBO);
        }

        VertexManager(const VertexManager &other) = delete;
        VertexManager &operator=(const VertexManager &other) = delete;

        // Move assignment operator.
        VertexManager &operator=(VertexManager &&other) noexcept {
            if (this != &other) {
                _VBO = other._VBO;
                other._VBO = 0;
                _VAO = other._VAO;
                other._VAO = 0;
                _EBO = other._EBO;
                other._EBO = 0;

                _vertices = std::move(other._vertices);
                _indices = std::move(other._indices);
            }
            return *this;
        }

        VertexManager(VertexManager &&other) noexcept {
            *this = std::move(other);
        };

    public:
        unsigned int get_element_count() const {
            return _indices.size();
        }

    public:
        void clear() {
            _indices.clear();
            _vertices.clear();
        }

        void add_quad(const gldraw::rect &rct, const gldraw::colour &colour = gldraw::COL_WHITE,
                      std::function<const void(TVertex &vertex)> vertex_callback = nullptr) {
            unsigned int indx = _vertices.size();

            /// bl, tl, tr, br
            _vertices.emplace_back(gldraw::coloured_vertex(rct.pos, {0.0f, 0.0f}, colour));
            _vertices.emplace_back(gldraw::coloured_vertex(rct.pos + glmath::vec2f(0.0f, rct.size.y), {0.0f, 1.0f}, colour));
            _vertices.emplace_back(gldraw::coloured_vertex(rct.pos + rct.size, {1.0f, 1.0f}, colour));
            _vertices.emplace_back(gldraw::coloured_vertex(rct.pos + glmath::vec2f(rct.size.x, 0.0f), {1.0f, 0.0f}, colour));

            if (vertex_callback) {
                vertex_callback(_vertices[_vertices.size() - 4]);
                vertex_callback(_vertices[_vertices.size() - 3]);
                vertex_callback(_vertices[_vertices.size() - 2]);
                vertex_callback(_vertices[_vertices.size() - 1]);
            }

#if defined CCW_WINDING
            // 0, 3, 1 first triangle
            _indices.push_back(indx);
            _indices.push_back(indx + 3);
            _indices.push_back(indx + 1);

            // 1, 3, 2 second triangle
            _indices.push_back(indx + 1);
            _indices.push_back(indx + 3);
            _indices.push_back(indx + 2);
#else
            // 0, 1, 3 first triangle
            _indices.push_back(indx);
            _indices.push_back(indx + 1);
            _indices.push_back(indx + 3);

            // 1, 2, 3 second triangle
            _indices.push_back(indx + 1);
            _indices.push_back(indx + 2);
            _indices.push_back(indx + 3);
#endif
        }

    public:
        void gen_buffers() {
            glBindVertexArray(_VAO);

            glBindBuffer(GL_ARRAY_BUFFER, _VBO);

            // do we re-use the buffer or generate a new larger one?
            if (_vertices.size() <= _vert_buf_size && !_static_buffers) {
                glBufferSubData(GL_ARRAY_BUFFER, 0, _vertices.size() * sizeof(TVertex), _vertices.data());
            } else {
                // need to allocate a new larger buffer
                glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(TVertex), _vertices.data(),
                             _static_buffers ? GL_STATIC_DRAW : GL_STREAM_DRAW);
                _vert_buf_size = _vertices.size();

                TVertex::map_vertex_attributes();
            }

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);

            // do we re-use the buffer or generate a new larger one?
            if (_indices.size() <= _ind_buf_size && !_static_buffers) {
                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, _indices.size() * sizeof(unsigned int), _indices.data());
            } else {
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), _indices.data(),
                             _static_buffers ? GL_STATIC_DRAW : GL_STREAM_DRAW);
                _ind_buf_size = _indices.size();
            }
        }

        [[nodiscard]] unsigned int get_vbo() const { return _VBO; }
        [[nodiscard]] unsigned int get_vao() const { return _VAO; }
        [[nodiscard]] unsigned int get_ebo() const { return _EBO; }

        [[nodiscard]] const void *const get_indicies() const { return _indices.data(); }

    private:
        bool _static_buffers;
        unsigned int _VBO{}, _VAO{}, _EBO{};
        std::vector<vertex_type> _vertices;
        std::vector<unsigned int> _indices;

        size_t _vert_buf_size{};
        size_t _ind_buf_size{};
    };
}