#include "dmatransfer.hpp"
#include "bitmanipulation.hpp"
#include "memorymap.hpp"

OamDmaTransfer::OamDmaTransfer(std::shared_ptr<AddressBus> address_bus,
                               std::span<std::byte, constants::OAM_DMA_NUM_BYTES> target) :
        m_address_bus(std::move(address_bus)), m_target(target) {}

void OamDmaTransfer::start_transfer(uint16_t start_address) {
    m_start_address = start_address;
    // In most cases echo ram is only E000-FDFF (which actually accesses RAM from C000-DDFF). OAM
    // DMA ist an exception, here the entire E000-FFF region is echo ram and accesses C000-DFFF.
    if (m_start_address > memmap::EchoRamEnd) {
        m_start_address -= memmap::InternalRamSize;
    }
    m_counter = 0;
}

void OamDmaTransfer::callback_cycle() {
    if (m_counter >= 160) {
        return;
    }

    auto x = std::byte{m_address_bus->read_byte(m_start_address + m_counter)};
    m_target[m_counter] = x;
    m_counter++;
}

uint16_t OamDmaTransfer::get_dma_start_address(uint8_t high_byte_address) const {
    return bitmanip::word_from_bytes(high_byte_address, 0);
}
