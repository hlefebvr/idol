//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_CONSTANT_H
#define OPTIMIZE_CONSTANT_H

#include "../../containers/Map.h"
#include "../parameters/Param.h"
#include "../numericals.h"
#include "../../containers/IteratorForward.h"

namespace idol {

    namespace Solution {
        class Primal;
        class Dual;
    }

    class Algorithm;
    class Constant;
    class QuadParam;
}

struct idol::QuadParam {
    const Param& key1;
    const Param& key2;
    const Constant& constant;
    QuadParam(const std::pair<Param, Param>& t_pair, const Constant& t_constant)
            : key1(t_pair.first), key2(t_pair.second), constant(t_constant) {}
};

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
class idol::Constant {
    Map<Param, double> m_linear_terms;
    Map<std::pair<Param, Param>, double, idol::impl::symmetric_pair_hash, idol::impl::symmetric_pair_equal_to> m_quadratic_terms;
    double m_constant = 0.;

    void insert_or_add(const Param& t_param, double t_value);
    void insert_or_add(const Param& t_param_1, const Param& t_param_2, double t_value);
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

    Constant(const Param& t_param_1, const Param& t_param_2, double t_value = 1.);

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
     * Sets the factor for a given pair of parameters in the constant term.
     *
     * If t_factor is zero, the entry for t_param is removed (i.e., not stored).
     * @param t_param_1 The parameter 1.
     * @param t_param_2 The parameter 3.
     * @param t_value The factor associated to the pair of parameters.
     */
    void set(const Param& t_param_1, const Param& t_param_2, double t_value);

    /**
     * Returns the factor associated to the parameter given as argument.
     *
     * If no factor is found, zero is returned.
     * @param t_param The queried parameter.
     */
    double get(const Param& t_param) const;

    double get(const Param& t_param_1, const Param& t_param_2) const;

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
    unsigned int size() const { return m_linear_terms.size(); }

    /**
     * Returns true if the Constant does not contain any Param-double pair.
     */
    bool is_numerical() const;

    auto linear() { return IteratorForward(m_linear_terms); }

    auto linear() const { return ConstIteratorForward(m_linear_terms); }

    auto quadratic() { return IteratorForward(m_quadratic_terms); }

    auto quadratic() const { return ConstIteratorForward(m_quadratic_terms); }

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
    Constant& operator+=(const Param& t_term);

    /**
     * Adds up another constant term (i.e., both the numerical terms and every Param-double pairs are added up).
     *
     * Resulting zero-entries are removed.
     * @param t_term
     */
    Constant& operator+=(const Constant& t_term);

    Constant& operator-=(const Constant& t_term);
    Constant& operator-=(double t_term);
    Constant& operator-=(Param t_term);

    double fix(const Solution::Primal& t_primals) const;

    double fix(const Solution::Dual& t_duals) const;

    static Constant Zero;
};

namespace idol {

    static std::ostream &operator<<(std::ostream &t_os, const Constant &t_coefficient) {

        const auto print_lin_term = [&t_os](const idol::Param &t_param, double t_coeff) {
            if (!idol::equals(t_coeff, 1., idol::ToleranceForSparsity)) {
                t_os << t_coeff << ' ';
            }
            t_os << t_param;
        };

        const auto print_quad_term = [&t_os](const std::pair<idol::Param, idol::Param> &t_pair, double t_coeff) {
            if (!idol::equals(t_coeff, 1., idol::ToleranceForSparsity)) {
                t_os << t_coeff << ' ';
            }
            t_os << t_pair.first << ' ' << t_pair.second;
        };

        const double constant = t_coefficient.numerical();

        bool first_term_has_been_printed = false;

        if (!idol::equals(constant, 0., idol::ToleranceForSparsity)) {
            t_os << constant;
            first_term_has_been_printed = true;
        }

        for (const auto& [param, coeff] : t_coefficient.linear()) {
            if (first_term_has_been_printed) {
                t_os << " + ";
            }
            print_lin_term(param, coeff);
            first_term_has_been_printed = true;
        }

        for (const auto& [pair, coeff] : t_coefficient.quadratic()) {
            if (first_term_has_been_printed) {
                t_os << " + ";
            }
            print_quad_term(pair, coeff);
            first_term_has_been_printed = true;
        }

        return t_os;
    }

}

#endif //OPTIMIZE_CONSTANT_H
