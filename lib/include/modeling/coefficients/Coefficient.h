//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_COEFFICIENT_H
#define OPTIMIZE_COEFFICIENT_H

#include "../../containers/Map.h"
#include "../parameters/Param.h"
#include "modeling/numericals.h"

class Coefficient {
    Map<Param, double> m_products;
    double m_constant = 0.;

    void insert_or_add(const Param& t_param, double t_value);
public:
    Coefficient() = default;
    Coefficient(const Param& t_param, double t_value = 1.); // NOLINT(google-explicit-constructor)
    Coefficient(double t_constant); // NOLINT(google-explicit-constructor)

    Coefficient(const Coefficient&) = default;
    Coefficient(Coefficient&&) noexcept = default;

    Coefficient& operator=(const Coefficient&) = default;
    Coefficient& operator=(Coefficient&&) noexcept = default;

    void set(const Param& t_param, double t_value);

    double get(const Param& t_param) const;

    void set_constant(double t_constant) { m_constant = t_constant; }

    double constant() const { return m_constant; }

    bool is_zero() const;

    unsigned int size() const { return m_products.size(); }

    bool is_numerical() const;

    using iterator = Map<Param, double>::iterator;
    using const_iterator = Map<Param, double>::const_iterator;

    iterator begin() { return m_products.begin(); }
    iterator end() { return m_products.end(); }
    const_iterator begin() const { return m_products.begin(); }
    const_iterator end() const { return m_products.end(); }
    const_iterator cbegin() const { return m_products.begin(); }
    const_iterator cend() const { return m_products.end(); }

    Coefficient& operator*=(double t_factor);

    Coefficient& operator+=(double t_term);
    Coefficient& operator+=(Param t_term);
    Coefficient& operator+=(const Coefficient& t_term);

    static Coefficient Zero;
};

Coefficient operator*(double t_factor, const Param& t_param);

Coefficient operator*(double t_factor, const Coefficient& t_coefficient);

Coefficient operator+(Coefficient t_a, const Coefficient& t_b);

static std::ostream& operator<<(std::ostream& t_os, const Coefficient& t_coefficient) {

    const auto print_term = [&t_os](const Param& t_param, double t_coeff) {
        if (!equals(t_coeff, 1., ToleranceForSparsity)) {
            t_os << t_coeff << ' ';
        }
        t_os << t_param;
    };

    const double constant = t_coefficient.constant();

    auto it = t_coefficient.begin();
    const auto end = t_coefficient.end();

    if (it == end) {
        return t_os << constant;
    }

    if (!equals(constant, 0., ToleranceForSparsity)) {
        t_os << constant << " + ";
    }

    print_term(it->first, it->second);

    for (++it ; it != end ; ++it) {
        const auto& [param, coeff] = *it;
        t_os << " + ";
        print_term(it->first, it->second);
    }

    return t_os;
}

#endif //OPTIMIZE_COEFFICIENT_H
