//
// Created by henri on 16/09/22.
//

#ifndef OPTIMIZE_GENERATORS_BASIC_H
#define OPTIMIZE_GENERATORS_BASIC_H

#include "Generator.h"
#include "modeling/models/Model.h"

namespace Generators {
    template<class KeyT> class Basic;
}

template<class KeyT>
class Generators::Basic : public Generator {
    const Model& m_rmp;
    const Model& m_subproblem;
protected:
    Expr<Var> m_constant;
    Map<KeyT, Row> m_values;

    void check_expression(const Expr<Var>& t_expr) const;
public:
    Basic(const Model& t_rmp, const Model& t_subproblem);

    const Model& rmp() const { return m_rmp; }

    const Model& subproblem() const { return m_subproblem; }

    void set(const KeyT& t_ctr, Expr<Var> t_expr, double t_offset = 0.);

    void set_constant(Expr<Var> t_expr);

    const Row& get(const KeyT& t_ctr) const;

    const Expr<Var>& constant() const;

    using const_iterator = typename Map<KeyT, Row>::const_iterator;

    const_iterator begin() const { return m_values.begin(); }
    const_iterator end() const { return m_values.end(); }
    const_iterator cbegin() const { return m_values.begin(); }
    const_iterator cend() const { return m_values.end(); }
};

template<class KeyT>
Generators::Basic<KeyT>::Basic(const Model& t_rmp, const Model& t_subproblem)
    : m_rmp(t_rmp), m_subproblem(t_subproblem) {

}

template<class KeyT>
void Generators::Basic<KeyT>::check_expression(const Expr<Var> &t_expr) const {
    for (const auto& [var, coefficient] : t_expr) {
        if (var.model_id() != m_subproblem.id()) {
            throw Exception("Trying to insert an expression with variables which do not belong to subproblem.");
        }
    }
}
template<class KeyT>
void Generators::Basic<KeyT>::set(const KeyT &t_ctr, Expr<Var> t_expr, double t_offset) {
    if (t_ctr.model_id() != m_rmp.id()) {
        throw Exception("Trying to insert a constraint which does not belong to RMP.");
    }
    check_expression(t_expr);
    auto it = m_values.find(t_ctr);
    if (it == m_values.end()) {
        m_values.emplace(t_ctr, Row(std::move(t_expr), t_offset));
    } else {
        it->second = Row(std::move(t_expr), t_offset);
    }
}

template<class KeyT>
const Row &Generators::Basic<KeyT>::get(const KeyT &t_ctr) const {
    auto it = m_values.find(t_ctr);
    return it == m_values.end() ? Row::EmptyRow : it->second;
}

template<class KeyT>
void Generators::Basic<KeyT>::set_constant(Expr<Var> t_expr) {
    check_expression(t_expr);
    m_constant = std::move(t_expr);
}

template<class KeyT>
const Expr<Var> &Generators::Basic<KeyT>::constant() const {
    return m_constant;
}


#endif //OPTIMIZE_GENERATORS_BASIC_H
