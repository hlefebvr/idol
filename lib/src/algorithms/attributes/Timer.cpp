//
// Created by henri on 10/10/22.
//
#include "../../../include/algorithms/attributes/Timer.h"
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
    m_cumulative += in_seconds();
}

double Timer::in_seconds() const {
    constexpr double convert_factor = 0.001;

    if (!m_has_started && !m_has_stopped) {
        return 0.0;
    }

    if (!m_has_stopped) {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_starting_clock).count() * convert_factor;
    }

    return std::chrono::duration_cast<std::chrono::milliseconds>(m_ending_clock - m_starting_clock).count() * convert_factor;
}

double Timer::cumulative_time_in_seconds() const {
    if (!m_has_stopped) {
        return m_cumulative + in_seconds();
    }
    return m_cumulative;
}