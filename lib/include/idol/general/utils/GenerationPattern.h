//
// Created by henri on 20.11.24.
//

#ifndef IDOL_GENERATIONPATTERN_H
#define IDOL_GENERATIONPATTERN_H

#include "idol/mixed-integer/modeling/expressions/AffExpr.h"
#include "Point.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

namespace idol {
    template<class, class> class GenerationPattern;
}

template<class GeneratedT,
         class CoefficientT = idol::Var
         >
class idol::GenerationPattern {
public:
    using KeyT = std::conditional_t<std::is_same_v<GeneratedT, Var>, Ctr, Var>;

    GenerationPattern() = default;

    GenerationPattern(idol::AffExpr<CoefficientT> t_constant, SparseVector<KeyT, idol::AffExpr<CoefficientT>> t_linear)
        : m_constant(std::move(t_constant)), m_linear(std::move(t_linear)) {}

    auto& constant() { return m_constant; }

    [[nodiscard]] const auto& constant() const { return m_constant; }

    auto& linear() { return m_linear; }

    [[nodiscard]] const auto& linear() const { return m_linear; }

    [[nodiscard]] double generate_constant(const Point<CoefficientT>& t_values) const {
        return evaluate(m_constant, t_values);
    }

    [[nodiscard]] LinExpr<KeyT> generate_linear(const Point<CoefficientT>& t_values) const {
        LinExpr<KeyT> result;

        for (const auto& [key, value] : m_linear) {
            result += key * evaluate(value, t_values);
        }

        return result;
    }

    AffExpr<KeyT> operator()(const Point<CoefficientT>& t_values) const {
        AffExpr<KeyT> result(generate_linear(t_values));
        result.constant() = generate_constant(t_values);
        return result;
    }

    GenerationPattern operator+=(const AffExpr<CoefficientT>& t_other) {
        m_constant += t_other.constant();
        for (const auto& [var, coeff] : t_other.linear()) {
            m_linear.set(var, m_linear.get(var) + coeff);
        }
        return *this;
    }

    GenerationPattern operator-=(const AffExpr<CoefficientT>& t_other) {
        m_constant -= t_other;
        return *this;
    }

    GenerationPattern operator+=(const GenerationPattern& t_other) {
        m_constant += t_other.m_constant;
        m_linear += t_other.m_linear;
        return *this;
    }

    GenerationPattern operator+=(double t_scalar) {
        m_constant += t_scalar;
        return *this;
    }

    GenerationPattern operator-=(const GenerationPattern& t_other) {
        m_constant -= t_other.m_constant;
        m_linear -= t_other.m_linear;
        return *this;
    }

    GenerationPattern operator*=(const double t_scalar) {
        m_constant *= t_scalar;
        m_linear *= t_scalar;
        return *this;
    }

    GenerationPattern operator/=(const double t_scalar) {
        m_constant /= t_scalar;
        m_linear /= t_scalar;
        return *this;
    }

    GenerationPattern operator*=(const KeyT& t_key) {
        if (!m_linear.empty()) {
            throw Exception("Trying to build a quadratic generation pattern.");
        }
        if (m_constant.is_zero(Tolerance::Sparsity)) {
            return *this;
        }
        m_linear.set(t_key, m_constant);
        m_constant.clear();
        return *this;
    }

protected:
    idol::AffExpr<CoefficientT> m_constant;
    SparseVector<KeyT, idol::AffExpr<CoefficientT>> m_linear;
};

namespace idol {

    template<class GeneratedT, class CoefficientT>
    std::ostream& operator<<(std::ostream& t_os, const idol::GenerationPattern<GeneratedT, CoefficientT>& t_gen) {
        const auto print_constant = [&](const idol::AffExpr<CoefficientT>& t_constant) {

            t_os << t_constant.constant();

            for (const auto& [key, value] : t_constant.linear()) {
                t_os << " + " << value << " !" << key;
            }

        };

        print_constant(t_gen.constant());
        for (const auto& [key, value] : t_gen.linear()) {
            t_os << " + ";
            if (value.linear().empty()) {
                print_constant(value);
            } else {
                t_os << "(";
                print_constant(value);
                t_os << ")";
            }
            t_os << " " << key;
        }

        return t_os;
    }

    template<class GeneratedT, class CoefficientT>
    idol::GenerationPattern<GeneratedT, CoefficientT>
    operator+(const CoefficientT &t_gen, const idol::GenerationPattern<GeneratedT, CoefficientT> &t_other) {
        auto result = t_other;
        result += idol::AffExpr<CoefficientT>(t_gen);
        return result;
    }

    template<class GeneratedT, class CoefficientT>
    idol::GenerationPattern<GeneratedT, CoefficientT> operator+(const idol::LinExpr<CoefficientT> &t_gen,
                                                                const idol::GenerationPattern<GeneratedT, CoefficientT> &t_other) {
        auto result = t_other;
        result += idol::AffExpr<CoefficientT>(t_gen);
        return result;
    }

