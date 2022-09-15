//
// Created by henri on 15/09/22.
//
#include "algorithms/logs/Log.h"
#include <iostream>

LogLevel Log::s_level = Info;

LogLevel Log::level() {
    return s_level;
}

void Log::set_level(LogLevel t_level) {
    s_level = t_level;
}

Log::Log(LogLevel t_msg_level) : m_msg_level(t_msg_level) {
    *this << "[" << current_time() << "] [" << m_msg_level << "]\t";
}

std::ostream &Log::os() {
    return std::cout;
}

Log::~Log() {
    os() << std::endl;
}

std::string Log::current_time() {
    time_t     now = time(0);
    struct tm  tstruct{};
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}
