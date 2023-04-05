cmake_minimum_required(VERSION 3.25)
project(game_boy_emulator)

set(CMAKE_CXX_STANDARD 20)

find_package(fmt REQUIRED)
find_package(Catch2 REQUIRED)
find_package(SDL2 REQUIRED)
find_package(imgui REQUIRED)
find_package(OpenGL REQUIRED)
find_package(magic_enum REQUIRED)
find_package(spdlog REQUIRED)
find_package(argparse REQUIRED)
find_package(Boost REQUIRED)
find_package(nativefiledialog REQUIRED)

# Enable clang-format integration by creating clang-format CMake targets
# Add a custom CMake Modules directory
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
set(CLANG_FORMAT_EXCLUDE_PATTERNS "src/bindings" ${CMAKE_BINARY_DIR})
find_package(ClangFormat)

if (CMAKE_CXX_COMPILER_ID STREQUAL Clang)
    # The CMAKE_CXX_CLANG_TIDY integration passes all compiler arguments to clang-tidy. For gcc we have a required
    # compiler option to increase constexpr evaluation limit (fconstexpr-ops-limit), which clang does not know (it has
    # a different option). But this flag leads to cland-diagnostics-error and the workaround would be manually removing
    # it from the compilation database. Instead activate clang-tidy only for clang.
    message(STATUS "Checking with clang-tidy was enabled")
    include(clang-tidy)
else()
    message(STATUS "clang-tidy is only active for clang, disabling checks")
endif ()

add_compile_options(-fsanitize=address,undefined)
add_link_options(-fsanitize=address,undefined)

add_subdirectory(src)
add_subdirectory(src/tests)