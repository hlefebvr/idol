//
// Created by henri on 15/09/22.
//

#ifndef OPTIMIZE_LOG_H
#define OPTIMIZE_LOG_H

#include "idol/modeling/numericals.h"
#include "idol/errors/Exception.h"
#include <ostream>
#include <iomanip>

// see https://sematext.com/blog/logging-levels/#:~:text=A%20log%20level%20or%20log,look%20at%20the%20severity%20first.
// for log_master levels

namespace idol {

    enum LogLevel { Mute, Fatal, Error, Warn, Info, Debug, Trace };

    enum Color { Default, Black, Red, Green, Yellow, Blue, Magenta, Cyan, White };

    class Logger;

    std::ostream& operator<<(std::ostream& t_os, idol::LogLevel t_level);

    std::ostream& operator<<(std::ostream& t_os, idol::Color t_color);

}

class idol::Logger {
    std::ostream& m_ostream = std::cout;
    Color m_color;
public:
    Logger(LogLevel t_level, Color t_color, const std::string& t_name);

    ~Logger();

    static std::string current_time();

    template<class U> Logger& operator<<(const U& t_object);
};

template<class U>
idol::Logger &idol::Logger::operator<<(const U &t_object) {

    if constexpr (std::is_arithmetic_v<U>) {

        if (is_pos_inf(t_object)) { m_ostream << "+inf"; }
        else if (is_neg_inf(t_object)) { m_ostream << "-inf"; }
        else { m_ostream << t_object; }

    } else {

        m_ostream << t_object;

    }

    return *this;
}

#define idol_Log(t_required_level, msg) \
if ((t_required_level) <= this->log_level()) { \
    ::idol::Logger(t_required_level, this->log_color(), this->name()) << msg; \
}

#endif //OPTIMIZE_LOG_H
