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
        )
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
