//
// Created by henri on 16/09/22.
//

#ifndef OPTIMIZE_BASEGENERATOR_H
#define OPTIMIZE_BASEGENERATOR_H

#include "AbstractGenerator.h"
#include "containers/Map.h"
#include "modeling/columns_and_rows/Row.h"

template<class KeyT>
class BaseGenerator : public AbstractGenerator {
protected:
    Expr m_constant;
    Map<KeyT, Row> m_values;
public:
    void set(const KeyT& t_ctr, Expr t_expr, double t_offset = 0.);

    void set_constant(Expr t_expr);

    const Row& get(const KeyT& t_ctr) const;

    const Expr& constant() const;

    using const_iterator = typename Map<KeyT, Row>::const_iterator;

    const_iterator begin() const { return m_values.begin(); }
    const_iterator end() const { return m_values.end(); }
    const_iterator cbegin() const { return m_values.begin(); }
    const_iterator cend() const { return m_values.end(); }
};

template<class KeyT>
void BaseGenerator<KeyT>::set(const KeyT &t_ctr, Expr t_expr, double t_offset) {
    auto it = m_values.find(t_ctr);
    if (it == m_values.end()) {
        m_values.emplace(t_ctr, Row(std::move(t_expr), t_offset));
    } else {
        it->second = Row(std::move(t_expr), t_offset);
    }
}

template<class KeyT>
const Row &BaseGenerator<KeyT>::get(const KeyT &t_ctr) const {
    auto it = m_values.find(t_ctr);
    return it == m_values.end() ? Row::EmptyRow : it->second;
}

template<class KeyT>
void BaseGenerator<KeyT>::set_constant(Expr t_expr) {
    m_constant = std::move(t_expr);
}

template<class KeyT>
const Expr &BaseGenerator<KeyT>::constant() const {
    return m_constant;
}

#endif //OPTIMIZE_BASEGENERATOR_H
