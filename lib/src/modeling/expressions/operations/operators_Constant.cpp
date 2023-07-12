//
// Created by henri on 26/10/22.
//
#include "../../../../include/modeling/expressions/operations/operators_Constant.h"

using namespace idol;

/* PRODUCT */

Constant operator*(const Constant& t_coefficient, double t_factor) {
    return t_factor * Constant(t_coefficient);
}

Constant operator*(double t_factor, const Param& t_param) {
    return { t_param, t_factor };
}

Constant operator*(const Param& t_param, double t_factor) {
    return { t_param, t_factor };
}

Constant operator*(double t_factor, Constant&& t_coefficient) {
    Constant result(std::move(t_coefficient));
    result *= t_factor;
    return result;
}

Constant operator*(Constant&& t_coefficient, double t_factor) {
    return t_factor * std::move(t_coefficient);
}

Constant operator*(double t_factor, const Constant& t_coefficient) {
    return t_factor * Constant(t_coefficient);
}

/* ADDITION */

Constant operator+(Constant&& t_coefficient) {
    return std::move(t_coefficient);
}

Constant operator+(const Constant& t_coefficient) {
    return t_coefficient;
}

Constant operator+(double t_a, Constant&& t_b) {
    Constant result(std::move(t_b));
    result += t_a;
    return result;
}

Constant operator+(double t_a, const Constant& t_b) {
    return Constant(t_b) + t_a;
}

Constant operator+(Constant&& t_b, double t_a) {
    return t_a + std::move(t_b);
}

Constant operator+(const Constant& t_b, double t_a) {
    return t_a + Constant(t_b);
}

Constant operator+(const Param& t_a, const Param& t_b) {
    return Constant(t_a) + t_b;
}

Constant operator+(Constant&& t_a, const Constant& t_b) {
    Constant result(std::move(t_a));
    result += t_b;
    return result;
}

Constant operator+(const Constant& t_a, Constant&& t_b) {
    return std::move(t_b) + t_a;
}

Constant operator+(Constant&& t_a, Constant&& t_b) {
    return std::move(t_a) + t_b;
}

Constant operator+(const Constant& t_a, const Constant& t_b) {
    return Constant(t_a) + t_b;
}

/* DIFFERENCE */

Constant operator-(const Constant& t_coefficient) {
    return -1. * t_coefficient;
}

Constant operator-(double t_a, const Constant& t_b) {
    Constant result(t_a);
    result -= t_b;
    return result;
}

Constant operator-(Constant&& t_b, double t_a) {
    Constant result(std::move(t_b));
    result -= t_a;
    return result;
}

Constant operator-(const Constant& t_b, double t_a) {
    Constant result(t_b);
    result -= t_a;
    return result;
}

Constant operator-(const Param& t_a, const Param& t_b) {
    Constant result(t_a);
    result -= t_b;
    return result;
}

Constant operator-(Constant&& t_a, const Constant& t_b) {
    Constant result(std::move(t_a));
    result -= t_b;
    return result;
}

Constant operator-(const Constant& t_a, const Constant& t_b) {
    return Constant(t_a) - t_b;
}