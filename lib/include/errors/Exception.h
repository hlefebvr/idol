//
// Created by henri on 21/09/22.
//

#ifndef OPTIMIZE_EXCEPTION_H
#define OPTIMIZE_EXCEPTION_H

#include <iostream>

class Exception : public std::exception {
    const std::string m_message;
public:
    explicit Exception(std::string t_message) : m_message(std::move(t_message)) {}

    [[nodiscard]] const char * what () const noexcept override { return m_message.c_str(); }
};

#endif //OPTIMIZE_EXCEPTION_H
