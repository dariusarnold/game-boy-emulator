#include "dmatransfer.hpp"
#include "bitmanipulation.hpp"

OamDmaTransfer::OamDmaTransfer(std::shared_ptr<AddressBus> address_bus,
                               std::span<uint8_t, constants::OAM_DMA_NUM_BYTES> target) :
        m_address_bus(std::move(address_bus)), m_target(target) {}

void OamDmaTransfer::start_transfer(uint16_t start_address) {
    m_start_address = start_address;
    m_counter = 0;
}

void OamDmaTransfer::callback_cycle() {
    if (m_counter >= 160) {
        return;
    }

    auto x = m_address_bus->read_byte(m_start_address + m_counter);
    m_target[m_counter] = x;
    m_counter++;
}

uint16_t OamDmaTransfer::get_dma_start_address(uint8_t high_byte_address) const {
    return bitmanip::word_from_bytes(high_byte_address, 0);
}
