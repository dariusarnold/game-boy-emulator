#include "ppu_registers.hpp"
#include "bitmanipulation.hpp"

uint8_t PpuRegisters::get_register_value(PpuRegisters::Register r) const {
    if (r == Register::LyRegister && m_fix_ly_register_value) {
        return 0x90;
    }
    return get(r);
}

void PpuRegisters::set_register_value(PpuRegisters::Register r, uint8_t value) {
    if (r == Register::DmaTransfer) {
        m_oam_transfer_requested = true;
    }
    get(r) = value;
}

uint8_t PpuRegisters::get_register_value(uint16_t address) const {
    return get_register_value(static_cast<Register>(address));
}

void PpuRegisters::set_register_value(uint16_t address, uint8_t value) {
    set_register_value(static_cast<Register>(address), value);
}

PpuRegisters::PpuRegisters(bool fix_ly) : m_fix_ly_register_value(fix_ly) {
    set_mode(PpuMode::OamScan_2);
}

void PpuRegisters::set_mode(PpuMode mode) {
    auto bit0 = bitmanip::is_bit_set(static_cast<int>(mode), 0);
    auto bit1 = bitmanip::is_bit_set(static_cast<int>(mode), 1);
    bitmanip::set(get(Register::StatRegister), 0, bit0);
    bitmanip::set(get(Register::StatRegister), 1, bit1);
}

PpuMode PpuRegisters::get_mode() const {
    auto bit0 = bitmanip::bit_value(get(Register::StatRegister), 0);
    auto bit1 = bitmanip::bit_value(get(Register::StatRegister), 1);
    return static_cast<PpuMode>(bit0 | (bit1 << 1));
}

uint8_t& PpuRegisters::get(PpuRegisters::Register r) {
    return m_registers[static_cast<uint16_t>(r) - static_cast<uint16_t>(Register::LcdcRegister)];
}

const uint8_t& PpuRegisters::get(PpuRegisters::Register r) const {
    return m_registers[static_cast<uint16_t>(r) - static_cast<uint16_t>(Register::LcdcRegister)];
}

bool PpuRegisters::is_stat_interrupt_enabled(
    PpuRegisters::StatInterruptSource stat_interrupt) const {
    return bitmanip::is_bit_set(get(Register::StatRegister), static_cast<int>(stat_interrupt));
}

PpuRegisters::BgWinAddressMode PpuRegisters::get_bg_win_address_mode() const {
    auto bit4 = get_register_bit(Register::LcdcRegister,
                                 static_cast<uint8_t>(LcdcBits::BgWindowTileDataArea));
    if (bit4 == 1) {
        return BgWinAddressMode::Unsigned;
    }
    return BgWinAddressMode::Signed;
}

PpuRegisters::TileMapAddressRange PpuRegisters::get_background_address_range() const {
    auto lcdc = get(Register::LcdcRegister);
    if (bitmanip::is_bit_set(lcdc, static_cast<int>(LcdcBits::BgTileMapArea))) {
        return TileMapAddressRange::High;
    }
    return TileMapAddressRange::Low;
}

PpuRegisters::TileMapAddressRange PpuRegisters::get_window_address_range() const {
    auto lcdc = get(Register::LcdcRegister);
    if (bitmanip::is_bit_set(lcdc, static_cast<int>(LcdcBits::WindowTileMapArea))) {
        return TileMapAddressRange::High;
    }
    return TileMapAddressRange::Low;
}

void PpuRegisters::set_register_bit(PpuRegisters::Register r, uint8_t bit_position,
                                    uint8_t bit_value) {
    auto& x = get(r);
    bitmanip::set(x, bit_position, bit_value > 0);
}

void PpuRegisters::increment_register(PpuRegisters::Register r) {
    auto& x = get(r);
    x++;
}

uint8_t PpuRegisters::get_register_bit(PpuRegisters::Register r, uint8_t bit_position) const {
    auto x = get(r);
    return bitmanip::bit_value(x, bit_position);
}

bool PpuRegisters::is_ppu_enabled() const {
    return get_register_bit(Register::LcdcRegister, static_cast<uint8_t>(LcdcBits::LcdAndPpuEnable))
           == 1;
}

bool PpuRegisters::was_oam_transfer_requested() const {
    return m_oam_transfer_requested;
}

void PpuRegisters::clear_oam_transfer_request() {
    m_oam_transfer_requested = false;
}