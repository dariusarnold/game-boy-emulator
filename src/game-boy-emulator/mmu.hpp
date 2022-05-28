#pragma once

#include "constants.h"
#include "i_mmu.hpp"
#include "memory_range.hpp"
#include <array>
#include <cstdint>
#include <span>
#include <memory>





class Mmu : public IMemoryAccess {
    std::vector<IMemoryRange*> memory_accessors;

    // The returned reference may be invalidated by mapping further memory accessors.
    [[nodiscard]] IMemoryRange& get_memory_accessor(uint16_t address) const;

public:
    Mmu();
    /**
     * Read memory value from address.
     */
    [[nodiscard]] uint8_t read_byte(u_int16_t address) const override;

    /**
     * Read word from address.
     * Assumes that words are stored least significant byte first
     */
    [[nodiscard]] uint16_t read_word(uint16_t address) const override;

    /**
     * Write value to ram at address.
     */
    void write_byte(uint16_t address, uint8_t value) override;

    /**
     * Write 2 byte wird to ram at address.
     */
    void write_word(uint16_t address, uint16_t value) override;

    /**
     * Map boot rom at memory address 0x00
     */
    void map_boot_rom(const std::array<uint8_t, constants::BOOT_ROM_SIZE>& boot_rom);
    void map_cartridge(const std::vector<uint8_t>& game_rom);
    void unmap_boot_rom();

    void map_memory_range(IMemoryRange* memory_range);
    void map_memory_range(const std::vector<IMemoryRange*>& memory_ranges);

    void unmap_memory_range(uint16_t begin_address, uint16_t end_address);
};