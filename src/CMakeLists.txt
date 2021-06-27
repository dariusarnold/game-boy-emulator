add_library(game_boy_emulator_library
        game-boy-emulator/constants.h
        game-boy-emulator/cpu.cpp
        game-boy-emulator/cpu.hpp
        game-boy-emulator/opcodes.cpp
        game-boy-emulator/opcodes.hpp
        game-boy-emulator/bitmanipulation.cpp
        game-boy-emulator/bitmanipulation.hpp
        game-boy-emulator/mmu.cpp
        game-boy-emulator/mmu.hpp
        game-boy-emulator/instructions/increment.cpp
        game-boy-emulator/instructions/increment.hpp
        game-boy-emulator/facades/register.cpp
        game-boy-emulator/facades/register.hpp
        game-boy-emulator/facades/flag.cpp
        game-boy-emulator/facades/flag.hpp
        game-boy-emulator/facades/incrementpc.cpp
        game-boy-emulator/facades/incrementpc.hpp
        game-boy-emulator/facades/pushstack.cpp
        game-boy-emulator/facades/pushstack.hpp
        game-boy-emulator/instructions/callintermediary.cpp
        game-boy-emulator/instructions/callintermediary.hpp
        game-boy-emulator/facades/memory.cpp
        game-boy-emulator/facades/memory.hpp
        game-boy-emulator/instructions/copyregister.cpp
        game-boy-emulator/instructions/copyregister.hpp
        game-boy-emulator/instructions/pushregister.cpp
        game-boy-emulator/instructions/pushregister.hpp
        game-boy-emulator/instructions/rotateleft.cpp
        game-boy-emulator/instructions/rotateleft.hpp)
target_include_directories(game_boy_emulator_library PUBLIC
        game-boy-emulator
        )
target_link_libraries(game_boy_emulator_library PUBLIC
        fmt
        )

add_executable(game_boy_emulator
        main.cpp
        )
target_link_libraries(game_boy_emulator PRIVATE
        game_boy_emulator_library
        )

set(CMAKE_VERBOSE_MAKEFILE ON)

option(WARNINGS_ENABLED "Enable compiler warnings for all targets. Works for GNU and Clang as well as MSVC." OFF)

if (WARNINGS_ENABLED)
    if (MSVC)
        target_compile_options(game_boy_emulatory_library PRIVATE /W4 /WX)
    else()
    target_compile_options(game_boy_emulator_library PRIVATE
            # Catchalls to avoid listing many single warnings
            -Wall -Wextra
            # Warn on conversion where the sign could be lost
            -Wsign-conversion
            # Warn on shadowing
            -Wshadow
            # Warn when parentheses are omitted in contexts where this could lead to bugs
            -Wparentheses
            # Warn about suspicious uses of logical operators in expressions
            # Hide this for non-GNU compiler
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
            )
    endif()
endif()
