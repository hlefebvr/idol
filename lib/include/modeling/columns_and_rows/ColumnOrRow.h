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

    void set_constant(Coefficient t_constant);

    [[nodiscard]] const Coefficient& constant() const;
};

template<class Key>
ColumnOrRow<Key>::ColumnOrRow(Coefficient t_constant) : m_constant(std::make_unique<MatrixCoefficient>(std::move(t_constant))) {

}

template<class Key>
void ColumnOrRow<Key>::set_constant(Coefficient t_constant) {
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
std::ostream &operator<<(std::ostream& t_os, const ColumnOrRow<Key>& t_column_or_row) {
    t_os << t_column_or_row.constant();
    for (const auto& [key, value] : t_column_or_row) {
        t_os << " + ";
        if (value.is_numerical()) {
            t_os << value;
        } else {
            t_os << '(' << value << ')';
        }
        t_os << " * " << key;
    }
    return t_os;
}

#endif //OPTIMIZE_COLUMNORROW_H
