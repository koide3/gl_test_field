cmake_minimum_required(VERSION 2.8.3)
project(gl_test_field)

# add_compile_options(-std=c++11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(OpenGL REQUIRED)
# find_package(Boost REQUIRED filesystem program_options)

###########
## Build ##
###########

include_directories(
  include
  thirdparty/imgui
  thirdparty/imgui/examples
  thirdparty/portable-file-dialogs
  thirdparty/Eigen
  thirdparty/gl3w
  thirdparty/glm
  thirdparty/glfw/include
)

link_directories(
  thirdparty/glfw/lib-vc2019
)

# imgui
add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GL3W)

# Win32
add_definitions(
  -DNOMINMAX
  -D_USE_MATH_DEFINES
  -D_ENABLE_EXTENDED_ALIGNED_STORAGE
  -D_CRT_SECURE_NO_WARNINGS
)

###########
## Build ##
###########

add_library(imgui
  thirdparty/gl3w/gl3w.c
  thirdparty/imgui/imgui.cpp
  thirdparty/imgui/imgui_demo.cpp
  thirdparty/imgui/imgui_draw.cpp
  thirdparty/imgui/imgui_widgets.cpp
  thirdparty/imgui/examples/imgui_impl_glfw.cpp
  thirdparty/imgui/examples/imgui_impl_opengl3.cpp
)
target_link_libraries(imgui
  ${OPENGL_LIBRARIES}
  glfw3
)

add_library(guik
  src/glk/mesh.cpp
  src/glk/lines.cpp
  src/glk/thin_lines.cpp
# src/glk/pointcloud_buffer.cpp
  src/glk/glsl_shader.cpp
  src/glk/frame_buffer.cpp
  src/glk/primitives/primitives.cpp
  src/glk/loaders/ply_loader.cpp
  src/guik/gl_canvas.cpp
  src/guik/model_control.cpp
  src/guik/camera_control.cpp
  src/guik/imgui_application.cpp
  src/guik/viewer/light_viewer.cpp
)
target_link_libraries(guik
  ${OPENGL_LIBRARIES}
  glfw3
  imgui
)
add_dependencies(guik
  imgui
)


add_executable(gl_test_field
  src/gl_test_field.cpp
)
target_link_libraries(gl_test_field
  imgui
  guik
)
add_dependencies(gl_test_field
  imgui
  guik
)


file(COPY data DESTINATION .)