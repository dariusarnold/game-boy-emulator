#include "addressbus.hpp"

#include "bootrom.hpp"
#include "cartridge.hpp"
#include "emulator.hpp"
#include "exceptions.hpp"
#include "gpu.hpp"
#include "memorymap.hpp"
#include "ram.hpp"
#include "timer.hpp"
#include "serial_port.hpp"

#include "spdlog/spdlog.h"


AddressBus::AddressBus(Emulator* emulator) : m_emulator(emulator), m_logger(spdlog::get("")) {}

uint8_t static io_registerFF42 = 0x64; // NOLINT

uint8_t AddressBus::read_byte(uint16_t address) const {
    if (m_emulator->is_booting() && memmap::is_in(address, memmap::BootRom)) {
        return m_emulator->get_boot_rom()->read_byte(address);
    }
    if (memmap::is_in(address, memmap::CartridgeRom)
        || memmap::is_in(address, memmap::CartridgeRam)) {
        return m_emulator->get_cartridge()->read_byte(address);
    }
    if (memmap::is_in(address, memmap::InternalRam) || memmap::is_in(address, memmap::HighRam)) {
        return m_emulator->get_ram()->read_byte(address);
    }
    if (memmap::is_in(address, memmap::VRam)) {
        return m_emulator->get_gpu()->read_byte(address);
    }
    if (memmap::is_in(address, memmap::SerialPort)) {
        return m_emulator->get_serial_port()->read_byte(address);
    }
    if (address == memmap::InterruptFlagBegin) {
        return m_emulator->get_interrupt_handler()->read_interrupt_flag();
    }
    if (address == memmap::InterruptEnableBegin) {
        return m_emulator->get_interrupt_handler()->read_interrupt_enable();
    }
    if (memmap::is_in(address, memmap::IORegisters)) {
        // TODO Those special cases are required for booting correctly
        if (address == 0xFF44) {
            return 0x90;
        }
        if (address == 0xFF42) {
            return io_registerFF42;
        }
        m_logger->warn("IGNORED: read from IO register {:04X}", address);
        return 0xFF;
    }
    throw NotImplementedError(fmt::format("Addressing unmapped memory byte at {:04X}", address));
}

void AddressBus::write_byte(uint16_t address, uint8_t value) {
    if (memmap::is_in(address, memmap::InternalRam) || memmap::is_in(address, memmap::HighRam)) {
        m_emulator->get_ram()->write_byte(address, value);
    } else if (memmap::is_in(address, memmap::VRam)) {
        m_emulator->get_gpu()->write_byte(address, value);
    } else if (memmap::is_in(address, memmap::CartridgeRom)
               || memmap::is_in(address, memmap::CartridgeRam)) {
        m_emulator->get_cartridge()->write_byte(address, value);
    } else if (memmap::is_in(address, memmap::SerialPort)) {
        m_emulator->get_serial_port()->write_byte(address, value);
    } else if (address == memmap::InterruptEnableBegin) {
        m_emulator->get_interrupt_handler()->write_interrupt_enable(value);
    } else if (address == memmap::InterruptFlagBegin) {
        m_emulator->get_interrupt_handler()->write_interrupt_flag(value);
    } else if (memmap::is_in(address, memmap::Timer)) {
        m_emulator->get_timer()->write_byte(address, value);
    } else if (memmap::is_in(address, memmap::IORegisters)) {
        if (m_emulator->is_booting()) {
            if (address == 0xFF50) {
                m_emulator->signal_boot_ended();
            }
        }
        if (address == 0xFF42) {
            io_registerFF42 = value;
        }
        m_logger->warn("IGNORED: write to IO registers {:04X}", address);
    } else {
        throw NotImplementedError(fmt::format("Writing unmapped memory byte at {:04X}", address));
    }
}
