//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_DEPRECATED_COLUMNORROW_H
#define OPTIMIZE_DEPRECATED_COLUMNORROW_H

#include "Deprecated_AbstractExpr.h"

namespace impl {
    template<class Key> class Deprecated_ColumnOrRowReference;
}

template<class Key>
class Deprecated_ColumnOrRow : public Deprecated_AbstractExpr<Key> {
    friend class impl::Deprecated_ColumnOrRowReference<Key>;
    std::unique_ptr<AbstractMatrixCoefficient> m_constant;
public:
    Deprecated_ColumnOrRow() = default;
    explicit Deprecated_ColumnOrRow(Constant t_constant);
    Deprecated_ColumnOrRow(Deprecated_AbstractExpr<Key> t_expr, Constant t_constant);

    Deprecated_ColumnOrRow(const Deprecated_ColumnOrRow& t_src);
    Deprecated_ColumnOrRow(Deprecated_ColumnOrRow&& t_src) noexcept = default;

    Deprecated_ColumnOrRow& operator=(const Deprecated_ColumnOrRow& t_src);
    Deprecated_ColumnOrRow& operator=(Deprecated_ColumnOrRow&& t_src) noexcept = default;

    virtual Deprecated_ColumnOrRow<Key>& operator*=(double t_factor);
    Deprecated_ColumnOrRow<Key>& operator+=(const Deprecated_ColumnOrRow<Key>& t_expr);

    void set_constant(Constant t_constant);

    [[nodiscard]] const Constant& constant() const;
};

template<class Key>
Deprecated_ColumnOrRow<Key>::Deprecated_ColumnOrRow(Constant t_constant) : m_constant(std::make_unique<MatrixCoefficient>(std::move(t_constant))) {

}

template<class Key>
void Deprecated_ColumnOrRow<Key>::set_constant(Constant t_constant) {

    if (t_constant.is_zero()) {
        m_constant.reset(nullptr);
        return;
    }

    if (!m_constant) {
        m_constant = std::make_unique<MatrixCoefficient>(std::move(t_constant));
    } else {
        m_constant->set_value(std::move(t_constant));
    }
}

template<class Key>
const Constant &Deprecated_ColumnOrRow<Key>::constant() const {
    return m_constant ? m_constant->value() : Constant::Zero;
}

template<class Key>
Deprecated_ColumnOrRow<Key>::Deprecated_ColumnOrRow(const Deprecated_ColumnOrRow &t_src)
    : Deprecated_AbstractExpr<Key>(t_src),
    m_constant(t_src.m_constant ? std::make_unique<MatrixCoefficient>(t_src.m_constant->value()) : std::unique_ptr<MatrixCoefficient>()) {

}

template<class Key>
Deprecated_ColumnOrRow<Key> &Deprecated_ColumnOrRow<Key>::operator=(const Deprecated_ColumnOrRow &t_src) {
    if (this == &t_src) { return *this; }
    if (m_constant) {
        *m_constant = t_src.m_constant;
    } else {
        m_constant = std::make_unique<MatrixCoefficient>(*t_src.m_constant);
    }
    return *this;
}

template<class Key>
Deprecated_ColumnOrRow<Key>::Deprecated_ColumnOrRow(Deprecated_AbstractExpr<Key> t_expr, Constant t_constant)
    : Deprecated_AbstractExpr<Key>(std::move(t_expr)),
      m_constant(std::make_unique<MatrixCoefficient>(std::move(t_constant))) {

}

template<class Key>
Deprecated_ColumnOrRow<Key> &Deprecated_ColumnOrRow<Key>::operator*=(double t_factor) {
    Deprecated_AbstractExpr<Key>::operator*=(t_factor);
    if (m_constant) {
        *m_constant *= t_factor;
    }
    return *this;
}

template<class Key>
Deprecated_ColumnOrRow<Key> &Deprecated_ColumnOrRow<Key>::operator+=(const Deprecated_ColumnOrRow<Key> &t_expr) {
    Deprecated_AbstractExpr<Key>::operator+=(t_expr);
    if (t_expr.m_constant) {
        if (!m_constant) {
            set_constant(t_expr.m_constant->value());
        } else {
            *m_constant += *t_expr.m_constant;
        }
    }
    return *this;
}

template<class Key>
std::ostream& operator<<(std::ostream& t_os, const Deprecated_ColumnOrRow<Key>& t_col_or_row) {
    if (!t_col_or_row.constant().is_zero()) {
        t_os << t_col_or_row.constant() << " + ";
    }
    return t_os << (Deprecated_AbstractExpr<Key>&) t_col_or_row;
}

#endif //OPTIMIZE_DEPRECATED_COLUMNORROW_H
