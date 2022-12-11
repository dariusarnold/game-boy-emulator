#include "serial_port.hpp"
#include "emulator.hpp"
#include "interrupthandler.hpp"
#include "exceptions.hpp"
#include "spdlog/spdlog.h"

SerialPort::SerialPort(Emulator* emulator) : m_emulator(emulator), m_logger(spdlog::get("")) {}

namespace {
const uint16_t ADDRESS_SERIAL_BUFFER = 0xFF01;
const uint16_t ADDRESS_SERIAL_CONTROL = 0xFF02;
} // namespace

void SerialPort::write_byte(uint16_t address, uint8_t value) {
    if (address == ADDRESS_SERIAL_BUFFER) {
        m_logger->debug("Writing {:02X} to serial buffer", value);
        m_serial_buffer = value;
        m_serial_written.push_back(static_cast<char>(value));
    }
    if (address == ADDRESS_SERIAL_CONTROL) {
        if (value == 0x81) {
            m_logger->debug("Request serial transfer");
            m_serial_control = 0x1;
            m_emulator->get_interrupt_handler()->request_interrupt(
                InterruptHandler::InterruptType::SerialLink);
        }
    }
}

uint8_t SerialPort::read_byte(uint16_t address) {
    if (address == ADDRESS_SERIAL_BUFFER) {
        m_logger->debug("Serial port read buffer");
        return m_serial_buffer;
    }
    if (address == ADDRESS_SERIAL_CONTROL) {
        m_logger->debug("Serial port read control");
        return m_serial_control;
    }
    throw LogicError(fmt::format("Invalid read in serial port"));
}

SerialPort::~SerialPort() {
    m_logger->error("Serial out: {}", m_serial_written);
}

std::string SerialPort::get_buffer() const {
    return m_serial_written;
}
