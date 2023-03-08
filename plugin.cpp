//
// Created by icarr on 09/12/2022.
//

#include <stdio.h>
#include <string>
#include <filesystem>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION

#include <XPLMGraphics.h>
#include <XPLMDisplay.h>
#include <XPLMUtilities.h>
#include <XPLMPlanes.h>

#include <glmath/projections.h>
#include <glmath/matrices.h>

#include <gldraw/shaders/coloured_vertex.h>
#include <gldraw/VertexManager.h>
#include <gldraw/textures.h>

#define PER_FRAME_GEOM
#define USE_STATIC_BUFFERS_ONLY

static XPLMAvionicsID __avionics_callback_id_pfd1;
static XPLMAvionicsID __avionics_callback_id_pfd2;
static XPLMAvionicsID __avionics_callback_id_mfd;

static int __avionics_count;

static GLuint _grid_texture_id_;
static std::unique_ptr<gldraw::VertexManager<gldraw::coloured_vertex>> _vmgr_;

static bool _buffers_generated_ = false;

#if defined(GLAD_OPTION_GL_DEBUG)
static void pre_call_gl_callback(const char *name, GLADapiproc apiproc, int len_args, ...) {
    GLAD_UNUSED(len_args);

    if (apiproc == NULL) {
        XPLMDebugString(std::format("GLAD: {} is NULL", name).c_str());
        return;
    }
    if (glad_glGetError == NULL) {
        XPLMDebugString("GLAD: glGetError is NULL");
        return;
    }

    (void) glad_glGetError();
}
static void post_call_gl_callback(void *ret, const char *name, GLADapiproc apiproc, int len_args, ...) {
    GLenum error_code;

    GLAD_UNUSED(ret);
    GLAD_UNUSED(apiproc);
    GLAD_UNUSED(len_args);

    error_code = glad_glGetError();

    if (error_code != GL_NO_ERROR) {
        XPLMDebugString(std::format("GLAD: code: {} in {}", error_code, name).c_str());
    }
}
#endif

#if !defined (NDEBUG)
void GLAPIENTRY
MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                GLsizei length, const GLchar *message, const void *userParam) {
    XPLMDebugString(std::format("GL type: {} severity: {} message: {}", type, severity, message).c_str());
}
#endif

static std::string resolve_resource(const std::string_view &resource_name) {
    char xp_filename[256];
    char xp_path[512];

    // search the aircraft folder first
    XPLMGetNthAircraftModel(XPLM_USER_AIRCRAFT, xp_filename, xp_path);

    std::filesystem::path aircraft_file(xp_path);

    if (std::filesystem::exists(aircraft_file)) {
        std::filesystem::path aircraft_folder = aircraft_file.parent_path();
        if (std::filesystem::exists(aircraft_folder)) {
            for (const auto &file: std::filesystem::directory_iterator(aircraft_folder)) {
                if (file.path().filename().string() == resource_name) {
                    return file.path().string();
                }
            }
        }
    }

    // then search the xp resources
    XPLMGetSystemPath(xp_path);
    std::filesystem::path system_folder(xp_path);
    if (std::filesystem::exists(aircraft_file)) {
        std::filesystem::path resources_folder = system_folder / "Resources";
        if (std::filesystem::exists(aircraft_file)) {
            for (const auto &file: std::filesystem::recursive_directory_iterator(resources_folder)) {
                if (file.path().filename().string() == resource_name) {
                    return file.path().string();
                }
            }
        }
    }

    // then search the plugin folder

    // resource not found
    throw std::runtime_error(std::format("Resource {} not found in aircraft or Resources folder", resource_name));
}

