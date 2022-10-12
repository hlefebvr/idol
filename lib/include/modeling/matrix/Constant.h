//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_CONSTANT_H
#define OPTIMIZE_CONSTANT_H

#include "../../containers/Map.h"
#include "../parameters/Param.h"
#include "../numericals.h"

namespace Solution {
    class Primal;
    class Dual;
}

class Algorithm;

/**
 * Constant term modeling object.
 *
 * This class is used to build constant terms which will appear in models.
 *
 * Typically, they represent a real number. However, they can also
 * be an affine combination of Param. This can be useful for defining specific decomposition schemes such as Dantzig-Wolfe
 * reformulations where decision variables from a model A are seen as constant terms in a model B.
 *
 * It is composed of a numerical term and a set of Param-numerical pairs representing the following mathematical expression.
 *
 * \f$ double_0 + \sum_{j=1}^n double_j \times Param_j \f$
 *
 * Constant terms can be created by calling the appropriate constructor or by using overloaded operators such as + and *. For instance,
 * 2 * xi where xi is a Param will yield a Constant.
 */
class Constant {
    Map<Param, double> m_products;
    double m_constant = 0.;

    void insert_or_add(const Param& t_param, double t_value);
public:
    /**
     * Create a new constant term equal to zero.
     */
    Constant() = default;

    /**
     * Creates a new constant term representing t_value * t_param.
     * @param t_param The parameter.
     * @param t_value The factor for the parameter.
     */
    Constant(const Param& t_param, double t_value = 1.); // NOLINT(google-explicit-constructor)

    /**
     * Creates a new constant term equal to the constant given as argument.
     * @param t_constant The constant.
     */
    Constant(double t_constant); // NOLINT(google-explicit-constructor)

    Constant(const Constant&) = default;
    Constant(Constant&&) noexcept = default;

    Constant& operator=(const Constant&) = default;
    Constant& operator=(Constant&&) noexcept = default;

    /**
     * Sets the factor for a given parameter in the constant term.
     *
     * If t_factor is zero, the entry for t_param is removed (i.e., not stored).
     * @param t_param The parameter.
     * @param t_value The factor associated to the parameter.
     */
    void set(const Param& t_param, double t_value);

    /**
     * Returns the factor associated to the parameter given as argument.
     *
     * If no factor is found, zero is returned.
     * @param t_param The queried parameter.
     */
    double get(const Param& t_param) const;

    /**
     * Sets the numerical term equal to the constant given as parameter.
     * @param t_constant The numerical term.
     */
    void set_numerical(double t_constant) { m_constant = t_constant; }

    /**
     * Returns the numerical term of the constant, false otherwise.
     */
    double numerical() const { return m_constant; }

    /**
     * Returns true if the constant is equal to zero (i.e., if its numerical term is zero and no Param-double pair can be found)
     */
    bool is_zero() const;

    /**
     * Returns the number of Param-double pairs stored inside the Constant.
     */
    unsigned int size() const { return m_products.size(); }

    /**
     * Returns true if the Constant does not contain any Param-double pair.
     */
    bool is_numerical() const;

    using iterator = Map<Param, double>::iterator;
    using const_iterator = Map<Param, double>::const_iterator;

    iterator begin() { return m_products.begin(); }
    iterator end() { return m_products.end(); }
    const_iterator begin() const { return m_products.begin(); }
    const_iterator end() const { return m_products.end(); }
    const_iterator cbegin() const { return m_products.begin(); }
    const_iterator cend() const { return m_products.end(); }

    /**
     * Multiplies the Constant by t_factor (i.e., every Param-double pairs and the numerical term are multiplied).
     * @param t_factor The factor for multiplication.
     */
    Constant& operator*=(double t_factor);

    /**
     * Adds up a numerical term to the current numerical term.
     * @param t_term The numerical term to add.
     */
    Constant& operator+=(double t_term);

    /**
     * Adds a parameter to the constant (equivalent to adding Constant(t_param, 1)).
     *
     * Resulting zero entries are removed.
     * @param t_term The parameter to add.
     */
    Constant& operator+=(Param t_term);

    /**
     * Adds up another constant term (i.e., both the numerical terms and every Param-double pairs are added up).
     *
     * Resulting zero-entries are removed.
     * @param t_term
     */
    Constant& operator+=(const Constant& t_term);

    Constant& operator-=(double t_term);
    Constant& operator-=(Param t_term);
    Constant& operator-=(const Constant& t_term);

    double fix(const Solution::Primal& t_primals) const;

    double fix(const Solution::Dual& t_duals) const;

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

    const double constant = t_coefficient.numerical();

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
