//
// Created by henri on 12/09/22.
//

#ifndef OPTIMIZE_ABSTRACTSOLUTION_H
#define OPTIMIZE_ABSTRACTSOLUTION_H

#include "containers/Map.h"
#include "containers/IteratorForward.h"
#include "solvers/Types.h"
#include "modeling/numericals.h"

template<class KeyT, class CRTP>
class AbstractSolution {
    SolutionStatus m_status = Unknown;
    double m_objective_value = Inf;
    Map<KeyT, double> m_values;

    double norm_inf() const;
public:
    AbstractSolution() = default;

    virtual ~AbstractSolution() = default;

    AbstractSolution(const AbstractSolution&) = default;
    AbstractSolution(AbstractSolution&&) noexcept = default;

    AbstractSolution& operator=(const AbstractSolution&) = default;
    AbstractSolution& operator=(AbstractSolution&&) noexcept = default;

    void set_status(SolutionStatus t_status) { m_status = t_status; }

    [[nodiscard]] SolutionStatus status() const { return m_status; }

    void set_objective_value(double t_value) { m_objective_value = t_value; }

    [[nodiscard]] double objective_value() const { return m_objective_value; }

    void set(const KeyT& t_key, double t_value);

    [[nodiscard]] double get(const KeyT& t_key) const;

    using const_iterator = typename Map<KeyT, double>::const_iterator;

    const_iterator begin() const { return const_iterator(m_values.begin()); }
    const_iterator end() const { return const_iterator(m_values.end()); }
    const_iterator cbegin() const { return const_iterator(m_values.begin()); }
    const_iterator cend() const { return const_iterator(m_values.end()); }

    [[nodiscard]] double norm(double t_p = 2.) const;

    CRTP& normalize(double t_p = 2.);
};

template<class KeyT, class CRTP>
void AbstractSolution<KeyT, CRTP>::set(const KeyT &t_key, double t_value) {

    if (equals(t_value, 0., ToleranceForSparsity)) {
        m_values.erase(t_key);
        return;
    }

    auto [it, success] = m_values.template emplace(t_key, t_value);
    if (!success) {
        it->second = t_value;
    }

}

template<class KeyT, class CRTP>
double AbstractSolution<KeyT, CRTP>::get(const KeyT &t_key) const {
    auto it = m_values.find(t_key);
    return it == m_values.end() ? 0. : it->second;
}

template<class KeyT, class CRTP>
CRTP& AbstractSolution<KeyT, CRTP>::normalize(double t_p) {

    const double scale = norm(t_p);

    m_objective_value /= scale;

    for (auto& [key, value] : m_values) {
        value /= scale;
    }

    return dynamic_cast<CRTP&>(*this);
}

template<class KeyT, class CRTP>
double AbstractSolution<KeyT, CRTP>::norm(double t_p) const {

    if (is_pos_inf(t_p)) {
        return norm_inf();
    }

    double result = 0.;
    for (const auto& [key, value] : m_values) {
        result += std::pow(value, t_p);
    }
    result = std::pow(result, 1 / t_p);
    return result;
}

template<class KeyT, class CRTP>
double AbstractSolution<KeyT, CRTP>::norm_inf() const {
    double result = -Inf;
    for (const auto& [key, value] : m_values) {
        if (result < value) {
            result = value;
        }
    }
    return result;
}

template<class KeyT, class CRTP>
static std::ostream& operator<<(std::ostream& t_os, const AbstractSolution<KeyT, CRTP>& t_solution) {
    t_os << "SOLUTION STATUS: " << t_solution.status() << '\n';
    t_os << "OBJECTIVE VALUE: " << t_solution.objective_value() << '\n';
    t_os << "STORED VALUES:" << '\n';
    for (const auto& [key, value] : t_solution) {
        t_os << '\t' << key.name() << " = " << value << '\n';
    }
    return t_os;
}

#endif //OPTIMIZE_ABSTRACTSOLUTION_H
