//
// Created by henri on 12/09/22.
//

#ifndef OPTIMIZE_ABSTRACTSOLUTION_H
#define OPTIMIZE_ABSTRACTSOLUTION_H

#include "idol/containers/Map.h"
#include "idol/containers/IteratorForward.h"
#include "types.h"
#include "idol/modeling/numericals.h"
#include "idol/optimizers/logs.h"
#include <optional>

namespace idol {
    template<class KeyT, class CRTP>
    class AbstractSolution;
}

/**
 * Base class for Solution::Primal and Solution::Dual. Stores a sparse vector of values associated to a given key.
 * @tparam KeyT The type of the key (typically Var or Ctr)
 * @tparam CRTP See [CRTP](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern).
 */
template<class KeyT, class CRTP>
class idol::AbstractSolution {
    SolutionStatus m_status = Loaded;
    SolutionReason m_reason = NotSpecified;
    std::optional<double> m_objective_value;
    Map<KeyT, double> m_values;

    [[nodiscard]] double norm_inf() const;
public:
    /**
     * Default constructor.
     *
     * Creates a new empty solution. The status is set to `Loaded` and the reason to `NotSpecified`.
     * No objective value nor solution entries is originally set.
     */
    AbstractSolution() = default;

    virtual ~AbstractSolution() = default;

    AbstractSolution(const AbstractSolution&) = default;
    AbstractSolution(AbstractSolution&&) noexcept = default;

    AbstractSolution& operator=(const AbstractSolution&) = default;
    AbstractSolution& operator=(AbstractSolution&&) noexcept = default;

    /**
     * Sets the solution status.
     * @param t_status The desired solution status.
     */
    void set_status(SolutionStatus t_status) { m_status = t_status; }

    /**
     * Returns the stored solution status.
     */
    [[nodiscard]] SolutionStatus status() const { return m_status; }

    /**
     * Sets the reason for the solution status.
     *
     * **Example**:
     * ```cpp
     * solution.set_status(Feasible);
     * solution.set_reason(IterLimit); // We only found a feasible solution because of iteration limit reached
     * ```
     * @param t_reason The desired reason.
     */
    void set_reason(SolutionReason t_reason) { m_reason = t_reason; }

    /**
     * Returns the reason for the solution status.
     * @return The reason for the solution status.
     */
    [[nodiscard]] SolutionReason reason() const { return m_reason; }

    /**
     * Sets the objective value of the solution.
     * @param t_value The desired objective value.
     */
    void set_objective_value(double t_value) { m_objective_value = t_value; }

    /**
     * Returns the objective value of the solution.
     */
    [[nodiscard]] double objective_value() const { return m_objective_value.value(); }

    /**
     * Returns true if the solution has an objective value, false otherwise.
     * @return True if the solution has an objective value, false otherwise.
     */
    [[nodiscard]] bool has_objective_value() const { return m_objective_value.has_value(); }

    /**
     * Resets the stored objective value.
     *
     * Trying to access the objective value after calling this method will throw an
     * exception.
     */
    void reset_objective_value() { m_objective_value.reset(); }

    /**
     * Sets the value associated to the object `t_key` given as argument.
     * @param t_key The key for which the entry should be set.
     * @param t_value The desired value associated to the key.
     */
    void set(const KeyT& t_key, double t_value);

    /**
     * Returns the value associated to the object `t_key` given as argument.
     *
     * If no value is stored, zero is returned.
     * @param t_key  The queried key.
     */
    [[nodiscard]] double get(const KeyT& t_key) const;

    /**
     * Returns the number of non-zero entries in the solution.
     * @return The number of non-zero entries in the solution
     */
    [[nodiscard]] unsigned int size() const { return m_values.size(); }

    using const_iterator = typename Map<KeyT, double>::const_iterator;

    /**
     * Returns an iterator on the values stored in the solution.
     * @return An iterator on the values stored in the solution.
     */
    const_iterator begin() const { return const_iterator(m_values.begin()); }

    /**
     * Returns an iterator on the values stored in the solution.
     * @return An iterator on the values stored in the solution.
     */
    const_iterator end() const { return const_iterator(m_values.end()); }

    /**
     * Returns an iterator on the values stored in the solution.
     * @return An iterator on the values stored in the solution.
     */
    const_iterator cbegin() const { return const_iterator(m_values.begin()); }

    /**
     * Returns an iterator on the values stored in the solution.
     * @return An iterator on the values stored in the solution.
     */
    const_iterator cend() const { return const_iterator(m_values.end()); }

    /**
     * Returns the \f$ l_p \f$-norm of the solution.
     *
     * Note that `Inf` is a possible value for `t_p`, in which case, the infinity norm is computed.
     * @param t_p The \f$ p \f$ parameter in the \f$ l_p \f$-norm.
     */
    [[nodiscard]] double norm(double t_p = 2.) const;

    /**
     * Merges the solution with another solution, explicitly requiring that no conflict arises (i.e., that no
     * entry from the solution `t_rhs` is already stored in the solution).
     * If a conflict is detected, an exception is thrown.
     * @param t_rhs The solution to merge.
     * @return *this
     */
    CRTP& merge_without_conflict(CRTP t_rhs);

