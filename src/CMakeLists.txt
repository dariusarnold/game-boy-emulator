add_library(game_boy_emulator_library
        game-boy-emulator/constants.h
        game-boy-emulator/cpu.cpp
        game-boy-emulator/cpu.hpp
        game-boy-emulator/opcodes.cpp
        game-boy-emulator/opcodes.hpp
        game-boy-emulator/bitmanipulation.cpp
        game-boy-emulator/bitmanipulation.hpp
        game-boy-emulator/io.cpp
        game-boy-emulator/io.hpp
        game-boy-emulator/emulator.cpp
        game-boy-emulator/emulator.hpp
        game-boy-emulator/gpu.cpp
        game-boy-emulator/gpu.hpp
        game-boy-emulator/graphics.cpp
        game-boy-emulator/graphics.hpp
        game-boy-emulator/interrupthandler.cpp
        game-boy-emulator/interrupthandler.hpp
        game-boy-emulator/registers.hpp
        game-boy-emulator/addressbus.cpp
        game-boy-emulator/addressbus.hpp
        game-boy-emulator/ram.cpp
        game-boy-emulator/ram.hpp
        game-boy-emulator/memorymap.hpp
        game-boy-emulator/exceptions.cpp
        game-boy-emulator/exceptions.hpp
        game-boy-emulator/bootrom.cpp
        game-boy-emulator/bootrom.hpp
        game-boy-emulator/cartridge.cpp
        game-boy-emulator/cartridge.hpp
        game-boy-emulator/timer.cpp
        game-boy-emulator/timer.hpp
        )

target_include_directories(game_boy_emulator_library PUBLIC
        game-boy-emulator
        )
target_link_libraries(game_boy_emulator_library PUBLIC
        imgui::imgui
        SDL2::SDL2
        OpenGL::GL
        magic_enum::magic_enum
        spdlog::spdlog
        fmt::fmt
        )

add_executable(game_boy_emulator
        main.cpp
        )
# Add external vendor code as an object library so it can be excluded from clang-tidy
add_library(imgui_bindings OBJECT
        ../bindings/imgui_impl_opengl3.cpp
        ../bindings/imgui_impl_sdl.cpp
        )
set_target_properties(
        imgui_bindings
        PROPERTIES CXX_CLANG_TIDY ""
)
target_include_directories(imgui_bindings
        SYSTEM
        PUBLIC
        ../bindings)
target_link_libraries(imgui_bindings
        PUBLIC
        imgui::imgui
        SDL2::SDL2
        OpenGL::GL
        )
target_link_libraries(game_boy_emulator PRIVATE
        game_boy_emulator_library
        imgui_bindings
        argparse::argparse
        )
target_include_directories(game_boy_emulator
        PRIVATE
        bindings
        )

option(WARNINGS_ENABLED "Enable compiler warnings for all targets. Works for GNU and Clang as well as MSVC." ON)

if (WARNINGS_ENABLED)
    target_compile_options(game_boy_emulator_library PRIVATE
            $<$<CXX_COMPILER_ID:GNU>:-fstrict-enums>
            -Werror
            # Catchalls to avoid listing many single warnings
            -Wall
            -Wextra
            $<$<CXX_COMPILER_ID:Clang>:-Weverything>
            # Warn on conversion where the sign could be lost
            -Wsign-conversion
            # Warn on shadowing
            -Wshadow
            # Warn when parentheses are omitted in contexts where this could lead to bugs
            -Wparentheses
            # Warn about suspicious uses of logical operators in expressions
            # Hide this for non-GNU compiler since Clang doesnt support it
            $<$<CXX_COMPILER_ID:GNU>:-Wlogical-op>
            # Better to use modern style casts
            -Wold-style-cast
            # Warn when function declaration hides virtual functions from a base class
            -Woverloaded-virtual
            # Overriding virtual functions should be marked override
            -Wsuggest-override
            # Dont allow zero as a nullptr constant
            -Wzero-as-null-pointer-constant
            # Warn if undefined identifer is used in an if directive
            -Wundef
            # Warn if object is used without initialization
            -Wuninitialized
            -Wimplicit-fallthrough
            -Wdelete-non-virtual-dtor
            -Woverflow
            # Enabling GCC specific warning flags
            $<$<CXX_COMPILER_ID:GNU>:-Wlogical-op>
            $<$<CXX_COMPILER_ID:GNU>:-Wcast-qual>
            $<$<CXX_COMPILER_ID:GNU>:-Wcast-align=strict>
            $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-branches>
            $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-cond>
            $<$<CXX_COMPILER_ID:GNU>:-Wfloat-equal>
            # Initializing a class member from itself is a nasty and hard to spot bug, so we turn it
            # into a warning. Clang has the flag but wont do anything, but gcc spots the error.
            $<$<CXX_COMPILER_ID:GNU>:-Winit-self>
            $<$<CXX_COMPILER_ID:GNU>:-Wlogical-op>
            $<$<CXX_COMPILER_ID:GNU>:-Wnull-dereference>
            $<$<CXX_COMPILER_ID:GNU>:-Wundef>
            # Disabling the errors enabled by Weverything for clang which are not useful
            $<$<CXX_COMPILER_ID:Clang>:-Wno-c++98-compat>
            $<$<CXX_COMPILER_ID:Clang>:-Wno-c++98-compat-pedantic>
            $<$<CXX_COMPILER_ID:Clang>:-Wno-documentation>
            $<$<CXX_COMPILER_ID:Clang>:-Wno-padded>
            $<$<CXX_COMPILER_ID:Clang>:-Wno-nested-anon-types>
            $<$<CXX_COMPILER_ID:Clang>:-Wno-gnu-anonymous-struct>
            $<$<CXX_COMPILER_ID:Clang>:-Wno-missing-prototypes>
            $<$<CXX_COMPILER_ID:Clang>:-Wno-implicit-int-conversion>
            # This warns when an enum value is not explicitly handled in a switch, even when a default statement
            # handles the enum.
            $<$<CXX_COMPILER_ID:Clang>:-Wno-switch-enum>
            $<$<CXX_COMPILER_ID:Clang>:-Wno-ctad-maybe-unsupported>
            )
endif()
