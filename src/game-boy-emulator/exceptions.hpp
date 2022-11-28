#pragma once

#include <stdexcept>
#include <string>

class NotImplementedError : public std::exception {
    std::string m_msg;

public:
    explicit NotImplementedError(std::string msg);
    [[nodiscard]] const char* what() const noexcept override;
};

class LogicError : public std::exception {
    std::string m_msg;

public:
    explicit LogicError(std::string msg);
    [[nodiscard]] const char* what() const noexcept override;
};
