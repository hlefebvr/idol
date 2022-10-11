//
// Created by henri on 10/10/22.
//

#ifndef OPTIMIZE_TIMER_H
#define OPTIMIZE_TIMER_H

#include <chrono>

class Timer {
    typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double>> time;
    bool m_has_started = false, m_has_stopped = false;
    time m_starting_clock;
    time m_ending_clock;
    double m_cumulative = 0;
public:
    void start();
    void stop();
    [[nodiscard]] double in_seconds() const;
    [[nodiscard]] double cumulative_time_in_seconds() const;
};
#endif //OPTIMIZE_TIMER_H
