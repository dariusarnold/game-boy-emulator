#pragma once

#include <stdexcept>
#include <string>

class NotImplementedError : public std::exception {
    std::string m_msg;

public:
    NotImplementedError(const std::string& msg);
    const char* what() const noexcept override;
};

class LogicError : public std::exception {
    std::string m_msg;

public:
    LogicError(const std::string& msg);
    const char* what() const noexcept override;
};
