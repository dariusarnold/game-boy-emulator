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
#include "interrupthandler.hpp"
#include "apu.hpp"

#include "spdlog/spdlog.h"


AddressBus::AddressBus(Emulator* emulator) : m_emulator(emulator), m_logger(spdlog::get("")) {}

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
    if (memmap::is_in(address, memmap::VRam) || memmap::is_in(address, memmap::PpuIoRegisters)) {
        return m_emulator->get_gpu()->read_byte(address);
    }
    if (memmap::is_in(address, memmap::SerialPort)) {
        return m_emulator->get_serial_port()->read_byte(address);
    }
    if (memmap::is_in(address, memmap::InterruptFlag)) {
        return m_emulator->get_interrupt_handler()->read_interrupt_flag();
    }
    if (memmap::is_in(address, memmap::InterruptEnable)) {
        return m_emulator->get_interrupt_handler()->read_interrupt_enable();
    }
    throw NotImplementedError(fmt::format("Addressing unmapped memory byte at {:04X}", address));
}

void AddressBus::write_byte(uint16_t address, uint8_t value) {
    if (memmap::is_in(address, memmap::InternalRam) || memmap::is_in(address, memmap::HighRam)) {
        m_emulator->get_ram()->write_byte(address, value);
    } else if (memmap::is_in(address, memmap::VRam) || memmap::is_in(address, memmap::OamRam)
               || memmap::is_in(address, memmap::PpuIoRegisters)) {
        m_emulator->get_gpu()->write_byte(address, value);
    } else if (memmap::is_in(address, memmap::CartridgeRom)
               || memmap::is_in(address, memmap::CartridgeRam)) {
        m_emulator->get_cartridge()->write_byte(address, value);
    } else if (memmap::is_in(address, memmap::SerialPort)) {
        m_emulator->get_serial_port()->write_byte(address, value);
    } else if (memmap::is_in(address, memmap::InterruptEnable)) {
        m_emulator->get_interrupt_handler()->write_interrupt_enable(value);
    } else if (memmap::is_in(address, memmap::InterruptFlag)) {
        m_emulator->get_interrupt_handler()->write_interrupt_flag(value);
    } else if (memmap::is_in(address, memmap::Timer)) {
        m_emulator->get_timer()->write_byte(address, value);

    } else if (memmap::is_in(address, memmap::Apu)) {
        m_emulator->get_apu()->write_byte(address, value);
    } else if (memmap::is_in(address, memmap::DisableBootRom)) {
        if (m_emulator->is_booting()) {
            m_emulator->signal_boot_ended();
        } else {
            m_logger->error("Signal boot end despite not booting");
        }
    } else if (memmap::is_in(address, memmap::Prohibited)) {
        // Some games do this (e.g. Tetris) because of bugs. We should not crash.
        m_logger->error("Write to prohibited memory");
    } else {
        throw NotImplementedError(fmt::format("Writing unmapped memory byte at {:04X}", address));
    }
}
