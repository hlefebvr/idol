//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_COLUMNORROW_H
#define OPTIMIZE_COLUMNORROW_H

#include "AbstractExpr.h"

namespace impl {
    template<class Key> class ColumnOrRowReference;
}

template<class Key>
class ColumnOrRow : public AbstractExpr<Key> {
    friend class impl::ColumnOrRowReference<Key>;
    std::unique_ptr<AbstractMatrixCoefficient> m_constant;
public:
    ColumnOrRow() = default;
    explicit ColumnOrRow(Coefficient t_constant);
    ColumnOrRow(AbstractExpr<Key> t_expr, Coefficient t_constant);

    ColumnOrRow(const ColumnOrRow& t_src);
    ColumnOrRow(ColumnOrRow&& t_src) noexcept = default;

    ColumnOrRow& operator=(const ColumnOrRow& t_src);
    ColumnOrRow& operator=(ColumnOrRow&& t_src) noexcept = default;

    virtual ColumnOrRow<Key>& operator*=(double t_factor);
    ColumnOrRow<Key>& operator+=(const ColumnOrRow<Key>& t_expr);

    void set_constant(Coefficient t_constant);

    [[nodiscard]] const Coefficient& constant() const;
};

template<class Key>
ColumnOrRow<Key>::ColumnOrRow(Coefficient t_constant) : m_constant(std::make_unique<MatrixCoefficient>(std::move(t_constant))) {

}

template<class Key>
void ColumnOrRow<Key>::set_constant(Coefficient t_constant) {

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
const Coefficient &ColumnOrRow<Key>::constant() const {
    return m_constant ? m_constant->value() : Coefficient::Zero;
}

template<class Key>
ColumnOrRow<Key>::ColumnOrRow(const ColumnOrRow &t_src)
    : AbstractExpr<Key>(t_src),
    m_constant(t_src.m_constant ? std::make_unique<MatrixCoefficient>(t_src.m_constant->value()) : std::unique_ptr<MatrixCoefficient>()) {

}

template<class Key>
ColumnOrRow<Key> &ColumnOrRow<Key>::operator=(const ColumnOrRow &t_src) {
    if (this == &t_src) { return *this; }
    if (m_constant) {
        *m_constant = t_src.m_constant;
    } else {
        m_constant = std::make_unique<MatrixCoefficient>(*t_src.m_constant);
    }
    return *this;
}

template<class Key>
ColumnOrRow<Key>::ColumnOrRow(AbstractExpr<Key> t_expr, Coefficient t_constant)
    : AbstractExpr<Key>(std::move(t_expr)),
      m_constant(std::make_unique<MatrixCoefficient>(std::move(t_constant))) {

}

template<class Key>
ColumnOrRow<Key> &ColumnOrRow<Key>::operator*=(double t_factor) {
    AbstractExpr<Key>::operator*=(t_factor);
    if (m_constant) {
        *m_constant *= t_factor;
    }
    return *this;
}

template<class Key>
ColumnOrRow<Key> &ColumnOrRow<Key>::operator+=(const ColumnOrRow<Key> &t_expr) {
    AbstractExpr<Key>::operator+=(t_expr);
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
std::ostream& operator<<(std::ostream& t_os, const ColumnOrRow<Key>& t_col_or_row) {
    if (!t_col_or_row.constant().is_zero()) {
        t_os << t_col_or_row.constant() << " + ";
    }
    return t_os << (AbstractExpr<Key>&) t_col_or_row;
}

#endif //OPTIMIZE_COLUMNORROW_H
