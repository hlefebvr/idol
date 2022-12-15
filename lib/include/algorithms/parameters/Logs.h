//
// Created by henri on 15/09/22.
//

#ifndef OPTIMIZE_LOG_H
#define OPTIMIZE_LOG_H

#include "../../modeling/numericals.h"
#include "../../errors/Exception.h"
#include <ostream>
#include <iomanip>

// see https://sematext.com/blog/logging-levels/#:~:text=A%20log%20level%20or%20log,look%20at%20the%20severity%20first.
// for log levels

enum LogLevel { Mute, Fatal, Error, Warn, Info, Debug, Trace };

std::ostream& operator<<(std::ostream& t_os, LogLevel t_level);

enum Color { Default, Black, Red, Green, Yellow, Blue, Magenta, Cyan, White };

namespace impl {
    template<class T>
    struct Log {
        static LogLevel level;
        static Color color;
        static std::ostream& ostream;
        template<class U> Log<T>& operator<<(const U& t_object);
        Log() {
            switch (color) {
                case Black:   ostream << "\033[30m"; break;
                case Red  :   ostream << "\033[31m"; break;
                case Green:   ostream << "\033[32m"; break;
                case Yellow:  ostream << "\033[33m"; break;
                case Blue :   ostream << "\033[34m"; break;
                case Magenta: ostream << "\033[35m"; break;
                case Cyan :   ostream << "\033[36m"; break;
                case White:   ostream << "\033[37m"; break;
                case Default: [[fallthrough]];
                default:;
            }
            ostream << std::fixed
                    << std::setprecision(2);
        }
        ~Log() {
            if (color != Default) {
                ostream << "\033[0m";
            }
            ostream << std::endl;
        }
    };

    template<class T> LogLevel Log<T>::level = Warn;
    template<class T> Color Log<T>::color = Default;
    template<class T> std::ostream& Log<T>::ostream = std::cout;
}

template<class T>
template<class U>
impl::Log<T> &impl::Log<T>::operator<<(const U &t_object) {

    if constexpr (std::is_arithmetic_v<U>) {
        if (is_pos_inf(t_object)) { ostream << "+inf"; }
        else if (is_neg_inf(t_object)) { ostream << "-inf"; }
        else { ostream << t_object; }
    } else {
        ostream << t_object;
    }

    return *this;
}

class Logs {
public:
    static std::string current_time();

    template<class T> static void set_level(LogLevel t_level) {
        impl::Log<T>::level = t_level;
    }

    template<class T> static void set_color(Color t_color) {
        impl::Log<T>::color = t_color;
    }

    template<class T> static void set_ostream(std::ostream& t_ostream) {
        impl::Log<T>::ostream = t_ostream;
    }

};

#define idol_Log(t_required_level, tag, msg) \
if ((t_required_level) <= ::impl::Log<tag>::level) { \
    ::impl::Log<tag>() << "[" << Logs::current_time() << "]\t[" << (t_required_level) << "]\t[" << #tag << "] \t" << msg; \
     \
}
#endif //OPTIMIZE_LOG_H
