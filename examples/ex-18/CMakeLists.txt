cmake_minimum_required(VERSION 3.20)

project(ex-18 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(main main.cpp)

if(EMSCRIPTEN)
  target_compile_definitions(main PRIVATE
    GLFW_INCLUDE_ES3
  )

  # Use the GLFW implementation included with Emscripten
  target_link_options(main PRIVATE
    "-sUSE_GLFW=3"
    "-sMIN_WEBGL_VERSION=2"  # Set the minimum WebGL version to 2
    "-sMAX_WEBGL_VERSION=2"  # Set the maximum WebGL version to 2
  )

  # Same as specifying "-o main.js"
  set_target_properties(main PROPERTIES SUFFIX ".js")
else()
  target_compile_definitions(main PRIVATE
    GLFW_INCLUDE_NONE
  )

  include(FetchContent)

  # Disable building GLFW documentation, tests, and examples
  set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

  FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.5.1
    GIT_SHALLOW    TRUE
  )
  FetchContent_MakeAvailable(glfw)

  # GLAD library for loading OpenGL functions
  add_library(glad STATIC
    third_party/glad/src/gl.c
  )

  target_include_directories(glad PUBLIC
    third_party/glad/include
  )

  target_link_libraries(main PRIVATE glad)
endif()

# IMGUI library for graphical user interfaces(GUI)
add_library(imgui STATIC
  third_party/imgui/imgui_demo.cpp
  third_party/imgui/imgui_draw.cpp
  third_party/imgui/imgui_impl_glfw.cpp
  third_party/imgui/imgui_impl_opengl3.cpp
  third_party/imgui/imgui_tables.cpp
  third_party/imgui/imgui_widgets.cpp
  third_party/imgui/imgui.cpp
)

target_include_directories(imgui PUBLIC
  third_party/imgui
)

if(EMSCRIPTEN)
  target_compile_definitions(imgui PUBLIC
    IMGUI_IMPL_OPENGL_ES3
  )
else()
  target_link_libraries(imgui PUBLIC glfw)
endif()

target_link_libraries(main PRIVATE imgui)
