//
// Created by henri on 16/09/22.
//

#ifndef OPTIMIZE_BASEGENERATOR_H
#define OPTIMIZE_BASEGENERATOR_H

#include "AbstractGenerator.h"
#include "modeling/models/Model.h"

template<class KeyT>
class BaseGenerator : public AbstractGenerator {
    const Model& m_rmp;
    const Model& m_subproblem;
protected:
    Deprecated_Expr m_constant;
    Map<KeyT, Deprecated_Row> m_values;

    void check_expression(const Deprecated_Expr& t_expr) const;
public:
    BaseGenerator(const Model& t_rmp, const Model& t_subproblem);

    const Model& rmp() const { return m_rmp; }

    const Model& subproblem() const { return m_subproblem; }

    void set(const KeyT& t_ctr, Deprecated_Expr t_expr, double t_offset = 0.);

    void set_constant(Deprecated_Expr t_expr);

    const Deprecated_Row& get(const KeyT& t_ctr) const;

    const Deprecated_Expr& constant() const;

    using const_iterator = typename Map<KeyT, Deprecated_Row>::const_iterator;

    const_iterator begin() const { return m_values.begin(); }
    const_iterator end() const { return m_values.end(); }
    const_iterator cbegin() const { return m_values.begin(); }
    const_iterator cend() const { return m_values.end(); }
};

template<class KeyT>
BaseGenerator<KeyT>::BaseGenerator(const Model& t_rmp, const Model& t_subproblem)
    : m_rmp(t_rmp), m_subproblem(t_subproblem) {

}

template<class KeyT>
void BaseGenerator<KeyT>::check_expression(const Deprecated_Expr &t_expr) const {
    for (const auto& [var, coefficient] : t_expr) {
        if (var.model_id() != m_subproblem.id()) {
            throw Exception("Trying to insert an expression with variables which do not belong to subproblem.");
        }
    }
}
template<class KeyT>
void BaseGenerator<KeyT>::set(const KeyT &t_ctr, Deprecated_Expr t_expr, double t_offset) {
    if (t_ctr.model_id() != m_rmp.id()) {
        throw Exception("Trying to insert a constraint which does not belong to RMP.");
    }
    check_expression(t_expr);
    auto it = m_values.find(t_ctr);
    if (it == m_values.end()) {
        m_values.emplace(t_ctr, Deprecated_Row(std::move(t_expr), t_offset));
    } else {
        it->second = Deprecated_Row(std::move(t_expr), t_offset);
    }
}

template<class KeyT>
const Deprecated_Row &BaseGenerator<KeyT>::get(const KeyT &t_ctr) const {
    auto it = m_values.find(t_ctr);
    return it == m_values.end() ? Deprecated_Row::EmptyRow : it->second;
}

template<class KeyT>
void BaseGenerator<KeyT>::set_constant(Deprecated_Expr t_expr) {
    check_expression(t_expr);
    m_constant = std::move(t_expr);
}

template<class KeyT>
const Deprecated_Expr &BaseGenerator<KeyT>::constant() const {
    return m_constant;
}


#endif //OPTIMIZE_BASEGENERATOR_H
