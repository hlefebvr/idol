//
// Created by henri on 15/09/22.
//
#include "algorithms/logs/Log.h"
#include <iostream>
#include <iomanip>

LogLevel Log::s_level = Info;
std::unordered_map<std::string, Color> Log::s_colors;

LogLevel Log::level() {
    return s_level;
}

void Log::set_level(LogLevel t_level) {
    s_level = t_level;
}

Log::Log(LogLevel t_msg_level, std::string t_tag) : m_msg_level(t_msg_level), m_tag(std::move(t_tag)) {
    *this
        << to_console_begin(color(m_tag))
        << "[" << current_time() << "] [" << std::setw(5) << m_msg_level << "] [" << m_tag << "]\t";
}

std::ostream &Log::os() {
    return std::cout;
}

Log::~Log() {
    if (const auto c = color(m_tag) ; c == NotSpecified) {
        os() << std::endl;
    } else {
        os() << "\033[0m" << std::endl;
    }
}

std::string Log::current_time() {
    time_t     now = time(0);
    struct tm  tstruct{};
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}

std::string Log::to_console_begin(Color t_color) {
    switch (t_color) {
        case NotSpecified: return "";
        case Black:   return "\033[30m";
        case Red  :   return "\033[31m";
        case Green:   return "\033[32m";
        case Yellow:  return "\033[33m";
        case Blue :   return "\033[34m";
        case Magenta: return "\033[35m";
        case Cyan :   return "\033[36m";
        case White:   return "\033[37m";
        default:;
    }
    return "";
}

Color Log::color(const std::string &t_tag) {
    auto it = s_colors.find(t_tag);
    return it == s_colors.end() ? NotSpecified : it->second;
}

void Log::set_color(const std::string &t_tag, Color t_color) {
    auto [it, success] = s_colors.emplace(t_tag, t_color);
    if (!success) {
        it->second = t_color;
    }
}
