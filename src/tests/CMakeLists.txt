add_executable(game_boy_emulator_tests
        main_tests.cpp
        test_helpers.cpp
        test_registers.cpp
        test_bitmanipulation.cpp
        test_cpu.cpp
        test_boot_rom_comparison.cpp
        test_graphics.cpp
        test_blargg1_comparison.cpp
        test_blargg2_comparison.cpp
        test_blargg3_comparison.cpp
        )


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
        ${CMAKE_CURRENT_SOURCE_DIR}/blargg1.txt
        ${CMAKE_CURRENT_BINARY_DIR}/blargg1.txt
)
add_custom_command(
        TARGET game_boy_emulator_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/blargg2.txt
        ${CMAKE_CURRENT_BINARY_DIR}/blargg2.txt
)
add_custom_command(
        TARGET game_boy_emulator_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/blargg3.txt
        ${CMAKE_CURRENT_BINARY_DIR}/blargg3.txt
)
add_custom_command(
        TARGET game_boy_emulator_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${PROJECT_SOURCE_DIR}/dmg01-boot.bin
            ${CMAKE_CURRENT_BINARY_DIR}/dmg01-boot.bin
)
add_custom_command(
        TARGET game_boy_emulator_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${PROJECT_SOURCE_DIR}/01-special.gb
        ${CMAKE_CURRENT_BINARY_DIR}/blargg1.gb
)
add_custom_command(
        TARGET game_boy_emulator_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${PROJECT_SOURCE_DIR}/02-interrupts.gb
        ${CMAKE_CURRENT_BINARY_DIR}/blargg2.gb
)
add_custom_command(
        TARGET game_boy_emulator_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${PROJECT_SOURCE_DIR}/03-op sp,hl.gb"
        ${CMAKE_CURRENT_BINARY_DIR}/blargg3.gb
)