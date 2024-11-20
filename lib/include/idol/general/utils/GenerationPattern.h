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
    using KeyT = std::conditional_t<std::is_same_v<GeneratedT, Var>, Ctr, Var>;
    idol::AffExpr<CoefficientT> m_constant;
    SparseVector<KeyT, idol::AffExpr<CoefficientT>> m_linear;
public:
    GenerationPattern() = default;

    GenerationPattern(idol::AffExpr<CoefficientT> t_constant, SparseVector<KeyT, idol::AffExpr<CoefficientT>> t_linear)
        : m_constant(std::move(t_constant)), m_linear(std::move(t_linear)) {}

    auto& constant() { return m_constant; }

    const auto& constant() const { return m_constant; }

    auto& linear() { return m_linear; }

    const auto& linear() const { return m_linear; }

    double generate_constant(const Point<CoefficientT>& t_values) const {
        return evaluate(m_constant, t_values);
    }

    LinExpr<KeyT> generate_linear(const Point<CoefficientT>& t_values) const {
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

};

#endif //IDOL_GENERATIONPATTERN_H
