#pragma once

#include "fmt/format.h"
#include "catch2/catch.hpp"
#include <string>
#include <vector>
#include <fstream>

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

inline double calculate_percentage(double part, double total) {
    return part / total * 100.0;
}

inline std::vector<std::string> read_rom_log_file(const std::string& filename) {
    std::ifstream file{filename};
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

#include "cpu.hpp"

inline std::vector<CpuDebugState> read_log_file(const std::string& filename) {
    // Example: A: C2 F: 10 B: 01 C: 10 D: C0 E: 03 H: 40 L: 03 SP: FFFE PC: 0206 (2A 12 1C 20)
    std::ifstream file{filename};
    std::string line;
    std::vector<CpuDebugState> lines;
    while (std::getline(file, line)) {
        CpuDebugState cds;
        cds.a = std::strtol(line.c_str() + 3, nullptr, 16);
        cds.f = std::strtol(line.c_str() + 9, nullptr, 16);
        cds.b = std::strtol(line.c_str() + 15, nullptr, 16);
        cds.c = std::strtol(line.c_str() + 21, nullptr, 16);
        cds.d = std::strtol(line.c_str() + 27, nullptr, 16);
        cds.e = std::strtol(line.c_str() + 33, nullptr, 16);
        cds.h = std::strtol(line.c_str() + 39, nullptr, 16);
        cds.l = std::strtol(line.c_str() + 45, nullptr, 16);
        cds.sp = std::strtol(line.c_str() + 52, nullptr, 16);
        cds.pc = std::strtol(line.c_str() + 61, nullptr, 16);
        cds.mem_pc[0] = std::strtol(line.c_str() + 67, nullptr, 16);
        cds.mem_pc[1] = std::strtol(line.c_str() + 70, nullptr, 16);
        cds.mem_pc[2] = std::strtol(line.c_str() + 73, nullptr, 16);
        cds.mem_pc[3] = std::strtol(line.c_str() + 76, nullptr, 16);
        lines.push_back(cds);
    }
    return lines;
}