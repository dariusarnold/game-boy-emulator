cmake_minimum_required(VERSION 3.17)
project(game_boy_emulator)

set(CMAKE_CXX_STANDARD 20)

# This is done because after executing "conan install .." in the binary dir, conan_paths.cmake
# will set up CMAKE_MODULE_PATH so find_package will find the packages install by conan.
include(${CMAKE_BINARY_DIR}/conan_paths.cmake OPTIONAL RESULT_VARIABLE CONAN_PATHS_FOUND)
if (${CONAN_PATHS_FOUND} STREQUAL "NOTFOUND")
    message(STATUS "File conan_paths.cmake not found. Maybe conan install was not executed?")
endif ()
find_package(fmt REQUIRED)
find_package(Catch2 REQUIRED)
find_package(SDL2 REQUIRED)
find_package(imgui REQUIRED)
find_package(OpenGL REQUIRED)
find_package(magic_enum REQUIRED)

# Enable clang-format integration by creating clang-format CMake targets
# Add a custom CMake Modules directory
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules
        ${CMAKE_MODULE_PATH})
set(CLANG_FORMAT_EXCLUDE_PATTERNS "src/bindings" ${CMAKE_BINARY_DIR})
find_package(ClangFormat)

add_subdirectory(src)
add_subdirectory(tests)