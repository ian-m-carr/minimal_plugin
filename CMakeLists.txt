cmake_minimum_required(VERSION 3.24)
project(minimal_plugin)

set(CMAKE_CXX_STANDARD 20)

# modify these to suit
set(LOCAL_XP_SDK_DIR D:/projects/X-Plane/XPSDK400b1/SDK/)

if (WIN32)
    set(CMAKE_DEBUG_POSTFIX -d)

    if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX "D:/projects/X-Plane/installs" CACHE PATH "..." FORCE)
    endif ()

    #message("install: ${CMAKE_INSTALL_PREFIX}")
    if (NOT DEFINED CMAKE_PREFIX_PATH)
        # list of install folders for dependant packages
        set(CMAKE_PREFIX_PATH ${LOCAL_XP_SDK_DIR}/Libraries/Win;${LOCAL_XPLANE_INSTALLS})
    endif ()

    add_compile_definitions(_WIN32_WINNT=0x0601)
endif ()

add_compile_definitions(IBM=1 XPLM200=1 XPLM210=1 XPLM300=1 XPLM301=1 XPLM302=1 XPLM303=1 XPLM400=1 NOMINMAX _CRTDBG_MAP_ALLOC)

add_compile_definitions(CCW_WINDING)

# locate the X-plane libraries
find_library(XPLM_LIB XPLM_64)
find_library(XPLWIDGETS_LIB XPWidgets_64)

# x-plane include folders
include_directories(SYSTEM "${LOCAL_XP_SDK_DIR}/CHeaders/XPLM" "${LOCAL_XP_SDK_DIR}/CHeaders/Widgets" "${LOCAL_XP_SDK_DIR}/CHeaders/Wrappers")

add_library(minimal_plugin SHARED
        plugin.cpp
        glmath/vectors.h glmath/matrices.h glmath/projections.h
        gldraw/VertexManager.h
        gldraw/geom.h
        gldraw/shaders/coloured_vertex.h gldraw/shaders/coloured_vertex.cpp
        gldraw/colour.h
        gldraw/textures.h
        stb/stb_image.h stb/stb_image.cpp
        glad/gl.h)

target_link_libraries(minimal_plugin PRIVATE "${XPLM_LIB}" "${XPLWIDGETS_LIB}" "${OPENGL_LIB}")

set_target_properties(minimal_plugin PROPERTIES OUTPUT_NAME "minimal_plugin")
set_target_properties(minimal_plugin PROPERTIES SUFFIX ".xpl")
target_include_directories(minimal_plugin PUBLIC SYSTEM ${CMAKE_CURRENT_SOURCE_DIR})
