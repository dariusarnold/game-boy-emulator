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
        test_blargg4_comparison.cpp
        test_blargg5_comparison.cpp
        test_blargg6_comparison.cpp
        test_blargg7_comparison.cpp
        test_blargg8_comparison.cpp
        test_blargg9_comparison.cpp
        test_blargg10_comparison.cpp
        test_blargg11_comparison.cpp
        test_framebuffer.cpp
        test_instruction_timing.cpp
        test_joypad.cpp
        test_mem_timing.cpp
        test_mooneye_mbc1.cpp
        test_mooneye_mbc5.cpp
        test_mooneye_oam_dma.cpp
        test_dmg_acid2.cpp
        )

target_link_libraries(game_boy_emulator_tests PRIVATE
        game_boy_emulator_library
        Catch2::Catch2
        )

add_custom_target(copy_test_dependencies ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/recorded-logs
        ${CMAKE_CURRENT_BINARY_DIR}/recorded-logs
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/roms
        ${CMAKE_CURRENT_BINARY_DIR}/roms
)
add_dependencies(game_boy_emulator_tests copy_test_dependencies)