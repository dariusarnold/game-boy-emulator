#include <cassert>
#include "memory_range.hpp"


MemoryRange::MemoryRange(uint16_t begin, uint16_t end) : begin_address(begin), end_address(end) {
    assert(end_address >= begin_address && "Invalid address pair given to MemoryRange");
    memory.resize(end_address - begin_address + 1);
    std::fill(memory.begin(), memory.end(), 0x0);
}

uint16_t MemoryRange::get_begin_address() const {
    return begin_address;
}

uint16_t MemoryRange::get_end_address() const {
    return end_address;
}

uint8_t MemoryRange::read_byte(uint16_t address) const {
    assert(is_address_in_mapped_range(address) && "Reading from invalid address");
    return memory[address - get_begin_address()];
}
void MemoryRange::write_byte(uint16_t address, uint8_t value) {
    assert(is_address_in_mapped_range(address) && "Writing to invalid address");
    memory[address - get_begin_address()] = value;
}