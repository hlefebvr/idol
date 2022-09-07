//
// Created by henri on 05/09/22.
//

#ifndef OPTIMIZE_IMPL_LINEXPR_H
#define OPTIMIZE_IMPL_LINEXPR_H

#include "Variable.h"
#include "../containers/Map.h"

namespace impl {
    template<enum Player PlayerT> class LinExpr;
}

template<enum Player PlayerT = Decision>
class impl::LinExpr {
    Map<::Variable<PlayerT>, double> m_terms;
    double m_constant = 0.;
public:
    LinExpr()= default;
    LinExpr(double t_offset); // NOLINT(google-explicit-constructor)
    LinExpr(const ::Variable<PlayerT>& t_var); // NOLINT(google-explicit-constructor)
    explicit LinExpr(Map<::Variable<PlayerT>, double>&& t_map, double t_constant);

    LinExpr(const LinExpr&) = default;
    LinExpr(LinExpr&&) noexcept = default;

    LinExpr<PlayerT>& operator=(const LinExpr<PlayerT>&) = default;
    LinExpr<PlayerT>& operator=(LinExpr<PlayerT>&&) noexcept = default;

    using iterator = typename Map<::Variable<PlayerT>, double>::iterator;
    using const_iterator = typename Map<::Variable<PlayerT>, double>::const_iterator;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

    [[nodiscard]] double constant() const;

    [[nodiscard]] unsigned int n_terms() const;

    [[nodiscard]] bool is_empty() const;

    [[nodiscard]] bool is_numerical() const;

    [[nodiscard]] double operator[](const ::Variable<PlayerT>& t_variable) const;

    void set_coefficient(const ::Variable<PlayerT>& t_var, double t_coeff);

    void set_constant(double t_constant);

    LinExpr<PlayerT>& operator*=(double t_rhs);
    LinExpr<PlayerT>& operator+=(double t_rhs);
    LinExpr<PlayerT>& operator+=(const ::Variable<PlayerT>& t_rhs);
    LinExpr<PlayerT>& operator+=(const LinExpr<PlayerT>& t_rhs);
};

template class impl::LinExpr<Decision>;
template class impl::LinExpr<Parameter>;

#endif //OPTIMIZE_IMPL_LINEXPR_H
