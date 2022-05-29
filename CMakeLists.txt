cmake_minimum_required(VERSION 3.17)
project(game_boy_emulator)

set(CMAKE_CXX_STANDARD 20)

# This is done because after executing "conan install .." in the binary dir, conan_paths.cmake
# will set up CMAKE_MODULE_PATH so find_package will find the packages install by conan.
include(${CMAKE_BINARY_DIR}/conan_paths.cmake OPTIONAL RESULT_VARIABLE CONAN_PATHS_FOUND)
if (${CONAN_PATHS_FOUND} STREQUAL "NOTFOUND")
    message(STATUS "File conan_paths.cmake not found. Maybe conan install was not executed?")
endif()
find_package(fmt REQUIRED)
find_package(Catch2 REQUIRED)
find_package(SFML COMPONENTS graphics window network audio system REQUIRED)
find_package(SDL2 REQUIRED)
find_package(stb REQUIRED)

add_subdirectory(src)
add_subdirectory(tests)

add_library(imgui)
target_sources(imgui PRIVATE
        imgui/imgui.cpp
        imgui/imgui.h
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_demo.cpp
        imgui/backends/imgui_impl_sdl.cpp
        imgui/backends/imgui_impl_sdlrenderer.cpp
        imgui/backends/imgui_impl_opengl3.cpp
        imgui/backends/imgui_impl_opengl3.h
        imgui/backends/imgui_impl_sdl.h
        )
target_include_directories(imgui SYSTEM PUBLIC imgui imgui/backends)
target_link_libraries(imgui PUBLIC SDL2::SDL2)