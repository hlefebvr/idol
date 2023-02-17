//
// Created by henri on 15/09/22.
//
#include "backends/parameters/Logs.h"
#include <iostream>
#include <iomanip>

std::string Logs::current_time() {
    time_t     now = time(0);
    struct tm  tstruct{};
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}

std::ostream& operator<<(std::ostream& t_os, LogLevel t_level) {
    switch (t_level) {
        case Mute: return t_os << "mute";
        case Fatal: return t_os << "fatal";
        case Error: return t_os << "error";
        case Warn: return t_os << "warn";
        case Info: return t_os << "info";
        case Debug: return t_os << "debug";
        case Trace: return t_os << "trace";
        default:;
    }
    throw Exception("Enum out of bounds");
}