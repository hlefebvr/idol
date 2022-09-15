//
// Created by henri on 15/09/22.
//

#ifndef OPTIMIZE_LOG_H
#define OPTIMIZE_LOG_H

#include <ostream>

// see https://sematext.com/blog/logging-levels/#:~:text=A%20log%20level%20or%20log,look%20at%20the%20severity%20first.
// for log levels

enum LogLevel { Mute, Fatal, Error, Warn, Info, Debug, Trace };

class Log {
public:
    explicit Log(LogLevel t_msg_level);
    ~Log();
    static LogLevel level();
    static void set_level(LogLevel t_level);
    static std::ostream& os();
    static std::string current_time();

    template<class T> Log& operator<<(const T& t_object);
private:
    static LogLevel s_level;
    LogLevel m_msg_level;
};

template<class T>
Log &Log::operator<<(const T &t_object) {
    if (m_msg_level <= s_level) {
        if constexpr (std::is_arithmetic_v<T>) {
            if (is_pos_inf(t_object)) { os() << "+inf"; }
            else if (is_neg_inf(t_object)) { os() << "-inf"; }
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
    throw std::runtime_error("Unexpected log level: " + std::to_string(t_level));
}

#define EASY_LOG(required_level, then) \
if (required_level <= Log::level()) { Log _log(required_level); _log << then; }

#endif //OPTIMIZE_LOG_H
