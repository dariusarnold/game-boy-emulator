#include "gpu.hpp"
#include "exceptions.hpp"
#include "memorymap.hpp"
#include "emulator.hpp"
#include "fmt/format.h"
#include "spdlog/spdlog.h"
#include "magic_enum.hpp"

#include <span>

Gpu::Gpu(Emulator* emulator) :
        m_registers(emulator->get_options().stub_ly),
        m_logger(spdlog::get("")),
        m_emulator(emulator) {}

uint8_t Gpu::read_byte(uint16_t address) {
    if (memmap::is_in(address, memmap::TileData)) {
        if (m_registers.is_ppu_enabled() && m_registers.get_mode() == PpuMode::PixelTransfer_3) {
            m_logger->error("GPU: VRAM read at {:04X} during pixel transfer", address);
        }
        return m_vram[address - memmap::VRamBegin];
    }
    if (memmap::is_in(address, memmap::OamRam)) {
        const auto mode = m_registers.get_mode();
        if (m_registers.is_ppu_enabled()
            && (mode == PpuMode::OamScan_2 || mode == PpuMode::PixelTransfer_3)) {
            m_logger->error("GPU: OAM read at {:04X} during mode {}", address,
                            magic_enum::enum_name(mode));
        }
        return reinterpret_cast<uint8_t*>(m_oam_ram.data())[address - memmap::OamRamBegin];
    }
    if (memmap::is_in(address, memmap::PpuIoRegisters)) {
        return m_registers.get_register_value(address);
    }
    if (memmap::is_in(address, memmap::TileMaps)) {
        return m_tile_maps[address - memmap::TileMapsBegin];
    }

    throw LogicError(fmt::format("GPU can't read from {:04X}", address));
}

void Gpu::write_byte(uint16_t address, uint8_t value) {
    if (memmap::is_in(address, memmap::TileData)) {
        if (m_registers.is_ppu_enabled() && m_registers.get_mode() == PpuMode::PixelTransfer_3) {
            m_logger->error("GPU: VRAM write at {:04X} during pixel transfer", address);
        }
        m_vram[address - memmap::VRamBegin] = value;
    } else if (memmap::is_in(address, memmap::PpuIoRegisters)) {
        if (address == 0xFF46) {
            throw NotImplementedError("OAM DMA access");
        }
        m_registers.set_register_value(address, value);
    } else if (memmap::is_in(address, memmap::OamRam)) {
        if (m_registers.is_ppu_enabled()
            && (m_registers.get_mode() == PpuMode::OamScan_2
                || m_registers.get_mode() == PpuMode::PixelTransfer_3)) {
            m_logger->error("GPU: Oam write at {:04X} during mode {}", address,
                            magic_enum::enum_name(m_registers.get_mode()));
        }
        reinterpret_cast<uint8_t*>(m_oam_ram.data())[address - memmap::OamRamBegin] = value;
    } else if (memmap::is_in(address, memmap::TileMaps)) {
        if (m_registers.is_ppu_enabled() && m_registers.get_mode() == PpuMode::PixelTransfer_3) {
            m_logger->error("GPU: VRAM write at {:04X} during pixel transfer", address);
        }
        m_tile_maps[address - memmap::TileMapsBegin] = value;
    } else {
        m_logger->error("GPU: unhandled write to {:04X}", address);
    }
}

std::span<uint8_t, 8192> Gpu::get_vram() {
    return {m_vram};
}

namespace {
// in M cycles
const int DURATION_OAM_SEARCH = 20;
const int DURATION_PIXEL_TRANSFER = 43;
const int DURATION_H_BLANK = 51;
const int DURATION_SCANLINE = DURATION_OAM_SEARCH + DURATION_PIXEL_TRANSFER + DURATION_H_BLANK;
const int DURATION_V_BLANK = 10 * (20 + 43 + 51);
} // namespace

void Gpu::cycle_elapsed_callback(size_t cycles_m_num) {
    (void)cycles_m_num;
    auto mode = m_registers.get_mode();
    m_clock_count++;
    switch (mode) {
    case PpuMode::OamScan_2:
        if (m_clock_count >= DURATION_OAM_SEARCH) {
            m_logger->info("GPU: cycle {}, LY {}, mode {}->{}", m_clock_count,
                           m_registers.get_register_value(PpuRegisters::Register::LyRegister),
                           static_cast<int>(m_registers.get_mode()),
                           static_cast<int>(PpuMode::PixelTransfer_3));

            m_clock_count = m_clock_count % DURATION_OAM_SEARCH;
            m_registers.set_mode(PpuMode::PixelTransfer_3);
        }
        break;
    case PpuMode::PixelTransfer_3:
        if (m_clock_count >= DURATION_PIXEL_TRANSFER) {
            m_logger->info("GPU: cycle {}, LY {}, mode {}->{}", m_clock_count,
                           m_registers.get_register_value(PpuRegisters::Register::LyRegister),
                           static_cast<int>(m_registers.get_mode()),
                           static_cast<int>(PpuMode::HBlank_0));

            m_clock_count = m_clock_count % DURATION_PIXEL_TRANSFER;
            m_registers.set_mode(PpuMode::HBlank_0);

        }
        break;
    case PpuMode::HBlank_0:
        if (m_clock_count >= DURATION_H_BLANK) {

            m_registers.increment_register(PpuRegisters::Register::LyRegister);

            auto new_mode = PpuMode::OamScan_2;
            // If we are on the last visible line we enter the vblank mode
            if (m_registers.get_register_value(PpuRegisters::Register::LyRegister) == 144) {
                new_mode = PpuMode::VBlank_1;
            }

            m_logger->info("GPU: cycle {}, LY {}, mode {}->{}", m_clock_count,
                           m_registers.get_register_value(PpuRegisters::Register::LyRegister),
                           static_cast<int>(m_registers.get_mode()), static_cast<int>(new_mode));

            m_clock_count = m_clock_count % DURATION_H_BLANK;
            m_registers.set_mode(new_mode);
        }
        break;
    case PpuMode::VBlank_1:
        if (m_clock_count >= DURATION_SCANLINE) {
            // Vblank duration is 10 scanlines
            m_registers.increment_register(PpuRegisters::Register::LyRegister);
            m_logger->info("GPU: cycle {}, LY {}, mode {}", m_clock_count,
                           m_registers.get_register_value(PpuRegisters::Register::LyRegister),
                           static_cast<int>(m_registers.get_mode()));
            m_clock_count = m_clock_count % DURATION_SCANLINE;

            if (m_registers.get_register_value(PpuRegisters::Register::LyRegister) == 154) {
                m_registers.set_register_value(PpuRegisters::Register::LyRegister, 0);
                m_logger->info("GPU: cycle {}, LY {}, mode {}->{}", m_clock_count,
                               m_registers.get_register_value(PpuRegisters::Register::LyRegister),
                               static_cast<int>(m_registers.get_mode()),
                               static_cast<int>(PpuMode::OamScan_2));
                m_registers.set_mode(PpuMode::OamScan_2);
            }
        }
        break;
    }
}

