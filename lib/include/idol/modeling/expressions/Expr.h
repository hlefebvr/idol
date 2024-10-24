//
// Created by henri on 21/10/22.
//

#ifndef IDOL_EXPR_H
#define IDOL_EXPR_H

#include "LinExpr.h"
#include "QuadExpr.h"

namespace idol {
    namespace impl {
        template<class Key1, class Key2>
        class Expr;
    }

    template<class Key1, class Key2>
    class Expr;
}

template<class Key1, class Key2>
class idol::impl::Expr {
    LinExpr<Key1> m_linear;
    QuadExpr<Key1, Key2> m_quadratic;
    std::unique_ptr<AbstractMatrixCoefficient> m_constant;
protected:
    class References {
        friend class impl::Expr<Key1, Key2>;
        using ParentT = impl::Expr<Key1, Key2>;

        ParentT* m_parent;
        explicit References(ParentT* t_parent) : m_parent(t_parent) {}
    public:
        void set_constant(MatrixCoefficientReference&& t_coefficient) { m_parent->m_constant = std::make_unique<MatrixCoefficientReference>(std::move(t_coefficient)); }
        void reset_constant() { m_parent->m_constant = std::make_unique<MatrixCoefficient>(0.); }
        [[nodiscard]] MatrixCoefficientReference get_constant() const { return MatrixCoefficientReference(*m_parent->m_constant); }
    };

    References refs() { return References(this); }

    void internal_fix(const Solution::Primal& t_primals);
public:
    Expr();
    Expr(double t_num); // NOLINT(google-explicit-constructor)
    Expr(const Param& t_param); // NOLINT(google-explicit-constructor)
    Expr(Constant&& t_expr); // NOLINT(google-explicit-constructor)
    Expr(const Constant& t_expr); // NOLINT(google-explicit-constructor)
    Expr(const Key1& t_var); // NOLINT(google-explicit-constructor)
    Expr(LinExpr<Key1>&& t_expr); // NOLINT(google-explicit-constructor)
    Expr(const LinExpr<Key1>& t_expr); // NOLINT(google-explicit-constructor)
    Expr(QuadExpr<Key1>&& t_expr); // NOLINT(google-explicit-constructor)
    Expr(const QuadExpr<Key1>& t_expr); // NOLINT(google-explicit-constructor)
    Expr(const LinExpr<Key1>& t_lin_expr, const QuadExpr<Key1, Key2>& t_quad_expr, double t_constant);

    virtual ~Expr() = default;

    Expr(const Expr& t_src);
    Expr(Expr&&) noexcept = default;

    Expr& operator=(const Expr& t_rhs);
    Expr& operator=(Expr&&) noexcept = default;

    Expr<Key1, Key2>& operator+=(const Expr<Key1, Key2>& t_rhs);
    Expr<Key1, Key2>& operator-=(const Expr<Key1, Key2>& t_rhs);
    Expr<Key1, Key2>& operator*=(double t_rhs);
    Expr<Key1, Key2>& operator/=(double t_rhs);

    LinExpr<Key1>& linear() { return m_linear; }
    [[nodiscard]] const LinExpr<Key1>& linear() const { return m_linear; }

    QuadExpr<Key1, Key2>& quadratic() { return m_quadratic; }
    const QuadExpr<Key1, Key2>& quadratic() const { return m_quadratic; }

    double& constant() { return m_constant->value(); }

    double constant() const { return m_constant->value(); }

    [[nodiscard]] bool is_zero() const { return std::abs(constant()) < Tolerance::Sparsity && linear().empty() && quadratic().empty(); }

    void round();

    [[nodiscard]] double gcd() const;

    void clear() {
        constant() = 0;
        m_linear.clear();
        m_quadratic.clear();
    }
};

template<class Key1, class Key2>
double idol::impl::Expr<Key1, Key2>::gcd() const {
    return std::gcd((long) m_constant->value(),std::gcd(m_linear.gcd(), m_quadratic.gcd()));
}

