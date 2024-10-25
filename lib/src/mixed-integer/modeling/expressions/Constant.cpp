//
// Created by henri on 07/09/22.
//
#include "idol/mixed-integer/modeling/expressions/Constant.h"
#include "idol/general/numericals.h"
#include "idol/general/utils/Point.h"
#include <memory>

idol::Constant idol::Constant::Zero;
idol::Map<idol::Param, double> idol::Constant::s_empty_linear_terms {};
idol::Map<idol::Pair<idol::Param, idol::Param>, double, idol::impl::symmetric_pair_hash, idol::impl::symmetric_pair_equal_to> idol::Constant::s_empty_quadratic_terms {};

idol::Constant::Constant(const idol::Constant &t_src)
    : m_constant(t_src.m_constant),
      m_linear_terms(t_src.m_linear_terms ? std::make_unique<MapForLinearTerms>(*t_src.m_linear_terms) : std::unique_ptr<MapForLinearTerms >()),
      m_quadratic_terms(t_src.m_quadratic_terms ? std::make_unique<MapForQuadraticTerms >(*t_src.m_quadratic_terms) : std::unique_ptr<MapForQuadraticTerms >()) {

}

idol::Constant::Constant(const Param &t_param, double t_value)
    : m_linear_terms(std::make_unique<MapForLinearTerms>(MapForLinearTerms { {t_param, t_value} })) {

    if (equals(t_value, 0., Tolerance::Sparsity)) {
        m_linear_terms.reset();
    }

}

idol::Constant::Constant(const idol::Param &t_param_1, const idol::Param &t_param_2, double t_value)  {

    if (equals(t_value, 0., Tolerance::Sparsity)) {
        return;
    }

    m_quadratic_terms = std::make_unique<MapForQuadraticTerms>();

    m_quadratic_terms->emplace(Pair(t_param_1, t_param_2), t_value);

}

idol::Constant::Constant(double t_constant) : m_constant(t_constant) {

}

void idol::Constant::set(const Param &t_param, double t_value) {

    if (equals(t_value, 0., Tolerance::Sparsity)) {

        if (!m_linear_terms) {
            return;
        }

        m_linear_terms->erase(t_param);
        delete_linear_map_if_empty();
        return;
    }

    create_linear_map_if_not_exists();

    auto [it, success] = m_linear_terms->emplace(t_param, t_value);
    if (!success) {
        it->second = t_value;
    }

}

void idol::Constant::set(const idol::Param &t_param_1, const idol::Param &t_param_2, double t_value) {

    if (equals(t_value, 0., Tolerance::Sparsity)) {

        if (!m_quadratic_terms) {
            return;
        }

        m_quadratic_terms->erase(Pair( t_param_1, t_param_2 ));
        delete_quadratic_map_if_empty();
        return;
    }

    create_quadratic_map_if_not_exists();

    auto [it, success] = m_quadratic_terms->emplace(Pair(t_param_1, t_param_2), t_value);
    if (!success) {
        it->second = t_value;
    }
}

double idol::Constant::get(const Param &t_param) const {

    if (!m_linear_terms) {
        return 0.;
    }

    auto it = m_linear_terms->find(t_param);
    return it == m_linear_terms->end() ? 0. : it->second;

}

double idol::Constant::get(const idol::Param &t_param_1, const idol::Param &t_param_2) const {

    if (!m_quadratic_terms) {
        return 0.;
    }

    auto it = m_quadratic_terms->find(Pair(t_param_1, t_param_2));
    return it == m_quadratic_terms->end() ? 0. : it->second;
}

idol::Constant &idol::Constant::operator*=(double t_factor) {

    if (equals(t_factor, 0., Tolerance::Sparsity)) {

        m_constant = 0;
        m_linear_terms.reset();
        m_quadratic_terms.reset();

        return *this;
    }

    m_constant *= t_factor;

    if (m_linear_terms) {

        for (auto &[param, value]: *m_linear_terms) {
            value *= t_factor;
        }

    }

    if (m_quadratic_terms) {

        for (auto &[pair, value]: *m_quadratic_terms) {
            value *= t_factor;
        }

    }

    return *this;
}

idol::Constant &idol::Constant::operator/=(double t_coefficient) {

    if (equals(t_coefficient, 1., Tolerance::Sparsity)) {
        return *this;
    }

    m_constant /= t_coefficient;

    if (m_linear_terms) {

        for (auto &[param, value]: *m_linear_terms) {
            value /= t_coefficient;
        }

    }

    if (m_quadratic_terms) {

        for (auto& [pair, value] : *m_quadratic_terms) {
            value /= t_coefficient;
        }

    }

    return *this;
}

idol::Constant &idol::Constant::operator+=(double t_term) {
    m_constant += t_term;
    return *this;
}

idol::Constant &idol::Constant::operator+=(const Param& t_term) {
    insert_or_add(t_term, 1.);
    return *this;
}

idol::Constant &idol::Constant::operator+=(const Constant &t_term) {
    m_constant += t_term.m_constant;
    for (auto [param, value] : t_term.linear()) {
        insert_or_add(param, value);
    }
    for (auto [pair, value] : t_term.quadratic()) {
        insert_or_add(pair.first, pair.second, value);
    }
    return *this;
}