    template<class GeneratedT, class CoefficientT>
    idol::GenerationPattern<GeneratedT, CoefficientT> operator+(const idol::AffExpr<CoefficientT> &t_gen,
                                                                const idol::GenerationPattern<GeneratedT, CoefficientT> &t_other) {
        auto result = t_other;
        result += t_gen;
        return result;
    }

    template<class GeneratedT, class CoefficientT>
    idol::GenerationPattern<GeneratedT, CoefficientT>
    operator*(const idol::GenerationPattern<GeneratedT, CoefficientT> &t_gen,
              const typename idol::GenerationPattern<GeneratedT, CoefficientT>::KeyT &t_key) {
        auto result = t_gen;
        result *= t_key;
        return result;
    }

    template<class GeneratedT, class CoefficientT>
    idol::GenerationPattern<GeneratedT, CoefficientT>
    operator*(const typename idol::GenerationPattern<GeneratedT, CoefficientT>::KeyT &t_key,
              const idol::GenerationPattern<GeneratedT, CoefficientT> &t_gen) {
        auto result = t_gen;
        result *= t_key;
        return result;
    }

    template<class GeneratedT, class CoefficientT>
    idol::GenerationPattern<GeneratedT, CoefficientT>
    operator+(const idol::GenerationPattern<GeneratedT, CoefficientT> &t_gen1,
              const idol::GenerationPattern<GeneratedT, CoefficientT> &t_gen2) {
        auto result = t_gen1;
        result += t_gen2;
        return result;
    }

    template<class GeneratedT, class CoefficientT>
    idol::GenerationPattern<GeneratedT, CoefficientT>
    operator-(const idol::GenerationPattern<GeneratedT, CoefficientT> &t_gen1,
              const idol::GenerationPattern<GeneratedT, CoefficientT> &t_gen2) {
        auto result = t_gen1;
        result -= t_gen2;
        return result;
    }

    template<class GeneratedT, class CoefficientT>
    idol::GenerationPattern<GeneratedT, CoefficientT>
    operator+(const idol::GenerationPattern<GeneratedT, CoefficientT> &t_gen, double t_scalar) {
        auto result = t_gen;
        result += t_scalar;
        return result;
    }

    template<class GeneratedT, class CoefficientT>
    idol::GenerationPattern<GeneratedT, CoefficientT>
    operator-(const idol::GenerationPattern<GeneratedT, CoefficientT> &t_gen, double t_scalar) {
        auto result = t_gen;
        result -= t_scalar;
        return result;
    }

    template<class GeneratedT, class CoefficientT>
    idol::GenerationPattern<GeneratedT, CoefficientT>
    operator+(double t_scalar, const idol::GenerationPattern<GeneratedT, CoefficientT> &t_gen) {
        auto result = t_gen;
        result += t_scalar;
        return result;
    }

    template<class GeneratedT, class CoefficientT>
    idol::GenerationPattern<GeneratedT, CoefficientT>
    operator-(double t_scalar, const idol::GenerationPattern<GeneratedT, CoefficientT> &t_gen) {
        auto result = t_gen;
        result -= t_scalar;
        return result;
    }

    template<class GeneratedT, class CoefficientT>
    idol::GenerationPattern<GeneratedT, CoefficientT>
    operator*(const double t_scalar, const idol::GenerationPattern<GeneratedT, CoefficientT> &t_gen) {
        auto result = t_gen;
        result *= t_scalar;
        return result;
    }

    template<class GeneratedT, class CoefficientT>
    idol::GenerationPattern<GeneratedT, CoefficientT>
    operator*(const idol::GenerationPattern<GeneratedT, CoefficientT> &t_gen, const double t_scalar) {
        auto result = t_gen;
        result *= t_scalar;
        return result;
    }

    template<class GeneratedT, class CoefficientT>
    idol::GenerationPattern<GeneratedT, CoefficientT>
    operator/(const idol::GenerationPattern<GeneratedT, CoefficientT> &t_gen, const double t_scalar) {
        auto result = t_gen;
        result /= t_scalar;
        return result;
    }

    static idol::GenerationPattern<idol::Ctr> operator!(const idol::Var &t_var) {
        idol::GenerationPattern<idol::Ctr> cut;
        cut.constant().linear().set(t_var, 1);
        return cut;
    }

    template<class GeneratedT, class CoefficientT>
    idol::GenerationPattern<GeneratedT, CoefficientT>
    operator!(const idol::GenerationPattern<GeneratedT, CoefficientT> &t_gen) {
        idol::GenerationPattern<GeneratedT, CoefficientT> result;
        result.constant().constant() = t_gen.constant().constant();
        for (const auto &[key, value]: t_gen.constant().linear()) {
            result.linear().set(key, result.linear().get(key) + value);
        }
        for (const auto &[key, coefficient]: t_gen.linear()) {
            result.constant().linear().set(key, result.constant().linear().get(key) + coefficient.constant());
            for (const auto &[key2, value]: coefficient.linear()) {
                result.linear().set(key2, result.linear().get(key2) + value * key);
            }
        }
        return result;
    }
}

#endif //IDOL_GENERATIONPATTERN_H
