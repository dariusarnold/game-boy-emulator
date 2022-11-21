#include "catch2/catch.hpp"

#include "io.hpp"
#include "cpu.hpp"

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
class DebugStringEqual: public Catch::MatcherBase<std::string> {
    std::string m_expected_result;
public:
    explicit DebugStringEqual(std::string expected_result): m_expected_result(std::move(expected_result)) {}

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
    REQUIRE(boot_rom);
    Emulator emulator{boot_rom.value(), {}};
    for (auto i = 0; const auto& expected_line: expected_output) {
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