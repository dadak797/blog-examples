cmake_minimum_required(VERSION 3.20)

project(ex-18 LANGUAGES C CXX)

add_executable(main main.cpp)
target_compile_features(main PRIVATE cxx_std_17)

if(EMSCRIPTEN)
  # Use the GLFW implementation included with Emscripten
  target_link_options(main PRIVATE
    "-sUSE_GLFW=3"
    "-sMIN_WEBGL_VERSION=2"  # Set the minimum WebGL version to 2
    "-sMAX_WEBGL_VERSION=2"  # Set the maximum WebGL version to 2
  )

  # Same as specifying "-o main.js"
  set_target_properties(main PROPERTIES SUFFIX ".js")
else()
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

  add_library(glad STATIC
    third_party/glad/src/gl.c
  )

  target_include_directories(glad PUBLIC
    third_party/glad/include
  )

  target_link_libraries(main PRIVATE glfw glad)
endif()
