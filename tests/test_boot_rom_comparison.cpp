#include "catch2/catch.hpp"

#include "cpu.hpp"
#include "io.hpp"

#include "apu.hpp"
#include "emulator.hpp"
#include "fmt/format.h"
#include "gpu.hpp"
#include "mmu.hpp"

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>


std::vector<std::string> read_bootrom_log_file() {
    std::ifstream file{"BootromLog.txt"};
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

double calculate_percentage(double part, double total) {
    return part / total * 100.0;
}

// This matcher prints both the actual and the expected results directly below each other so that
// each actual value is aligned with the expected value. This makes it easier to spot differences.
class DebugStringEqual : public Catch::MatcherBase<std::string> {
    std::string m_expected_result;

public:
    explicit DebugStringEqual(std::string expected_result) :
            m_expected_result(std::move(expected_result)) {}

    bool match(const std::string& actual_result) const override {
        return actual_result == m_expected_result;
    }

    std::string describe() const override {
        return fmt::format("\n  \"{}\" (expected)", m_expected_result);
    }
};

inline DebugStringEqual StringEqualAlignedOutput(std::string expected_result) {
    return DebugStringEqual(std::move(expected_result));
}

TEST_CASE("Compare boot sequence") {
    auto expected_output = read_bootrom_log_file();
    REQUIRE_FALSE(expected_output.empty());
    auto boot_rom_path = std::filesystem::absolute(("dmg01-boot.bin"));
    auto boot_rom = load_boot_rom_file(boot_rom_path);
    std::vector<uint8_t> cartridge(1024, 0);
    // clang-format off
    const std::vector<uint8_t> cartridge_header{0x00, 0xC3, 0x1, 0x50 /* entry point */,
                                                0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00,
                                                0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD,
                                                0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB,
                                                0xB9, 0x33, 0x3E /* logo */,
                                                0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 /* game title (should be 16 bytes, but the boot rom log only uses 15 */,
                                                0x80 /* cgb flag*/,
                                                0x0,
                                                0x0 /* licensee code */,
                                                0x0 /* sgb flag*/,
                                                0x1 /* cartridge type */,
                                                0x0 /* rom size*/,
                                                0x0 /* ram size */,
                                                0x0 /* destination code */,
                                                0x0 /* old licensee code */,
                                                0x0 /* version number */,
                                                0x66 /* header checksum */,
                                                0x0, 0x0 /* global checksum */};
    // clang-format on
    for (auto i = 0; i < cartridge_header.size(); ++i) {
        cartridge[i + 0x100] = cartridge_header[i];
    }
    REQUIRE(boot_rom);
    Emulator emulator{boot_rom.value(), cartridge};
    for (auto i = 0; const auto& expected_line : expected_output) {
        auto actual_output = emulator.get_cpu_debug_state();
        INFO(fmt::format("Executing instruction number {}/{} ({:.2f}% done. Last instructions: {} "
                         "{}.\n CPU state: {})",
                         i, expected_output.size(), calculate_percentage(i, expected_output.size()),
                         emulator.get_current_instruction(), emulator.get_previous_instruction(),
                         emulator.get_cpu_debug_state()));
        ++i;
        REQUIRE_THAT(actual_output, StringEqualAlignedOutput(expected_line));
        REQUIRE(emulator.step());
    }
}