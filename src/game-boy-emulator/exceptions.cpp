#include "exceptions.hpp"


NotImplementedError::NotImplementedError(const std::string& msg): m_msg(msg) {}

LogicError::LogicError(const std::string& msg) : m_msg(msg) {}
