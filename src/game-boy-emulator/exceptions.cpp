#include "exceptions.hpp"


NotImplementedError::NotImplementedError(const std::string& msg): m_msg(msg) {}

const char* NotImplementedError::what() const noexcept {
    return m_msg.c_str();
}

LogicError::LogicError(const std::string& msg) : m_msg(msg) {}

const char* LogicError::what() const noexcept {
    return m_msg.c_str();
}
