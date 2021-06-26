add_executable(game_boy_emulator_tests
        main_tests.cpp
        test_registers.cpp
        test_bitmanipulation.cpp
        test_cpu.cpp
        test_mmu.cpp
        test_increment_instruction.cpp
        test_call_intermediary.cpp
        test_load_register_from_register.cpp
        test_push_register.cpp
        test_rotate_left.cpp)
target_link_libraries(game_boy_emulator_tests PRIVATE
        game_boy_emulator_library
        Catch2::Catch2
        )