//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_CONSTANT_H
#define OPTIMIZE_CONSTANT_H

#include "idol/utils/Map.h"
#include "idol/modeling/parameters/Param.h"
#include "idol/modeling/numericals.h"
#include "idol/utils/IteratorForward.h"
#include "idol/utils/Point.h"

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
 * Constant term modeling-old object.
 *
 * This class is used to build constant terms which will appear in models.
 *
 * Typically, they represent a real number. However, they can also
 * be an affine combination of `Param`. This can be useful to define specific decomposition schemes such as Dantzig-Wolfe
 * reformulations where decision variables from a model `A` are seen as constant terms in a model `B`.
 *
 * It is composed of a numerical term and a set of `{ Param, numerical }` pairs representing the following mathematical expression.
 *
 * \f$ \text{numerical}_0 + \sum_{j=1}^n \text{numerical}_j \times \text{Param}_j \f$
 *
 * Constant terms can be created by calling the appropriate constructor, or by using overloaded operators such as `+` and `*`.
 * For instance, `2 * xi` where `xi` is a `Param` will yield a `Constant`.
 */
class idol::Constant {

    using MapForLinearTerms = Map<Param, double>;
    using MapForQuadraticTerms = Map<idol::Pair<Param, Param>, double, idol::impl::symmetric_pair_hash, idol::impl::symmetric_pair_equal_to>;


    static MapForLinearTerms s_empty_linear_terms;
    static MapForQuadraticTerms s_empty_quadratic_terms;

    std::unique_ptr<Map<Param, double>> m_linear_terms;
    std::unique_ptr<MapForQuadraticTerms> m_quadratic_terms;
    double m_constant = 0.;

    void insert_or_add(const Param& t_param, double t_value);
    void insert_or_add(const Param& t_param_1, const Param& t_param_2, double t_value);

    void create_linear_map_if_not_exists();
    void delete_linear_map_if_empty();
    void create_quadratic_map_if_not_exists();
    void delete_quadratic_map_if_empty();
public:
    /**
     * Create a new constant term equal to zero.
     */
    Constant() = default;

    /**
     * Constructor.
     *
     * Creates a new constant term representing `t_value * t_param`.
     * @param t_param The parameter.
     * @param t_value The coefficient for the parameter.
     */
    Constant(const Param& t_param, double t_value = 1.); // NOLINT(google-explicit-constructor)

    /**
     * Constructor.
     *
     * Creates a new constant term representing `t_value * t_param_1 * t_param_2`.
     * @param t_param_1 The first parameter.
     * @param t_param_2 The second parameter.
     * @param t_value The coefficient for the parameters.
     */
    Constant(const Param& t_param_1, const Param& t_param_2, double t_value = 1.);

    /**
     * Constructor.
     *
     * Creates a new constant term equal to the constant given as argument.
     * @param t_constant The constant.
     */
    Constant(double t_constant); // NOLINT(google-explicit-constructor)

    /**
     * Copy constructor.
     */
    Constant(const Constant& t_src);

    /**
     * Move constructor.
     */
    Constant(Constant&&) noexcept = default;

    /**
     * Copy-assignment operator.
     * @return *this
     */
    Constant& operator=(const Constant& t_rhs);

    /**
     * Move-assignment operator.
     * @return *this
     */
    Constant& operator=(Constant&&) noexcept = default;

    /**
     * Sets the coefficient for a given parameter in the constant term.
     *
     * If t_coefficient is zero, the entry for `t_param` is removed (i.e., not stored). The tolerance `Tolerance::Sparsity`
     * is used to compare with zero.
     * @param t_param The parameter.
     * @param t_value The coefficient associated to the parameter.
     */
    void set(const Param& t_param, double t_value);

    /**
     * Sets the coefficient for a given pair of parameters in the constant term.
     *
     * If `t_coefficient` is zero, the entry for t_param is removed (i.e., not stored). The tolerance `Tolerance::Sparsity`
     * is used to compare with zero.
     * @param t_param_1 The first parameter.
     * @param t_param_2 The second parameter.
     * @param t_value The coefficient associated to the pair of parameters.
     */
    void set(const Param& t_param_1, const Param& t_param_2, double t_value);

    /**
     * Returns the coefficient of the parameter given as argument.
     *
     * If no coefficient is found, zero is returned.
     * @param t_param The queried parameter.
     */
    double get(const Param& t_param) const;

    /**
     * Returns the coefficient of the pair of parameters given as arguments.
     * @param t_param_1 The first parameter.
     * @param t_param_2 The second parameter.
     * @return The queried parameters.
     */
    double get(const Param& t_param_1, const Param& t_param_2) const;

    /**
     * Sets the numerical term equal to the constant given as parameter.
     * @param t_constant The numerical term.
     */
    void set_numerical(double t_constant) { m_constant = t_constant; }

    /**
     * Returns the numerical term of the constant.
     */
    double numerical() const { return m_constant; }

    /**
     * Returns the numerical term of the constant, and throws an exception if the `Constant` also contains non-numerical
     * terms, i.e., the `Constant` is a parametrized constant.
     * @return The numerical term of the constant
     */
    double as_numerical() const;

