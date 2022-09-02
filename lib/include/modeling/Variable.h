//
// Created by henri on 01/09/22.
//

#ifndef OPTIMIZE_VARIABLE_H
#define OPTIMIZE_VARIABLE_H

#include "impl_Variable.h"
#include "Object.h"

template<enum Player PlayerT>
class Variable : public Object<impl::Variable> {
public:
    explicit Variable(impl::Variable* t_impl);

    Variable(const Variable&) = default;
    Variable(Variable&&) noexcept = default;

    Variable& operator=(const Variable&) = default;
    Variable& operator=(Variable&&) noexcept = default;

    [[nodiscard]] double lower_bound() const;

    [[nodiscard]] double upper_bound() const;

    [[nodiscard]] VariableType type() const;
};

template<enum Player PlayerT>
Variable<PlayerT>::Variable(impl::Variable *const t_impl) : Object<impl::Variable>(t_impl) {

}

template<enum Player PlayerT>
double Variable<PlayerT>::lower_bound() const {
    return impl().lower_bound();
}

template<enum Player PlayerT>
double Variable<PlayerT>::upper_bound() const {
    return impl().upper_bound();
}

template<enum Player PlayerT>
VariableType Variable<PlayerT>::type() const {
    return impl().type();
}

template<enum Player PlayerT>
struct std::hash<Variable<PlayerT>> {
    std::size_t operator()(const Variable<PlayerT>& t_variable) const {
        return std::hash<unsigned int>()(t_variable.id());
    }
};

template<enum Player PlayerT>
struct std::equal_to<Variable<PlayerT>> {
    std::size_t operator()(const Variable<PlayerT>& t_a, const Variable<PlayerT>& t_b) const {
        return t_a.id() == t_b.id();
    }
};

#endif //OPTIMIZE_VARIABLE_H
