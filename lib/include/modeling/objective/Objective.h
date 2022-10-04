//
// Created by henri on 09/09/22.
//

#ifndef OPTIMIZE_OBJECTIVE_H
#define OPTIMIZE_OBJECTIVE_H

#include <vector>
#include "modeling/variables/Variable.h"
#include "modeling/columns_and_rows/Deprecated_AbstractExpr.h"
#include "modeling/columns_and_rows/Deprecated_Column.h"

class Model;
class Var;
class Constant;

class Objective {
    friend class Model;
    const Constant* m_offset;
    const std::vector<Var>* m_variables;
    explicit Objective(const std::vector<Var>& t_variables, const Constant& t_offset) : m_variables(&t_variables), m_offset(&t_offset) {}
public:
    Objective() = delete;

    Objective(const Objective&) = default;
    Objective(Objective&&) noexcept = default;

    Objective& operator=(const Objective&) = default;
    Objective& operator=(Objective&&) noexcept = default;

    const Constant& offset() const { return *m_offset; }

    [[nodiscard]] const Constant& get(const Var& t_var) const { return t_var.column().constant(); } // NOLINT(readability-convert-member-functions-to-static)

    class const_iterator {
        std::vector<Var>::const_iterator m_it;
    public:
        explicit const_iterator(const std::vector<Var>::const_iterator&& t_it) : m_it(t_it) {}
        bool operator!=(const const_iterator& t_rhs) const { return m_it != t_rhs.m_it; }
        bool operator==(const const_iterator& t_rhs) const { return m_it == t_rhs.m_it; }
        const_iterator& operator++() { ++m_it; return *this; }
        Deprecated_AbstractExpr<Var>::Entry operator*() const { return Deprecated_AbstractExpr<Var>::Entry(*m_it, m_it->column().constant()); }
    };

    [[nodiscard]] const_iterator begin() const { return const_iterator(m_variables->cbegin()); }
    [[nodiscard]] const_iterator end() const { return const_iterator(m_variables->cend()); }
    [[nodiscard]] const_iterator cbegin() const { return const_iterator(m_variables->cbegin()); }
    [[nodiscard]] const_iterator cend() const { return const_iterator(m_variables->cend()); }
};

static std::ostream &operator<<(std::ostream& t_os, const Objective& t_obj) {

    const auto print_term = [&t_os](const Var& t_key, const Constant& t_coeff) {
        if (t_coeff.is_numerical()) {
            if (!equals(t_coeff.constant(), 1., ToleranceForSparsity)) {
                t_os << t_coeff << ' ';
            }
        } else if(equals(t_coeff.constant(), 0., ToleranceForSparsity) && t_coeff.size() == 1) {
            t_os << t_coeff << ' ';
        } else {
            t_os << '(' << t_coeff << ") ";
        }
        t_os << t_key;
    };

    auto it = t_obj.begin();
    const auto end = t_obj.end();

    if (it == end) {
        return t_os << '0';
    }

    print_term((*it).first, (*it).second);

    for (++it ; it != end ; ++it) {
        t_os << " + ";
        print_term((*it).first, (*it).second);
    }

    return t_os;
}

#endif //OPTIMIZE_OBJECTIVE_H
