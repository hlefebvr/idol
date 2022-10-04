//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_CONSTANT_H
#define OPTIMIZE_CONSTANT_H

#include "../../containers/Map.h"
#include "../parameters/Param.h"
#include "modeling/numericals.h"

class Constant {
    Map<Param, double> m_products;
    double m_constant = 0.;

    void insert_or_add(const Param& t_param, double t_value);
public:
    Constant() = default;
    Constant(const Param& t_param, double t_value = 1.); // NOLINT(google-explicit-constructor)
    Constant(double t_constant); // NOLINT(google-explicit-constructor)

    Constant(const Constant&) = default;
    Constant(Constant&&) noexcept = default;

    Constant& operator=(const Constant&) = default;
    Constant& operator=(Constant&&) noexcept = default;

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

    Constant& operator*=(double t_factor);

    Constant& operator+=(double t_term);
    Constant& operator+=(Param t_term);
    Constant& operator+=(const Constant& t_term);
    Constant& operator-=(double t_term);
    Constant& operator-=(Param t_term);
    Constant& operator-=(const Constant& t_term);

    static Constant Zero;
};

Constant operator*(double t_factor, const Param& t_param);

Constant operator*(double t_factor, const Constant& t_coefficient);

Constant operator+(Constant t_a, const Constant& t_b);

Constant operator-(const Constant& t_coefficient);

static std::ostream& operator<<(std::ostream& t_os, const Constant& t_coefficient) {

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

#endif //OPTIMIZE_CONSTANT_H