    /**
     * Normalizes the solution (i.e., divides every entry by the norm of the solution) with respect to a given \f$ l_p \f$-norm.
     * @param t_p The parameter \f$ p \f$ for the norm.
     * @return *this
     */
    CRTP& normalize(double t_p = 2.);

    /**
     * Rounds all the entries of the solution to the closest `double` with `t_n_digits`.
     *
     * Using `t_n_digits = 0` leads to the usual rounding, i.e., closest integer.
     * @param t_n_digits The number of digits for the closest `double`.
     * @return *this
     */
    CRTP& round(unsigned int t_n_digits = 0);

    /**
     * Adds the solution to another solution `t_rhs`.
     * @param t_rhs The solution to add.
     * @return *this
     */
    CRTP& operator +=(const CRTP& t_rhs);

    /**
     * Multiplies every entries in the solution by `t_factor`.
     * @param t_factor The factor for multiplication.
     * @return *this
     */
    CRTP& operator *=(double t_factor);
};

template<class KeyT, class CRTP>
void idol::AbstractSolution<KeyT, CRTP>::set(const KeyT &t_key, double t_value) {

    if (equals(t_value, 0., Tolerance::Sparsity)) {
        m_values.erase(t_key);
        return;
    }

    auto [it, success] = m_values.template emplace(t_key, t_value);
    if (!success) {
        it->second = t_value;
    }

}

template<class KeyT, class CRTP>
double idol::AbstractSolution<KeyT, CRTP>::get(const KeyT &t_key) const {
    auto it = m_values.find(t_key);
    return it == m_values.end() ? 0. : it->second;
}

template<class KeyT, class CRTP>
CRTP& idol::AbstractSolution<KeyT, CRTP>::normalize(double t_p) {

    const double scale = norm(t_p);

    if (m_objective_value.has_value()) {
        m_objective_value.value() /= scale;
    }

    for (auto& [key, value] : m_values) {
        value /= scale;
    }

    return dynamic_cast<CRTP&>(*this);
}

template<class KeyT, class CRTP>
CRTP& idol::AbstractSolution<KeyT, CRTP>::round(unsigned int t_n_digits) {

    for (auto& [key, value] : m_values) {
        value = ::idol::round(value, t_n_digits);
    }

    return dynamic_cast<CRTP&>(*this);
}

template<class KeyT, class CRTP>
double idol::AbstractSolution<KeyT, CRTP>::norm(double t_p) const {

    if (is_pos_inf(t_p)) {
        return norm_inf();
    }

    double result = 0.;
    for (const auto& [key, value] : m_values) {
        result += std::pow(std::abs(value), t_p);
    }
    result = std::pow(result, 1 / t_p);
    return result;
}

template<class KeyT, class CRTP>
double idol::AbstractSolution<KeyT, CRTP>::norm_inf() const {
    double result = -Inf;
    for (const auto& [key, value] : m_values) {
        if (double abs = std::abs(value) ; result < abs) {
            result = abs;
        }
    }
    return result;
}

template<class KeyT, class CRTP>
CRTP &idol::AbstractSolution<KeyT, CRTP>::merge_without_conflict(CRTP t_rhs) {
    for (const auto& [key, value] : t_rhs.m_values) {
        auto [it, success] = m_values.template emplace(key, value);
        if (!success) {
            throw Exception("Conflicts were found while trying to merge explicitly \"without conflict\".");
        }
    }
    /*
    m_values.template merge(t_rhs.m_values);
    if (!t_rhs.m_values.empty()) {
        throw Exception("Conflicts were found while trying to merge explicitly \"without conflict\".");
    }
    */
    return dynamic_cast<CRTP&>(*this);
}

template<class KeyT, class CRTP>
CRTP &idol::AbstractSolution<KeyT, CRTP>::operator+=(const CRTP &t_rhs) {
    for (const auto& [key, value] : t_rhs) {
        auto [it, success] = m_values.template emplace(key, value);
        if (!success) {
            it->second += value;
            if (equals(it->second, 0., Tolerance::Sparsity)) {
                m_values.erase(it);
            }
        }
    }
    return dynamic_cast<CRTP&>(*this);
}

template<class KeyT, class CRTP>
CRTP &idol::AbstractSolution<KeyT, CRTP>::operator*=(double t_factor) {

    if (equals(t_factor, 0., Tolerance::Sparsity)) {
        m_values.clear();
        return dynamic_cast<CRTP&>(*this);
    }

    for (auto& [key, value] : m_values) {
        value *= t_factor;
    }

    return dynamic_cast<CRTP&>(*this);
}

namespace idol {

    template<class KeyT, class CRTP>
    static std::ostream &operator<<(std::ostream &t_os, const idol::AbstractSolution<KeyT, CRTP> &t_solution) {
        t_os << "+-----------------------\n";
        t_os << "| Status: " << t_solution.status() << '\n';
        t_os << "| Reason: " << t_solution.reason() << '\n';
        if (t_solution.has_objective_value()) {
            t_os << "| Objective: " << t_solution.objective_value() << '\n';
        }
        t_os << "| Values:" << '\n';
        for (const auto &[key, value]: t_solution) {
            t_os << "| \t" << std::setw(8) << key.name() << " = " << pretty_double(value) << '\n';
        }
        t_os << "+-----------------------\n";
        return t_os;
    }

}

#endif //OPTIMIZE_ABSTRACTSOLUTION_H
