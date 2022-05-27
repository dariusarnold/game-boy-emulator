#include "mmu.hpp"
#include "bitmanipulation.hpp"
#include "memory_range.hpp"
#include <ranges>
#include <utility>
#include <fmt/format.h>


uint8_t Mmu::read_byte(u_int16_t address) const {
    auto& p = get_memory_accessor(address);
    return p.read_byte(address);
}

void Mmu::write_byte(uint16_t address, uint8_t value) {
    auto& p = get_memory_accessor(address);
    p.write_byte(address, value);
}

void Mmu::map_boot_rom(const std::array<uint8_t, constants::BOOT_ROM_SIZE>& boot_rom) {
    auto*p = new MemoryRange(0x0, 0xFF, boot_rom.begin(), boot_rom.end());
    map_memory_range(p);

    // The logo should be stored on the cartridge, but for now just use this since it is required
    // for booting.
    const std::vector<uint8_t> cartridge_header{0x0, 0xC3, 0x1, 0x50 /* entry point */,
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
    auto* q = new MemoryRange(0x100, 0x100 + cartridge_header.size() - 1, cartridge_header.begin(),
                              cartridge_header.end());
    map_memory_range(q);
}

uint16_t Mmu::read_word(uint16_t address) const {
    auto& p = get_memory_accessor(address);
    return p.read_word(address);
}

void Mmu::write_word(uint16_t address, uint16_t value) {
    auto& p = get_memory_accessor(address);
    p.write_word(address, value);
}

IMemoryRange& Mmu::get_memory_accessor(uint16_t address) const {
    for (auto it = memory_accessors.rbegin(); it!= memory_accessors.rend(); it++) {
        if ((*it)->is_address_in_mapped_range(address)) {
            return **it;
        }
    }
    throw std::logic_error(fmt::format("No handler for address {:#02X} was found", address));
}

void Mmu::map_memory_range(IMemoryRange* memory_range) {
    fmt::print("Mapping memory range {:#02x}-{:#02x}\n", memory_range->get_begin_address(),
               memory_range->get_end_address());
    memory_accessors.push_back(memory_range);
}

class BankRegister: public MemoryRange {
    std::function<void(bool)> m_write_callback;
public:
    BankRegister(std::function<void(bool)> write_callback):
            MemoryRange(0xFF50, 0xFF50), m_write_callback(std::move(write_callback)) {}

    void write_byte(uint16_t address, uint8_t value) override {
        m_write_callback(value != 0);
    }
};

Mmu::Mmu() {
    // TODO Lifetime des Pointers managen
    auto* p_work_ram = new MemoryRange(0xC000, 0xCFFF);
    map_memory_range(p_work_ram);
    auto* p_high_ram = new MemoryRange(0xFF80, 0xFFFE);
    map_memory_range(p_high_ram);
    auto* p_bank_register = new BankRegister([this](bool was_non_zero){
        if (was_non_zero) {
            unmap_boot_rom();
        }
    });
    map_memory_range(p_bank_register);
}

void Mmu::map_memory_range(const std::vector<IMemoryRange*>& memory_ranges) {
    for (auto* p: memory_ranges) {
        map_memory_range(p);
    }
}

void Mmu::unmap_boot_rom() {
    fmt::print("Unmapping boot rom after successful bootup\n");
    unmap_memory_range(0x100);
}

void Mmu::unmap_memory_range(uint16_t address) {
    auto it = std::find_if(memory_accessors.begin(), memory_accessors.end(),
                           [address](const IMemoryRange* accessor) {
                               return accessor->is_address_in_mapped_range(address);
                           });
    if (it == memory_accessors.end()) {
        throw std::logic_error(fmt::format("Trying to unmapped non mapped address {}", address));
    }
    memory_accessors.erase(it);
}
