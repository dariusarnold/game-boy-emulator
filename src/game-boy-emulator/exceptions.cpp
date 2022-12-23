#include "exceptions.hpp"

#include <utility>


NotImplementedError::NotImplementedError(std::string msg) : m_msg(std::move(msg)) {}

const char* NotImplementedError::what() const noexcept {
    return m_msg.c_str();
}

LogicError::LogicError(std::string msg) : m_msg(std::move(msg)) {}

const char* LogicError::what() const noexcept {
    return m_msg.c_str();
}

LoadError::LoadError(std::string msg) : m_msg(std::move(msg)) {}

const char* LoadError::what() const noexcept {
    return m_msg.c_str();
}
