#pragma once

#include "addressbus.hpp"
#include "constants.h"
#include <memory>
#include <span>
#include <utility>


/*
 * Class doing the OAM DMA transfer for the PPU.
 */
class OamDmaTransfer {
    std::shared_ptr<AddressBus> m_address_bus;
    uint16_t m_start_address = 0;
    std::span<uint8_t, constants::OAM_DMA_NUM_BYTES> m_target;
    // Count the number of bytes transferred (Total 160 bytes will be transferred)
    size_t m_counter = 0;

public:
    explicit OamDmaTransfer(std::shared_ptr<AddressBus> address_bus,
                            std::span<uint8_t, constants::OAM_DMA_NUM_BYTES> target);

    // Will set the transfer state to active
    void start_transfer(uint16_t start_address);

    // Call every cycle, will transfer one byte if a DMA transfer is active and do nothing
    // otherwise.
    void callback_cycle();

    [[nodiscard]] uint16_t get_dma_start_address(uint8_t high_byte_address) const;
};