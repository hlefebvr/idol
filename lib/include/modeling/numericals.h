//
// Created by henri on 01/09/22.
//

#ifndef OPTIMIZE_NUMERICALS_H
#define OPTIMIZE_NUMERICALS_H

#include <limits>
#include <cmath>

static constexpr double Inf = std::numeric_limits<double>::max();
static double TolFeas = 1e-8;

static bool equals(double t_a, double t_b, double t_tolerance) {
    return std::abs(t_a - t_b) <= t_tolerance;
}

#endif //OPTIMIZE_NUMERICALS_H
