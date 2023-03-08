//
// Created by icarr on 15/01/2023.
//

#include <stdexcept>
#include <format>

#include "coloured_vertex.h"

namespace gldraw {
    static GLuint __gauge_shader_id;

    GLuint get_coloured_vertex_shader() {
        if (__gauge_shader_id != 0) {
            return __gauge_shader_id;
        }

        char infoLog[512];

        const char *vs_str = R"term(
                #version 460 core
                layout (location = 0) in vec3 aPos;
                layout (location = 1) in vec2 aTexCoord;
                layout (location = 2) in vec4 aForeColor;

                uniform mat4 projection;
                uniform mat4 model;
                flat out vec4 ourForeColor;
                out vec2 TexCoord;

                void main(){
                    gl_Position = projection * model * vec4(aPos, 1.0);
                    ourForeColor = aForeColor;
                    TexCoord = aTexCoord;
                }
                )term";

        const char *fs_str = R"term(
                #version 460 core
                out vec4 FragColor;

                flat in vec4 ourForeColor;
                in vec2 TexCoord;

                uniform sampler2D our_texture;

                void main() {
                    FragColor = texture(our_texture, TexCoord) * ourForeColor;
                }
                )term";

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vs_str, nullptr);
        glCompileShader(vs);
        int success = -1;
        glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
        if (GL_TRUE != success) {
            glGetShaderInfoLog(vs, 512, nullptr, infoLog);
            throw std::runtime_error(std::format("Vertex shader compilation failed:\n{}", infoLog));
        }

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fs_str, nullptr);
        glCompileShader(fs);
        glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
        if (GL_TRUE != success) {
            glGetShaderInfoLog(fs, 512, nullptr, infoLog);
            throw std::runtime_error(std::format("Fragment shader compilation failed:\n{}", infoLog));
        }

        __gauge_shader_id = glCreateProgram();
        glAttachShader(__gauge_shader_id, vs);
        glAttachShader(__gauge_shader_id, fs);
        glLinkProgram(__gauge_shader_id);
        glGetProgramiv(__gauge_shader_id, GL_LINK_STATUS, &success);
        if (GL_TRUE != success) {
            glGetProgramInfoLog(__gauge_shader_id, 512, NULL, infoLog);
            throw std::runtime_error(std::format("Shader link  failed:\n{}", infoLog));
        }

        // we can delete the component shaders now we have linked the program
        glDeleteShader(vs);
        glDeleteShader(fs);

        return __gauge_shader_id;
    }
}