idol::Constant &idol::Constant::operator-=(double t_term) {
    m_constant -= t_term;
    return *this;
}

idol::Constant &idol::Constant::operator-=(Param t_term) {
    insert_or_add(t_term, -1.);
    return *this;
}

idol::Constant &idol::Constant::operator-=(const Constant &t_term) {
    m_constant -= t_term.m_constant;
    for (auto [param, value] : t_term.linear()) {
        insert_or_add(param, -value);
    }
    for (auto [pair, value] : t_term.quadratic()) {
        insert_or_add(pair.first, pair.second, -value);
    }
    return *this;
}

void idol::Constant::insert_or_add(const Param &t_param, double t_value) {

    if (equals(t_value, 0., Tolerance::Sparsity)) {
        return;
    }

    create_linear_map_if_not_exists();

    auto [it, success] = m_linear_terms->emplace(t_param, t_value);
    if (!success) {
        it->second += t_value;
        if (equals(it->second, 0., Tolerance::Sparsity)) {
            m_linear_terms->erase(it);
            delete_linear_map_if_empty();
        }
    }

}

void idol::Constant::insert_or_add(const idol::Param &t_param_1, const idol::Param &t_param_2, double t_value) {

    if (equals(t_value, 0., Tolerance::Sparsity)) {
        return;
    }

    create_quadratic_map_if_not_exists();

    auto [it, success] = m_quadratic_terms->emplace(Pair(t_param_1, t_param_2), t_value);
    if (!success) {
        it->second += t_value;
        if (equals(it->second, 0., Tolerance::Sparsity)) {
            m_quadratic_terms->erase(it);
            delete_quadratic_map_if_empty();
        }
    }
}

bool idol::Constant::is_zero() const {
    return equals(m_constant, 0., Tolerance::Sparsity) && is_numerical();
}

bool idol::Constant::is_numerical() const {
    return (!m_linear_terms || m_linear_terms->empty()) && (!m_quadratic_terms || m_quadratic_terms->empty());
}

double idol::Constant::fix(const PrimalPoint &t_primals) const {
    double result = m_constant;

    if (m_linear_terms) {

        for (const auto &[param, coefficient]: *m_linear_terms) {
            result += coefficient * t_primals.get(param.as<Var>());
        }

    }

    if (m_quadratic_terms) {

        for (const auto &[pair, coefficient]: *m_quadratic_terms) {
            const auto [param1, param2] = pair;
            result += coefficient * t_primals.get(param1.as<Var>()) * t_primals.get(param2.as<Var>());
        }

    }

    return result;
}

double idol::Constant::fix(const DualPoint &t_duals) const {
    double result = m_constant;

    if (m_linear_terms) {

        for (const auto &[param, coeff]: *m_linear_terms) {
            result += coeff * t_duals.get(param.as<Ctr>());
        }

    }

    if (m_quadratic_terms) {

        for (const auto &[pair, coeff]: *m_quadratic_terms) {
            const auto [param1, param2] = pair;
            result += coeff * t_duals.get(param1.as<Ctr>()) * t_duals.get(param2.as<Ctr>());
        }

    }

    return result;
}

double idol::Constant::as_numerical() const {
    if (!is_numerical()) {
        throw Exception("Non-numerical found when converting constant to numerical.");
    }
    return numerical();
}

void idol::Constant::round() {

    m_constant = std::round(m_constant);

    if (m_linear_terms) {
        for (auto &[param, coeff]: *m_linear_terms) {
            coeff = std::round(coeff);
        }
    }

    if (m_quadratic_terms) {

        for (auto &[params, coeff]: *m_quadratic_terms) {
            coeff = std::round(coeff);
        }

    }

}

idol::Constant &idol::Constant::operator=(const idol::Constant &t_rhs) {

    if (this == &t_rhs) {
        return *this;
    }

    m_constant = t_rhs.m_constant;

    if (t_rhs.m_linear_terms) {

        m_linear_terms = std::make_unique<MapForLinearTerms>(*t_rhs.m_linear_terms);

    } else {

        m_linear_terms.reset();

    }

    if (t_rhs.m_quadratic_terms) {

        m_quadratic_terms = std::make_unique<MapForQuadraticTerms>(*t_rhs.m_quadratic_terms);

    } else {

        m_quadratic_terms.reset();

    }

    return *this;
}

void idol::Constant::create_linear_map_if_not_exists() {

    if (m_linear_terms) {
        return;
    }

    m_linear_terms = std::make_unique<MapForLinearTerms>();

}

void idol::Constant::delete_linear_map_if_empty() {

    if (!m_linear_terms || !m_linear_terms->empty()) {
        return;
    }

    m_linear_terms.reset();

}

void idol::Constant::create_quadratic_map_if_not_exists() {

    if (m_quadratic_terms) {
        return;
    }

    m_quadratic_terms = std::make_unique<MapForQuadraticTerms >();
}

void idol::Constant::delete_quadratic_map_if_empty() {

    if (!m_quadratic_terms || !m_quadratic_terms->empty()) {
        return;
    }

    m_quadratic_terms.reset();

}
