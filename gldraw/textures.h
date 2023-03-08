//
// Created by icarr on 24/10/2022.
//

#pragma once

#include <iostream>
#include <glad/gl.h>
#include <stb/stb_image.h>

namespace gldraw {
    static void create_and_bind_texture(GLuint *texture_id) {
        if (texture_id != nullptr) {
            XPLMGenerateTextureNumbers(reinterpret_cast<int *>(texture_id), 1);
            XPLMBindTexture2d(*texture_id, 0);
        }
    }

    GLuint create_clamped_texture_from_image_file(const std::string &image_filename) {
        // load and create a texture
        // -------------------------
        unsigned int texture_id;

        create_and_bind_texture(&texture_id);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLfloat max_aniso = 0.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &max_aniso);
        // set the maximum!
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, max_aniso);

        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        unsigned char *data = stbi_load(image_filename.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
#if defined (DSA)
            glTextureStorage2D(texture_id, 1, GL_RGBA, width, height);
            glTextureSubImage2D(texture_id, 1, 0,0, width, height, nrChannels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateTextureMipmap(texture_id);
#else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, nrChannels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
#endif
        } else {
            XPLMDebugString(std::format("Failed to load texture: {}", stbi_failure_reason()).c_str());
        }
        // data has been transferred to the GPU we can loose it locally!
        stbi_image_free(data);

        return texture_id;
    }

    GLuint get_white_1x1_texture() {
        static GLuint __white_1x1 = 0;

        if (__white_1x1 == 0) {
            unsigned char white_data[] = {0xFF, 0xFF, 0xFF, 0xFF};

            create_and_bind_texture(&__white_1x1);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

#if defined (DSA)
            glTextureStorage2D(__white_1x1, 1, GL_RGBA8, 1, 1);
            glTextureSubImage2D(__white_1x1, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &white_data);
            glGenerateTextureMipmap(__white_1x1);
#else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA2, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &white_data);
            glGenerateMipmap(GL_TEXTURE_2D);
#endif
        }
        return __white_1x1;
    }
}