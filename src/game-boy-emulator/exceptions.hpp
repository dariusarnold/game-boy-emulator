#pragma once

#include <exception>
#include <string>

class NotImplementedError : public std::exception {
    std::string m_msg;
public:
    NotImplementedError(const std::string& msg);
};

class LogicError : public std::exception {
    std::string m_msg;
public:
    LogicError(const std::string& msg);
};
