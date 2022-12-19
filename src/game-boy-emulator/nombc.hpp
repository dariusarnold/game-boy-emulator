#pragma once

#include "mbc.hpp"

class NoMbc : public Mbc {
public:
    using Mbc::Mbc;
    [[nodiscard]] uint8_t read_byte(uint16_t address) const override;
    void write_byte(uint16_t address, uint8_t value) override;
};
