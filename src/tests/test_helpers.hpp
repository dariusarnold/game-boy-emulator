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