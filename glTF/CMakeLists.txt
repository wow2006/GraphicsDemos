# ${CMAKE_SOURCE_DIR}/glTF/CMakeLists.txt
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
find_package(gl3w   REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(fmt REQUIRED)

add_executable(
    SimpleBox
    SimpleBox.cpp
)

target_link_libraries(
    SimpleBox
    PUBLIC
    gl3w::gl3w
    OpenGL::GL
    glfw
    fmt::fmt
)

target_include_directories(
    SimpleBox
    SYSTEM PUBLIC
    ${RAPIDJSON_INCLUDE_DIRS}
)

set_target_properties(
    SimpleBox
    PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
)