static int avionics_draw_callback(XPLMDeviceID inDeviceID, int inIsBefore, void *inRefcon) {
    // only draw in the after
    if (!inIsBefore) {
        // grab the current winding order
        GLint front_face;
        glGetIntegerv(GL_FRONT_FACE, &front_face);

#if defined CCW_WINDING
        // set the winding order to OpenGL standard
        glFrontFace(GL_CCW);

        // check the new front_face setting is applied
        GLint front_face1;
        glGetIntegerv(GL_FRONT_FACE, &front_face1);
#endif

        XPLMSetGraphicsState(0/*GL_FOG*/, 1/*GL_TEXTURE_2D*/, 0/*GL_LIGHT0*/, 0/*GL_ALPHA_TEST*/, 1/*GL_BLEND*/, 0/*GL_DEPTH_TEST*/, 0/*glDepthMask(GL_TRUE)*/);

        // render the content

        // the shader program
        GLuint g1000_shader = gldraw::get_coloured_vertex_shader();
        glUseProgram(g1000_shader);
        glUniform1i(glGetUniformLocation(g1000_shader, "our_texture"), 0);

        // orthographic pixel projection
        glmath::mat4x4 fb_projection2 = glmath::ortho(0, 1024, 0, 768);

        // set the projection matrix for the shader
        glUniformMatrix4fv(glGetUniformLocation(g1000_shader, "projection"), 1, GL_FALSE, fb_projection2.as_pointer_to_float());

        // setup the object transform
        glmath::mat4x4 model_mat = glmath::mat4x4::identity;

        // store the model transform to the shader
        glUniformMatrix4fv(glGetUniformLocation(g1000_shader, "model"), 1, GL_FALSE, model_mat.as_pointer_to_float());

        // render the vertex manager content

        // bind textures on corresponding texture units
        XPLMBindTexture2d(_grid_texture_id_, 0);

        // render the rectangle
        if (_vmgr_) {
#if defined PER_FRAME_GEOM
            _vmgr_->clear();
            // rectangle 1024x768 and uv 0,0 to 1,1
            _vmgr_->add_quad({{0.0f,    0.0f},
                              {1024.0f, 768.0f}});
            _vmgr_->gen_buffers();
#else
            if (!_buffers_generated_) {
                _vmgr_->gen_buffers();
                _buffers_generated_ = true;
            }
#endif
            glBindVertexArray(_vmgr_->get_vao());
            glDrawElements(GL_TRIANGLES, _vmgr_->get_element_count(), GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);

#if defined CCW_WINDING
        // set the winding order back to stored value
        glFrontFace(front_face);
#endif
    }
    return 1;
}

PLUGIN_API int XPluginStart(char *name, char *sig, char *desc) {
    XPLMDebugString("XPluginStart");
    strcpy(name, "Zink Texturing test");
    strcpy(sig, "imc.test.zink_texture_example");
    strcpy(desc, "check uv mapping under zink.");

    if (!gladLoaderLoadGL()) {
        XPLMDebugString("Failed to initialize glad");
    }

#if !defined (NDEBUG)
    glDebugMessageCallback(MessageCallback, 0);
#endif

#if defined(GLAD_OPTION_GL_DEBUG)
    gladSetGLPreCallback(pre_call_gl_callback);
    gladSetGLPostCallback(post_call_gl_callback);
#endif

    try {
        _grid_texture_id_ = gldraw::create_clamped_texture_from_image_file(resolve_resource("uvgrid.jpg"));

        _vmgr_ = std::make_unique<gldraw::VertexManager<gldraw::coloured_vertex>>(
#if defined(USE_STATIC_BUFFERS_ONLY)
                true
#endif
        );

        if (_vmgr_) {
            // rectangle 1024x768 and uv 0,0 to 1,1
            _vmgr_->add_quad({{0.0f,    0.0f},
                              {1024.0f, 768.0f}});
        }
    } catch (const std::exception &ex) {
        XPLMDebugString(std::format("exception configuring plugin: {}", ex.what()).c_str());
    }

    return 1;
}

PLUGIN_API void XPluginStop(void) {
    XPLMDebugString("XPluginStop\n");
}

PLUGIN_API int XPluginEnable(void) {
    XPLMDebugString("XPluginEnable\n");

    XPLMCustomizeAvionics_t params;
    params.structSize = sizeof(XPLMCustomizeAvionics_t);
    params.deviceId = xplm_device_G1000_PFD_1;
    params.drawCallbackBefore = nullptr;
    params.drawCallbackAfter = avionics_draw_callback;

    __avionics_callback_id_pfd1 = XPLMRegisterAvionicsCallbacksEx(&params);
    if (__avionics_callback_id_pfd1 == nullptr) {
        XPLMDebugString("PFD1: XPLMRegisterAvionicsCallbacksEx failed!\n");
    } else {
        XPLMDebugString("PFD1: XPLMRegisterAvionicsCallbacksEx success!\n");
    }

    params.deviceId = xplm_device_G1000_PFD_2;
    __avionics_callback_id_pfd2 = XPLMRegisterAvionicsCallbacksEx(&params);
    if (__avionics_callback_id_pfd2 == nullptr) {
        XPLMDebugString("PFD2: XPLMRegisterAvionicsCallbacksEx failed!\n");
    } else {
        XPLMDebugString("PFD2: XPLMRegisterAvionicsCallbacksEx success!\n");
    }

    params.deviceId = xplm_device_G1000_MFD;
    __avionics_callback_id_mfd = XPLMRegisterAvionicsCallbacksEx(&params);
    if (__avionics_callback_id_mfd == nullptr) {
        XPLMDebugString("MFD: XPLMRegisterAvionicsCallbacksEx failed!\n");
    } else {
        XPLMDebugString("MFD: XPLMRegisterAvionicsCallbacksEx success!\n");
    }

    return 1;
}

PLUGIN_API void XPluginDisable(void) {
    XPLMDebugString("XPluginDisable");

    if (__avionics_callback_id_pfd1) {
        XPLMUnregisterAvionicsCallbacks(__avionics_callback_id_pfd1);
    }
    if (__avionics_callback_id_pfd2) {
        XPLMUnregisterAvionicsCallbacks(__avionics_callback_id_pfd2);
    }
    if (__avionics_callback_id_mfd) {
        XPLMUnregisterAvionicsCallbacks(__avionics_callback_id_mfd);
    }
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID from, long msg, void *p) {
    XPLMDebugString("XPluginReceiveMessage\n");
}