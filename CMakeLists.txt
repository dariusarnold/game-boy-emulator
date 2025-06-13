cmake_minimum_required(VERSION 3.25)
project(game_boy_emulator)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(SANITIZE "Enable address and undefined behavior sanitizers" OFF)
option(CLANG_TIDY "Enable clang-tidy checks" OFF)
option(WARNINGS_ENABLED "Enable compiler warnings for all targets. Works for GNU and Clang." OFF)
option(WARNINGS_AS_ERRORS "Treat any compiler warning as an error." OFF)
option(USE_CCACHE "Use the ccache compiler cache to speed up compilation" OFF)

find_package(fmt REQUIRED)
find_package(Catch2 REQUIRED)
find_package(SDL2 REQUIRED)
find_package(imgui REQUIRED)
find_package(OpenGL REQUIRED)
find_package(magic_enum REQUIRED)
find_package(spdlog REQUIRED)
find_package(argparse REQUIRED)
find_package(Boost REQUIRED)
# Nativefiledialog-extend is not yet availabe in Conan Center
include(FetchContent)
FetchContent_Declare(nativefiledialog-extended
        # Without this clang-tidy would warn about things in the libraries header, but this requires cmake 3.25
        SYSTEM
        GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended.git
        GIT_TAG v1.2.1
        )
FetchContent_MakeAvailable(nativefiledialog-extended)


# Add a custom CMake Modules directory to find the modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
# Exclude automatically copied imgui bindings directory and build directory.
set(CLANG_FORMAT_EXCLUDE_PATTERNS "bindings" ${CMAKE_BINARY_DIR})
# Enable clang-format integration by creating clang-format CMake targets
find_package(ClangFormat)

if (CMAKE_CXX_COMPILER_ID STREQUAL Clang AND CLANG_TIDY)
    # The CMAKE_CXX_CLANG_TIDY integration passes all compiler arguments to clang-tidy. For gcc we have a required
    # compiler option to increase constexpr evaluation limit (fconstexpr-ops-limit), which clang does not know (it has
    # a different option). But this flag leads to cland-diagnostics-error and the workaround would be manually removing
    # it from the compilation database. Instead activate clang-tidy only for clang.
    include(clang-tidy)
else()
    message(STATUS "clang-tidy is only active for clang, disabling checks")
endif ()

if (USE_CCACHE)
    include(ccache)
endif ()

if (SANITIZE)
    message(STATUS "Sanitizers were enabled")
    add_compile_options(-fsanitize=address,undefined)
    add_link_options(-fsanitize=address,undefined)
endif ()

add_subdirectory(src)
add_subdirectory(src/tests)