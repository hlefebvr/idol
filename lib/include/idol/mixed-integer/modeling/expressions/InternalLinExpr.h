//
// Created by henri on 28.10.24.
//

#ifndef IDOL_INTERNALLINEXPR_H
#define IDOL_INTERNALLINEXPR_H

#include "idol/mixed-integer/modeling/expressions/LinExpr.h"

namespace idol {
    class Model;

    template<class>
    class InternalLinExpr;
}

template<class Key>
class idol::InternalLinExpr : public LinExpr<Key> {
    const Model* m_model = nullptr;
protected:
    unsigned int get_raw_index(const Key &t_index) const override {
        return t_index.index(*m_model);
    }
public:
    InternalLinExpr() = default;
    InternalLinExpr(const Key& t_key) : LinExpr<Key>(t_key)  {} // NOLINT(google-explicit-constructor)
    InternalLinExpr(double t_factor, const Key& t_key) : LinExpr<Key>(t_factor, t_key) {}

    InternalLinExpr(const Model* t_model) : m_model(t_model) {}
    InternalLinExpr(const Model* t_model, LinExpr<Key>&& t_other) : LinExpr<Key>(std::move(t_other)), m_model(t_model) {}
    InternalLinExpr(const Model* t_model, const LinExpr<Key>& t_other) : LinExpr<Key>(t_other), m_model(t_model) {}
};

#endif //IDOL_INTERNALLINEXPR_H
