//
// Created by henri on 07/09/22.
//
#include "idol/modeling/expressions/Constant.h"
#include "idol/modeling/numericals.h"
#include "idol/modeling/solutions/Solution.h"
#include <memory>

idol::Constant idol::Constant::Zero;
idol::Map<idol::Param, double> idol::Constant::s_empty_linear_terms {};
idol::Map<idol::Pair<idol::Param, idol::Param>, double, idol::impl::symmetric_pair_hash, idol::impl::symmetric_pair_equal_to> idol::Constant::s_empty_quadratic_terms {};

idol::Constant::Constant(const idol::Constant &t_src)
    : m_constant(t_src.m_constant) //,
//m_linear_terms(t_src.m_linear_terms ? std::make_unique<MapForLinearTerms>(*t_src.m_linear_terms) : std::unique_ptr<MapForLinearTerms >()),
//    m_quadratic_terms(t_src.m_quadratic_terms ? std::make_unique<MapForQuadraticTerms >(*t_src.m_quadratic_terms) : std::unique_ptr<MapForQuadraticTerms >()) {
{
}

idol::Constant::Constant(const Param &t_param, double t_value) {



}

idol::Constant::Constant(const idol::Param &t_param_1, const idol::Param &t_param_2, double t_value)  {

    if (equals(t_value, 0., Tolerance::Sparsity)) {
        return;
    }


}

idol::Constant::Constant(double t_constant) : m_constant(t_constant) {

}

void idol::Constant::set(const Param &t_param, double t_value) {

    if (equals(t_value, 0., Tolerance::Sparsity)) {

        return;
    }


}

void idol::Constant::set(const idol::Param &t_param_1, const idol::Param &t_param_2, double t_value) {

}

double idol::Constant::get(const Param &t_param) const {
    return 0;
}

double idol::Constant::get(const idol::Param &t_param_1, const idol::Param &t_param_2) const {
    return 0;
}

idol::Constant &idol::Constant::operator*=(double t_factor) {

    if (equals(t_factor, 0., Tolerance::Sparsity)) {

        m_constant = 0;

        return *this;
    }

    m_constant *= t_factor;

    return *this;
}

idol::Constant &idol::Constant::operator/=(double t_coefficient) {

    if (equals(t_coefficient, 1., Tolerance::Sparsity)) {
        return *this;
    }

    m_constant /= t_coefficient;

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

}

void idol::Constant::insert_or_add(const idol::Param &t_param_1, const idol::Param &t_param_2, double t_value) {

    if (equals(t_value, 0., Tolerance::Sparsity)) {
        return;
    }

}

bool idol::Constant::is_zero() const {
    return equals(m_constant, 0., Tolerance::Sparsity) && is_numerical();
}

bool idol::Constant::is_numerical() const {
    return true;
}

double idol::Constant::fix(const Solution::Primal &t_primals) const {
    double result = m_constant;

    return result;
}

double idol::Constant::fix(const Solution::Dual &t_duals) const {
    double result = m_constant;

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

}

idol::Constant &idol::Constant::multiply_with_precision(double t_factor, unsigned int t_n_digits) {

    m_constant = ::idol::multiply_with_precision(m_constant, t_factor, t_n_digits);

    return *this;
}

idol::Constant &
idol::Constant::multiply_with_precision_by_power_of_10(unsigned int t_exponent, unsigned int t_n_digits) {

    m_constant = ::idol::multiply_with_precision_by_power_of_10(m_constant, t_exponent, t_n_digits);

    return *this;
}

idol::Constant &idol::Constant::operator=(const idol::Constant &t_rhs) {

    if (this == &t_rhs) {
        return *this;
    }

    m_constant = t_rhs.m_constant;

    return *this;
}

void idol::Constant::create_linear_map_if_not_exists() {

}

void idol::Constant::delete_linear_map_if_empty() {

}

void idol::Constant::create_quadratic_map_if_not_exists() {

}

void idol::Constant::delete_quadratic_map_if_empty() {

}