template<class Key1, class Key2>
void idol::impl::Expr<Key1, Key2>::round() {
    const double val = m_constant->value();
    m_constant->set_value(std::round(val));
    m_linear.round();
    m_quadratic.round();
}

template<class Key1, class Key2>
idol::impl::Expr<Key1, Key2>::Expr()
        : m_constant(std::make_unique<MatrixCoefficient>(0.)) {

}

template<class Key1, class Key2>
idol::impl::Expr<Key1, Key2>::Expr(double t_num)
        : m_constant(std::make_unique<MatrixCoefficient>(t_num)) {

}

template<class Key1, class Key2>
idol::impl::Expr<Key1, Key2>::Expr(const Param &t_param)
        : m_constant(std::make_unique<MatrixCoefficient>(t_param)) {

}

template<class Key1, class Key2>
idol::impl::Expr<Key1, Key2>::Expr(Constant &&t_expr)
        : m_constant(std::make_unique<MatrixCoefficient>(std::move(t_expr))) {

}

template<class Key1, class Key2>
idol::impl::Expr<Key1, Key2>::Expr(const Constant &t_expr)
        : m_constant(std::make_unique<MatrixCoefficient>(t_expr)) {

}

template<class Key1, class Key2>
idol::impl::Expr<Key1, Key2>::Expr(const Key1 &t_var)
        : m_linear(t_var),
          m_constant(std::make_unique<MatrixCoefficient>(0.)) {

}

template<class Key1, class Key2>
idol::impl::Expr<Key1, Key2>::Expr(LinExpr<Key1> &&t_expr)
        : m_linear(std::move(t_expr)),
          m_constant(std::make_unique<MatrixCoefficient>(0.)) {

}

template<class Key1, class Key2>
idol::impl::Expr<Key1, Key2>::Expr(const LinExpr<Key1> &t_expr)
        : m_linear(t_expr),
          m_constant(std::make_unique<MatrixCoefficient>(0.)) {

}

template<class Key1, class Key2>
idol::impl::Expr<Key1, Key2>::Expr(QuadExpr<Key1> &&t_expr)
        : m_quadratic(std::move(t_expr)),
          m_constant(std::make_unique<MatrixCoefficient>(0.)) {

}

template<class Key1, class Key2>
idol::impl::Expr<Key1, Key2>::Expr(const QuadExpr<Key1> &t_expr)
        : m_quadratic(t_expr),
          m_constant(std::make_unique<MatrixCoefficient>(0.)) {

}

template<class Key1, class Key2>
idol::impl::Expr<Key1, Key2>::Expr(const LinExpr<Key1> &t_lin_expr, const QuadExpr<Key1, Key2> &t_quad_expr, double t_constant)
        : m_linear(t_lin_expr),
          m_quadratic(t_quad_expr),
          m_constant(std::make_unique<MatrixCoefficient>(t_constant)) {

}

template<class Key1, class Key2>
idol::impl::Expr<Key1, Key2>::Expr(const Expr &t_src)
        : m_linear(t_src.m_linear),
          m_quadratic(t_src.m_quadratic),
          m_constant(std::make_unique<MatrixCoefficient>(t_src.m_constant->value()))  {

}

template<class Key1, class Key2>
idol::impl::Expr<Key1, Key2> &idol::impl::Expr<Key1, Key2>::operator=(const Expr &t_rhs) {
    if (this == &t_rhs) { return *this; }
    m_linear = t_rhs.m_linear;
    m_quadratic = t_rhs.m_quadratic;
    m_constant->value() = t_rhs.m_constant->value();
    return *this;
}

template<class Key1, class Key2>
idol::impl::Expr<Key1, Key2> &idol::impl::Expr<Key1, Key2>::operator+=(const impl::Expr<Key1, Key2> &t_rhs) {
    m_linear += t_rhs.m_linear;
    m_quadratic += t_rhs.m_quadratic;
    m_constant->value() += t_rhs.m_constant->value();
    return *this;
}

