//
// Created by henri on 01/09/22.
//

#ifndef OPTIMIZE_NUMERICALS_H
#define OPTIMIZE_NUMERICALS_H

#include <limits>
#include <cmath>

static constexpr double Inf = 1e20;
static double ToleranceForSparsity = 1e-8;
static double ToleranceForRelativeGapMIP = 1e-4;
static double ToleranceForAbsoluteGapMIP = 1e-6;
static double ToleranceForAbsoluteGapPricing = 1e-6;
static double ToleranceForIntegrality = 1e-5;

static bool is_pos_inf(double t_value) {
    return t_value >= Inf;
}

static bool is_neg_inf(double t_value) {
    return t_value <= -Inf;
}

static bool is_inf(double t_value) {
    return is_pos_inf(t_value) || is_neg_inf(t_value);
}

static bool equals(double t_a, double t_b, double t_tolerance) {
    return std::abs(t_a - t_b) <= t_tolerance;
}

#endif //OPTIMIZE_NUMERICALS_H
