//
// Created by henri on 01/09/22.
//

#ifndef OPTIMIZE_IMPL_VARIABLE_H
#define OPTIMIZE_IMPL_VARIABLE_H

#include "numericals.h"
#include "../Types.h"
#include "impl_Object.h"

template<enum Player> class Model;

namespace impl {
    class Variable;
}

class impl::Variable : public impl::Object {
    double m_lower_bound;
    double m_upper_bound;
    VariableType m_type;
    const unsigned int m_index;
public:
    Variable(Env& t_env, unsigned int t_index, double t_lower_bound, double t_upper_bound, VariableType t_type, std::string&& t_name);

    Variable(const Variable&) = delete;
    Variable(Variable&&) noexcept = default;

    Variable& operator=(const Variable&) = delete;
    Variable& operator=(Variable&&) noexcept = delete;

    [[nodiscard]] inline unsigned int index() const;

    [[nodiscard]] inline double lower_bound() const;

    [[nodiscard]] inline double upper_bound() const;

    [[nodiscard]] inline VariableType type() const;

    inline void set_lower_bound(double t_lower_bound);

    inline void set_upper_bound(double t_upper_bound);

    inline void set_type(VariableType t_type);
};

unsigned int impl::Variable::index() const {
    return m_index;
}

double impl::Variable::lower_bound() const {
    return m_lower_bound;
}

double impl::Variable::upper_bound() const {
    return m_upper_bound;
}

VariableType impl::Variable::type() const {
    return m_type;
}

void impl::Variable::set_lower_bound(double t_lower_bound) {
    m_lower_bound = t_lower_bound;
}

void impl::Variable::set_upper_bound(double t_upper_bound) {
    m_upper_bound = t_upper_bound;
}

void impl::Variable::set_type(VariableType t_type) {
    m_type = t_type;
}

#endif //OPTIMIZE_IMPL_VARIABLE_H
