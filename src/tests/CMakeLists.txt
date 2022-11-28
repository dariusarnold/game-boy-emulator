add_executable(game_boy_emulator_tests
        main_tests.cpp
        test_registers.cpp
        test_bitmanipulation.cpp
        test_cpu.cpp
        test_boot_rom_comparison.cpp
        test_graphics.cpp)


target_link_libraries(game_boy_emulator_tests PRIVATE
        game_boy_emulator_library
        Catch2::Catch2
        )

add_custom_command(
        TARGET game_boy_emulator_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/BootromLog.txt
            ${CMAKE_CURRENT_BINARY_DIR}/BootromLog.txt
)
add_custom_command(
        TARGET game_boy_emulator_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${PROJECT_SOURCE_DIR}/dmg01-boot.bin
            ${CMAKE_CURRENT_BINARY_DIR}/dmg01-boot.bin
)