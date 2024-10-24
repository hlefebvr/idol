//
// Created by henri on 10/10/22.
//

#ifndef OPTIMIZE_TIMER_H
#define OPTIMIZE_TIMER_H

#include <chrono>

namespace idol {
    class Timer;
}

/**
 * This class is used to measure the time it takes for a certain operation to complete.
 */
class idol::Timer {
    using time = std::chrono::high_resolution_clock::time_point;
    using duration = std::chrono::duration<double>;
    bool m_has_started = false, m_has_stopped = false;
    time m_starting_clock;
    time m_ending_clock;
    duration m_cumulative = duration(0);

    [[nodiscard]] duration as_duration() const;
public:
    enum Unit { Seconds, Milliseconds, Microseconds };

    /**
     * Starts the timer.
     */
    void start();

    /**
     * Stops the timer.
     */
    void stop();

    /**
     * Returns the time elapsed since the timer was started.
     * @param t_unit the unit of time to return the elapsed time in.
     * @return the time elapsed since the timer was started.
     */
    [[nodiscard]] double count(Unit t_unit = Seconds) const;

    /**
     * Returns the cumulative elapsed time
     * @param t_unit the unit of time to return the elapsed time in.
     * @return the cumulative elapsed time
     */
    [[nodiscard]] double cumulative_count(Unit t_unit = Seconds) const;

    [[nodiscard]] double factor(Unit t_unit) const;
};

#endif //OPTIMIZE_TIMER_H