    /**
     * Returns true if the constant is equal to zero (i.e., if its numerical term is zero and no `{ Param, numerical }` pair can be found).
     * The tolerance `Tolerance::Sparsity` is used to compare with zero.
     */
    bool is_zero() const;

    /**
     * Returns the number of `{ Param, numerical }` pairs stored inside the `Constant`.
     */
    unsigned int size() const { return m_linear_terms ? m_linear_terms->size() : 0; }

    /**
     * Returns true if the `Constant` is a pure numerical, i.e., the `Constant` is not a parametrized constant, false
     * otherwise.
     */
    bool is_numerical() const;

    /**
     * Returns the linear part of the parametrized expression.
     * @return The linear part of the parametrized expression.
     */
    auto linear() { return IteratorForward(m_linear_terms ? *m_linear_terms : s_empty_linear_terms); }

    /**
     * Returns the linear part of the parametrized expression.
     * @return The linear part of the parametrized expression.
     */
    auto linear() const { return ConstIteratorForward(m_linear_terms ? *m_linear_terms : s_empty_linear_terms); }

    /**
     * Returns the quadratic part of the parametrized expression.
     * @return The quadratic part of the parametrized expression.
     */
    auto quadratic() { return IteratorForward(m_quadratic_terms ? *m_quadratic_terms : s_empty_quadratic_terms); }

    /**
     * Returns the quadratic part of the parametrized expression.
     * @return The quadratic part of the parametrized expression.
     */
    auto quadratic() const { return ConstIteratorForward(m_quadratic_terms ? *m_quadratic_terms : s_empty_quadratic_terms); }

    /**
     * Multiplies the `Constant` by `t_coefficient` (i.e., every `{ Param, numerical }` pairs and the numerical term are multiplied).
     * @param t_coefficient The coefficient for multiplication.
     */
    Constant& operator*=(double t_coefficient);

    Constant& operator/=(double t_coefficient);

    /**
     * Adds up a numerical term to the current numerical term.
     * @param t_term The numerical term to add.
     */
    Constant& operator+=(double t_term);

    /**
     * Adds a parameter to the constant (equivalent to adding `Constant(t_param, 1)`).
     *
     * Resulting zero entries are removed. The tolerance `Tolerance::Sparsity` is used to compare with zero.
     * @param t_term The parameter to add.
     */
    Constant& operator+=(const Param& t_term);

    /**
     * Adds up another constant term (i.e., both the numerical terms and every `{ Param, numerical }` pairs are added up).
     *
     * Resulting zero-entries are removed. The tolerance `Tolerance::Sparsity` is used to compare with zero.
     * @param t_term
     */
    Constant& operator+=(const Constant& t_term);

    /**
     * Subtracts another constant term (i.e., both the numerical terms and every `{ Param, numerical }` pairs are substracted).
     *
     * Resulting zero-entries are removed. The tolerance `Tolerance::Sparsity` is used to compare with zero.
     * @param t_term The constant to subtract.
     */
    Constant& operator-=(const Constant& t_term);

    /**
     * Subtracts a numerical term to the current numerical term.
     * @param t_term The numerical term to subtract.
     */
    Constant& operator-=(double t_term);

    /**
     * Subtracts parameter to the constant (equivalent to subtracting `Constant(t_param, 1)`).
     *
     * Resulting zero entries are removed. The tolerance `Tolerance::Sparsity` is used to compare with zero.
     * @param t_term The parameter to subtract.
     */
    Constant& operator-=(Param t_term);

    /**
     * Computes the resulting numerical by replacing each `Param`  by their corresponding values in `t_primals`.
     * @param t_primals Primal values associated to the stored `Param`
     * @return the resulting numerical
     */
    double fix(const PrimalPoint& t_primals) const;

    /**
     * Computes the resulting numerical by replacing each `Param`  by their corresponding values in `t_duals`.
     * @param t_primals Dual values associated to the stored `Param`
     * @return the resulting numerical
     */
    double fix(const DualPoint& t_duals) const;

    void round();

    Constant& multiply_with_precision(double t_factor, unsigned int t_n_digits);

    Constant& multiply_with_precision_by_power_of_10(unsigned int t_exponent, unsigned int t_n_digits);

    /**
     * Represents an empty constant.
     */
    static Constant Zero;
};

namespace idol {

    static std::ostream &operator<<(std::ostream &t_os, const Constant &t_coefficient) {

        const auto print_lin_term = [&t_os](const idol::Param &t_param, double t_coeff) {
            if (!idol::equals(t_coeff, 1., idol::Tolerance::Sparsity)) {
                t_os << t_coeff << ' ';
            }
            t_os << '!' << t_param.name();
        };

        const auto print_quad_term = [&t_os](const auto &t_pair, double t_coeff) {
            if (!idol::equals(t_coeff, 1., idol::Tolerance::Sparsity)) {
                t_os << t_coeff << ' ';
            }
            t_os << t_pair.first << " !" <<  t_pair.second.name();
        };

        if (t_coefficient.is_zero()) {
            return t_os << "0";
        }

        const double constant = t_coefficient.numerical();

        bool first_term_has_been_printed = false;

        if (!idol::equals(constant, 0., idol::Tolerance::Sparsity)) {
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
