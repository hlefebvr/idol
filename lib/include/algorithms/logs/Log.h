//
// Created by henri on 15/09/22.
//

#ifndef OPTIMIZE_LOG_H
#define OPTIMIZE_LOG_H

#include "../../modeling/numericals.h"
#include <ostream>
#include <unordered_map>
#include <cmath>

// see https://sematext.com/blog/logging-levels/#:~:text=A%20log%20level%20or%20log,look%20at%20the%20severity%20first.
// for log levels

enum LogLevel { Mute, Fatal, Error, Warn, Info, Debug, Trace };

enum Color { NotSpecified, Black, Red, Green, Yellow, Blue, Magenta, Cyan, White };

class Log {
public:
    explicit Log(LogLevel t_msg_level, std::string t_tag);
    ~Log();
    static LogLevel level();
    static void set_level(LogLevel t_level);
    static void set_color(const std::string& t_tag, Color t_color);
    static Color color(const std::string& t_tag);
    static std::ostream& os();
    static std::string current_time();

    template<class T> Log& operator<<(const T& t_object);
private:
    static LogLevel s_level;
    static std::string to_console_begin(Color t_color);
    static std::unordered_map<std::string, Color> s_colors;
    LogLevel m_msg_level;
    std::string m_tag;
};

template<class T>
Log &Log::operator<<(const T &t_object) {
    if (m_msg_level <= s_level) {
        if constexpr (std::is_arithmetic_v<T>) {
            if (is_pos_inf(t_object)) { os() << "+inf"; }
            else if (is_neg_inf(t_object)) { os() << "-inf"; }
            else if (std::abs((double) t_object) <= ToleranceForSparsity) { os() << 0.; }
            else { os() << t_object; }
        } else {
            os() << t_object;
        }
        return *this;
    }
    return *this;
}

static std::ostream& operator<<(std::ostream& t_os, LogLevel t_level) {
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
    throw std::runtime_error("Unexpected log_last_primal_solution level: " + std::to_string(t_level));
}

#define EASY_LOG(required_level, tag, then) \
if (required_level <= Log::level()) { Log _log(required_level, tag); _log << then; }

#endif //OPTIMIZE_LOG_H
