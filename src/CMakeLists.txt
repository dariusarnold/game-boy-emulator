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
        )

target_include_directories(game_boy_emulator_library PUBLIC
        game-boy-emulator
        )
target_link_libraries(game_boy_emulator_library PUBLIC
        fmt::fmt-header-only
        imgui::imgui
        SDL2::SDL2
        OpenGL::GL
        magic_enum::magic_enum
        )

add_executable(game_boy_emulator
        main.cpp
        bindings/imgui_impl_opengl3.cpp
        bindings/imgui_impl_sdl.cpp
        )
target_link_libraries(game_boy_emulator PRIVATE
        game_boy_emulator_library
        )
target_include_directories(game_boy_emulator
        PRIVATE
        bindings
        )

option(WARNINGS_ENABLED "Enable compiler warnings for all targets. Works for GNU and Clang as well as MSVC." OFF)

if (WARNINGS_ENABLED)
    if (MSVC)
        target_compile_options(game_boy_emulatory_library PRIVATE /W4 /WX)
    else()
    target_compile_options(game_boy_emulator_library PRIVATE
            # Catchalls to avoid listing many single warnings
            -Wall
            -Wextra
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
            # Initializing a class member from itself is a nasty and hard to spot bug, so we turn it
            # into a warning. Clang has the flag but wont do anything, but gcc spots the error.
            -Werror=init-self
            -Wimplicit-fallthrough
            $<$<CXX_COMPILER_ID:Clang>:-Werror=delete-abstract-non-virtual-dtor>
            $<$<CXX_COMPILER_ID:GNU>:-Werror=delete-non-virtual-dtor>
            $<$<CXX_COMPILER_ID:Clang>:-Werror=constant-conversion>
            $<$<CXX_COMPILER_ID:GNU>:-Werror=overflow>
            -Werror
            $<$<CXX_COMPILER_ID:GNU>:-Wlogical-op>
            )
    endif()
endif()
