//
// Created by henri on 15/09/22.
//
#include "optimizers/Logger.h"
#include <iostream>
#include <iomanip>

std::ostream& operator<<(std::ostream& t_os, idol::LogLevel t_level) {
    using namespace idol;
    switch (t_level) {
        case Mute:  return t_os << "mute";
        case Fatal: return t_os << "fatal";
        case Error: return t_os << "error";
        case Warn:  return t_os << "warn";
        case Info:  return t_os << "info";
        case Debug: return t_os << "debug";
        case Trace: return t_os << "trace";
        default:;
    }
    throw Exception("Enum out of bounds");
}

std::ostream& operator<<(std::ostream& t_os, idol::Color t_color) {
    using namespace idol;
    switch (t_color) {

        case Default: return t_os << "default";
        case Black:   return t_os << "black";
        case Red:     return t_os << "red";
        case Green:   return t_os << "green";
        case Yellow:  return t_os << "yellow";
        case Blue:    return t_os << "blue";
        case Magenta: return t_os << "magenta";
        case Cyan:    return t_os << "cyan";
        case White:   return t_os << "white";
        default:;
    }
    throw Exception("Enum out of bounds");
}

idol::Logger::Logger(LogLevel t_level, Color t_color, const std::string &t_name)  : m_color(t_color) {

    switch (t_color) {
        case Black:   m_ostream << "\033[30m"; break;
        case Red  :   m_ostream << "\033[31m"; break;
        case Green:   m_ostream << "\033[32m"; break;
        case Yellow:  m_ostream << "\033[33m"; break;
        case Blue :   m_ostream << "\033[34m"; break;
        case Magenta: m_ostream << "\033[35m"; break;
        case Cyan :   m_ostream << "\033[36m"; break;
        case White:   m_ostream << "\033[37m"; break;
        case Default: [[fallthrough]];
        default:;
    }

    m_ostream << '[' << current_time() << "]\t";
    m_ostream << '[' << t_level << "]\t";
    m_ostream << '[' << t_name << "]\t";

    m_ostream << std::fixed << std::setprecision(2);
}

idol::Logger::~Logger() {

    if (m_color != Default) {
        m_ostream << "\033[0m";
    }

    m_ostream << std::endl;

}

std::string idol::Logger::current_time() {
    time_t     now = time(0);
    struct tm  tstruct{};
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}