template<class Key1, class Key2>
idol::impl::Expr<Key1, Key2> &idol::impl::Expr<Key1, Key2>::operator-=(const impl::Expr<Key1, Key2> &t_rhs) {
    m_linear -= t_rhs.m_linear;
    m_quadratic -= t_rhs.m_quadratic;
    m_constant->value() -= t_rhs.m_constant->value();
    return *this;
}

template<class Key1, class Key2>
idol::impl::Expr<Key1, Key2> &idol::impl::Expr<Key1, Key2>::operator*=(double t_rhs) {
    m_linear *= t_rhs;
    m_quadratic *= t_rhs;
    m_constant->value() *= t_rhs;
    return *this;
}

template<class Key1, class Key2>
idol::impl::Expr<Key1, Key2> &idol::impl::Expr<Key1, Key2>::operator/=(double t_rhs) {
    m_linear /= t_rhs;
    m_quadratic /= t_rhs;
    m_constant->value() /= t_rhs;
    return *this;
}

template<class Key1 = idol::Var, class Key2 = Key1>
class idol::Expr : public impl::Expr<Key1, Key2> {
    friend class Matrix;
public:
    Expr() = default;
    Expr(double t_num) : impl::Expr<Key1, Key2>(t_num) {} // NOLINT(google-explicit-constructor)
    Expr(const Param& t_param) : impl::Expr<Key1, Key2>(t_param) {} // NOLINT(google-explicit-constructor)
    Expr(Constant&& t_expr) : impl::Expr<Key1, Key2>(std::move(t_expr)) {} // NOLINT(google-explicit-constructor)
    Expr(const Constant& t_expr) : impl::Expr<Key1, Key2>(t_expr) {} // NOLINT(google-explicit-constructor)
    Expr(const Key1& t_var) : impl::Expr<Key1, Key2>(t_var) {} // NOLINT(google-explicit-constructor)
    Expr(LinExpr<Key1>&& t_expr) : impl::Expr<Key1, Key2>(std::move(t_expr)) {} // NOLINT(google-explicit-constructor)
    Expr(const LinExpr<Key1>& t_expr) : impl::Expr<Key1, Key2>(t_expr) {} // NOLINT(google-explicit-constructor)
    Expr(QuadExpr<Key1>&& t_expr) : impl::Expr<Key1, Key2>(std::move(t_expr)) {} // NOLINT(google-explicit-constructor)
    Expr(const QuadExpr<Key1>& t_expr) : impl::Expr<Key1, Key2>(t_expr) {} // NOLINT(google-explicit-constructor)
    Expr(const LinExpr<Key1>& t_lin_expr, const QuadExpr<Key1, Key2>& t_quad_expr, double t_constant) : impl::Expr<Key1, Key2>(t_lin_expr, t_quad_expr, t_constant) {}

    Expr(const Expr& t_src) = default;
    Expr(Expr&&) noexcept = default;

    Expr& operator=(const Expr& t_rhs) = default;
    Expr& operator=(Expr&&) noexcept = default;

    Expr fix(const Solution::Primal& t_primals) const;
};

template<class Key1, class Key2>
idol::Expr<Key1, Key2> idol::Expr<Key1, Key2>::fix(const Solution::Primal& t_primals) const {
    auto result = *this;
    result.internal_fix(t_primals);
    return result;
}

namespace idol {

    template<class Key1, class Key2>
    std::ostream &operator<<(std::ostream &t_os, const idol::Expr<Key1, Key2> &t_expr) {

        if (std::abs(t_expr.constant()) < Tolerance::Sparsity) {

            if (t_expr.linear().empty()) {
                return t_os << t_expr.quadratic();
            }

            t_os << t_expr.linear();

            if (!t_expr.quadratic().empty()) {
                return t_os << " + " << t_expr.quadratic();
            }

            return t_os;
        }

        t_os << t_expr.constant();

        if (!t_expr.linear().empty()) {
            t_os << " + " << t_expr.linear();
        }

        if (!t_expr.quadratic().empty()) {
            t_os << " + " << t_expr.quadratic();
        }

        return t_os;
    }

}

#endif //IDOL_EXPR_H
