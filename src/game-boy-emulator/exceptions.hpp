#pragma once

#include <stdexcept>
#include <string>

// Thrown when a game uses a feature which is not yet implemented.
class NotImplementedError : public std::exception {
    std::string m_msg;

public:
    explicit NotImplementedError(std::string msg);
    [[nodiscard]] const char* what() const noexcept override;
};

// Thrown for cases where something should not happen but did.
// E.g. cartridge saying it has no RAM but trying to access cartridge RAM.
class LogicError : public std::exception {
    std::string m_msg;

public:
    explicit LogicError(std::string msg);
    [[nodiscard]] const char* what() const noexcept override;
};

// Thrown when a file failed to load.
class LoadError : public std::exception {
    std::string m_msg;

public:
    explicit LoadError(std::string msg);
    [[nodiscard]] const char* what() const noexcept override;
};
