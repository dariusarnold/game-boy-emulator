#pragma once

#include "fmt/format.h"

#include <cstdint>

/**
 * Main data structure for register data.
 * Unions allow easy access to the lower and higher half of a two byte register.
 * This works on little endian only, for big endian the order of bytes has to be
 * swapped.
 */
struct Registers {
    uint16_t sp = 0, pc = 0;
    union {
        uint16_t af = 0;
        struct {
            uint8_t f, a;
        };
    };
    union {
        uint16_t bc = 0;
        struct {
            uint8_t c, b;
        };
    };
    union {
        uint16_t de = 0;
        struct {
            uint8_t e, d;
        };
    };
    union {
        uint16_t hl = 0;
        struct {
            uint8_t l, h;
        };
    };
};


/**
 * Formatted output for Registers.
 * Allows to print single registers by specifying the name in lower case.
 * Example: {:sp} would print the stack pointer
 */
template <>
struct fmt::formatter<Registers> {

    std::string format_string;

    /**
     * Parse until closing curly brace '}' is hit.
     * @param context Contains format string after :, meaning '{:f}' becomes 'f}'.
     * @return iterator past the end of the parsed range
     */
    auto parse(fmt::format_parse_context& context) {
        if (context.begin() == context.end()) {
            // No format string was specified
            return context.begin();
        }
        auto it = std::find(context.begin(), context.end(), '}');
        if (it == context.end()) {
            throw fmt::format_error("Invalid format");
        } else {
            format_string.assign(context.begin(), it);
        }
        return it;
    }

    template <typename FormatContext>
    auto format(const Registers& registers, FormatContext& context) {
        if (format_string == "sp") {
            return format_to(context.out(), "sp: {:02X}", registers.sp);
        } else if (format_string == "pc") {
            return format_to(context.out(), "pc: {:02X}", registers.pc);
        } else {
            throw fmt::format_error(fmt::format("Format string {} not supported", format_string));
        }
    }
};