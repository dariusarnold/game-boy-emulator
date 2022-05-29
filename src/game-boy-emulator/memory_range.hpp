#pragma once

#include "bitmanipulation.hpp"
#include "i_mmu.hpp"
#include <algorithm>
#include <cstdint>
#include <vector>
#include <cassert>


/**
 * Class wrapping a part of the address space and treating it as memory.
 * Can be used by subsystems such as GPU, Audio, Ram to register itself at the Mmu and handle
 * memory access for an address range.
 */
class MemoryRange : public IMemoryRange {
    uint16_t begin_address;
    uint16_t end_address;
public:
    std::vector<uint8_t> memory;
    // Construct with empty storage
    MemoryRange(uint16_t begin, uint16_t end);
    // Construct by filling with existing data
    template <typename It>
    MemoryRange(uint16_t begin_addr, uint16_t end_addr, It begin_data, It end_data) :
            memory(begin_data, end_data), begin_address(begin_addr), end_address(end_addr) {
        assert(std::distance(begin_data, end_data) == (end_addr - begin_addr + 1)
               && "MemoryRange data and address range differ");
    }

    [[nodiscard]] uint16_t get_begin_address() const override;

    [[nodiscard]] uint16_t get_end_address() const override;

    [[nodiscard]] uint8_t read_byte(u_int16_t address) const override;

    [[nodiscard]] uint16_t read_word(uint16_t address) const override;

    void write_byte(uint16_t address, uint8_t value) override;

    void write_word(uint16_t address, uint16_t value) override;
};
