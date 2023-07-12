//
// Created by henri on 10/10/22.
//

#ifndef OPTIMIZE_TIMER_H
#define OPTIMIZE_TIMER_H

#include <chrono>

namespace idol {
    class Timer;
}

class idol::Timer {
    using time = std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double>>;
    using duration = std::chrono::duration<double>;
    bool m_has_started = false, m_has_stopped = false;
    time m_starting_clock;
    time m_ending_clock;
    duration m_cumulative = duration(0);

    [[nodiscard]] duration as_duration() const;
public:
    enum Unit { Seconds, Milliseconds, Microseconds };
    void start();
    void stop();
    [[nodiscard]] double count(Unit t_unit = Seconds) const;
    [[nodiscard]] double cumulative_count(Unit t_unit = Seconds) const;
    [[nodiscard]] double factor(Unit t_unit) const;
};

#endif //OPTIMIZE_TIMER_H
