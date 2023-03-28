//
// Created by henri on 10/10/22.
//
#include "optimizers/Timer.h"
#include <stdexcept>

void Timer::start() {
    m_starting_clock = std::chrono::high_resolution_clock::now();
    m_has_started = true;
    m_has_stopped = false;
}

void Timer::stop() {
    if (m_has_stopped) {
        throw std::runtime_error("Timer has already been stopped.");
    }

    m_ending_clock = std::chrono::high_resolution_clock::now();
    m_has_stopped = true;
    m_cumulative += m_ending_clock - m_starting_clock;
}

Timer::duration Timer::as_duration() const {
    if (!m_has_started && !m_has_stopped) {
        return duration(0);
    }

    if (!m_has_stopped) {
        return std::chrono::high_resolution_clock::now() - m_starting_clock;
    }

    return m_ending_clock - m_starting_clock;
}

double Timer::count(Unit t_unit) const {
    return (double) std::chrono::duration_cast<std::chrono::microseconds>(as_duration()).count() / factor(t_unit);
}

double Timer::cumulative_count(Unit t_unit) const {
    double result = 0.;
    if (!m_has_stopped) {
        result += count(t_unit);
    }
    result += (double) std::chrono::duration_cast<std::chrono::microseconds>(m_cumulative).count() / factor(t_unit);
    return result;
}

double Timer::factor(Timer::Unit t_unit) const {
    switch (t_unit) {
        case Seconds:      return 1e+6;
        case Milliseconds: return 1e+3;
        case Microseconds: return 1e+1;
        default:;
    }
    throw std::runtime_error("Enum out of bounds.");
}
